#include "libKinectInterface2.h"
#include "libSkeleton.h"

#include "timers.h"
#include <fstream>


#pragma comment (lib, "Msdmo")

extern HANDLE kinectMutex;
static int r=0;

// Infrared camera parameters
const float KinectThread::InfraredSourceValueMaximum = static_cast<float>(USHRT_MAX);
const float KinectThread::InfraredOutputValueMinimum = 0.01f;
const float KinectThread::InfraredOutputValueMaximum = 1.0f;
const float KinectThread::InfraredSceneValueAverage = 0.08f;
const float KinectThread::InfraredSceneStandardDeviations = 3.0f;


/**
* Safe release for interfaces
*/
template <class Interface>
inline void SafeRelease (Interface *& pInterfaceToRelease)
{
	if (pInterfaceToRelease != NULL)
	{
		pInterfaceToRelease -> Release ();
		pInterfaceToRelease = NULL;
	}
}

/**
 *	Safe delete for objects created with new
 */
template <class Object>
inline void SafeDelete (Object *& pObjectToDelete)
{
	if (pObjectToDelete != NULL)
	{
		delete pObjectToDelete;
		pObjectToDelete = NULL;
	}
}


/**
* Kinect thread class constructor
*/
KinectThread::KinectThread(int index, bool capture_color, bool capture_depth, bool capture_skeleton, bool capture_audio):
cam_index(index),
	bThreadExit(false),
	bCaptureColor(capture_color),
	bCaptureDepth(capture_depth),
	bCaptureInfrared(true),
	bColorMapping(true),
	bTrackPlayer(true),
	bCaptureSkeleton(capture_skeleton),
	updateC2WTransform(false),
	m_pKinectSensor(nullptr),
	m_pMultiSourceFrameReader(nullptr),
	m_pBodyFrameReader(nullptr),
	m_pColorRGBX(nullptr),
	m_pDepthRGBX(nullptr),
	m_pInfraredRGBX(nullptr),
	m_pDepthCoordinates(nullptr),
	m_pSkeletonSet(nullptr),
	m_skeletonSequenceBuffer(nullptr),
	m_currentFrame(0),
	m_bSaveSkeletons(false),
	m_bBufferSkeletons(false),
	streamPoses(0),
	m_oldTimeDouble(0),
	m_collectedFrames(0),
	m_fps(0),
	pImageWriter(nullptr), //.FO.
	m_bSaveImageFrames(false), //.FO.
	m_bStopSaveThread(false) //.FO.
{
	sumGroundPlaneOffset = 0;
	nGroundPlaneVotes = 0;

	// Create events
	hStartEvent = CreateEvent(NULL,TRUE,FALSE,TEXT("ThreadStart"));
	hKinectReadyEvent = CreateEvent(NULL,TRUE,FALSE,TEXT("KinectReady"));
	hStopEvent = CreateEvent(NULL,TRUE,FALSE,TEXT("ThreadStop"));			// make it manual reset, otherwise it will set itself all the time
	hNewFrameEvent = CreateEvent(NULL, FALSE, FALSE, TEXT("KinectNewFrame"));

	// Create buffers for Kinect
	m_pColorRGBX = new RGBQUAD[cColorWidth * cColorHeight];
	m_pDepthRGBX = new RGBQUAD[cDepthWidth * cDepthHeight];
	m_pInfraredRGBX = new RGBQUAD[cInfraredWidth * cInfraredHeight];

	// create heap storage for the coordinate mapping from color to depth
	m_pDepthCoordinates = new DepthSpacePoint[cColorWidth * cColorHeight];

	// Create openCV images/buffers
	m_cvImageBufferRGB = cv::Mat(cv::Size(cColorWidth, cColorHeight), CV_8UC3);
	m_cvImageMappedBuffer = cv::Mat(cvSize(cColorWidth, cColorHeight), CV_8UC3);
	m_cvDepthBuffer = cv::Mat(cvSize(cDepthWidth, cDepthHeight), CV_8UC3);
	m_cvDepthBufferRaw = cv::Mat(cvSize(cDepthWidth, cDepthHeight), CV_16U);
	m_cvInfraredBuffer = cv::Mat(cvSize(cInfraredWidth, cInfraredHeight), CV_8UC3);
	m_cvBodyIndexBufferRaw = cv::Mat(cvSize(cDepthWidth, cDepthHeight), CV_8U);

	// initialize storage for our skeleton
	m_pSkeletonSet = new SkeletonSet();
	m_pSkeletonSet->resize(BODY_COUNT);	// we'll have 6 skeletons
	m_skeletonSequenceBuffer = new SkeletonSetSequence();

	// initialize camera to world transformation. TODO: Flip axes to have the same coordinate space as in Stepan's code
	m_matCameraToWorld = Eigen::Matrix4f::Identity();


	// Initialize Critical section
	InitializeCriticalSection(&criticalSection);
	//hCaptureMutex = CreateMutex(NULL, TRUE, NULL);	// this is done in the live part

	// Initialize threading
	//.FO. hCaptureThread = CreateThread (NULL, 0, captureThreadFunc, (LPVOID)this, 0, NULL);
	unsigned stId;
	HANDLE sh = (HANDLE) _beginthreadex( NULL, 0, captureThreadFunc, this, CREATE_SUSPENDED, &stId);
	ResumeThread(sh);
	// create thread and wait
	SetEvent(hStartEvent);

	

	DWORD dwTimeout = WaitForSingleObject(hKinectReadyEvent, INFINITE);
	if (dwTimeout == WAIT_TIMEOUT) {
		throw ("KinectThread::KinectThread(): Cannot get Kinect to run.");
	}
}


/**
* Kinect thread class destructor
*/
KinectThread::~KinectThread() {

	// TODO: add some error checking
	EnterCriticalSection(&criticalSection);

	// done with color frame reader
	SafeRelease(m_pMultiSourceFrameReader);
	SafeRelease(m_pBodyFrameReader);

	// close the Kinect Sensor
	if (m_pKinectSensor)
	{
		m_pKinectSensor->Close();
	}

	SafeRelease(m_pKinectSensor);

	// clear OpenCV stuff
	m_cvImageBufferRGB.release();
	m_cvDepthBuffer.release();
	m_cvDepthBufferRaw.release();
	m_cvInfraredBuffer.release();
	m_cvBodyIndexBufferRaw.release();
	m_cvImageMappedBuffer.release();


	// skeleton
	SafeDelete(m_pSkeletonSet);
	SafeDelete(m_skeletonSequenceBuffer);

	// clear Kinect buffers
	if (m_pColorRGBX)
	{
		delete [] m_pColorRGBX;
		m_pColorRGBX = nullptr;
	}

	if (m_pDepthRGBX)
	{
		delete [] m_pDepthRGBX;
		m_pDepthRGBX = nullptr;
	}

	if (m_pInfraredRGBX)
	{
		delete [] m_pInfraredRGBX;
		m_pInfraredRGBX = nullptr;
	}

	if (m_pDepthCoordinates)
	{
		delete [] m_pDepthCoordinates;
		m_pDepthCoordinates = nullptr;
	}

	LeaveCriticalSection(&criticalSection);
	DeleteCriticalSection(&criticalSection);
	
}

/**
* Kinect initialization
*/
void KinectThread::initialize() 
{

	// Create Kinect stuff
	HRESULT hr = initializekinect();
	std::cout << "Kinect init: " << (hr == 0 ? "success" : " failed") << std::endl;
	if (FAILED(hr))
	{
		throw Exception("Error in KinectThread::initialize(): Cannot start Kinect camera.");	/// TODO: Throw an error
	}

	ResetEvent(hStopEvent);

	// Done with Kinect initialization, signal main loop to continue
	SetEvent(hKinectReadyEvent);

	//.FO. initialize writer thread if requested
	if (!pImageWriter)
	{
		threadConfig.bSave = &m_bSaveImageFrames;
		threadConfig.bStop = &m_bStopSaveThread;
		threadConfig.imagePath = &m_imageSavePath;
		pImageWriter = new ImageWriter(&m_cvImageBufferRGB, &m_cvDepthBuffer, &threadConfig);
		ResumeThread(pImageWriter->getThreadHandle());
	}

	// Done with Kinect initialization, signal main loop to continue
	std::cout << "Starting Kinect capture..." << std::endl;

}

/** 
* Stop thread
*/
void KinectThread::stop() {
	
	// only stop if not already stopped
	if (!bThreadExit)
	{
		bThreadExit = true;
		::Sleep(30000);
		WaitForSingleObject(hStopEvent, INFINITE);
	//if (kinectAudioStream) kinectAudioStream->StopCapture();
	}
}


HRESULT KinectThread::initializekinect()
{
	HRESULT hr;
	hr = GetDefaultKinectSensor(&m_pKinectSensor);
	if (FAILED (hr)) throw ("KinectThread::initializekinect():: Cannot get Kinect to work: %08x", hr);

	BOOLEAN status;
	hr = m_pKinectSensor->get_IsAvailable(&status);
	if (FAILED (hr) ) throw ("KinectThread::initializekinect(): Kinect is not available: %08x", hr);

	IColorFrameSource* pColorFrameSource = NULL;
	IDepthFrameSource* pDepthFrameSource = NULL;
	IInfraredFrameSource* pInfraredFrameSource = NULL;
	IMultiSourceFrame* pMultiSourceFrame = NULL;

	if (m_pKinectSensor)
	{
		// get coordinate mapper
		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_CoordinateMapper(&m_pCoordinateMapper);
		}

		// Initialize the Kinect 
		hr=NULL;
		hr = m_pKinectSensor->Open();

		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->OpenMultiSourceFrameReader(
				FrameSourceTypes::FrameSourceTypes_Depth | FrameSourceTypes::FrameSourceTypes_Color | FrameSourceTypes::FrameSourceTypes_BodyIndex |
				FrameSourceTypes::FrameSourceTypes_Infrared,
				&m_pMultiSourceFrameReader);

			if (FAILED(hr))
			{
				std::cout << "Error in KinectThread::initializekinect(): Cannot open multisrouce reader. " << std::endl;
			}
		}
		
		// skeleton reader
		IBodyFrameSource* pBodyFrameSource = NULL;
		if (SUCCEEDED(hr))
		{
			hr = m_pKinectSensor->get_BodyFrameSource(&pBodyFrameSource);
		}
		if (SUCCEEDED(hr))
		{
			hr = pBodyFrameSource->OpenReader(&m_pBodyFrameReader);
		}
		SafeRelease(pBodyFrameSource);


	}

	hr = m_pMultiSourceFrameReader->SubscribeMultiSourceFrameArrived(&hFrameEvent);
	if(FAILED(hr)) {
		throw exception("Couldn't subscribe frame");
	}


	if (!m_pKinectSensor || FAILED(hr))
		throw ("Cannot get Kinect", hr);
	return hr;
}
/**
* Main Kinect capture thread
*/
//.FO. DWORD WINAPI KinectThread::captureThreadFunc(LPVOID lpParam)
unsigned __stdcall KinectThread::captureThreadFunc(void * pThis)
{
	KinectThread * pthK = (KinectThread*)pThis;

	std::cout << "Starting Kinect capture thread ..." << std::endl;

	// initialize and start capture
	pthK->initialize();
	WaitForSingleObject (pthK->hStartEvent, INFINITE);
	::Sleep (10);
	// run capture loop
	while (!pthK->bThreadExit) {
		ThreadTimes tt;
		tt.tic();
		pthK->capture();
		tt.toc();
		//static int n = 0;
		std::cout << "Frame number: " << pthK->getFrameNumber() << " capture time: " << tt << std::endl;
		//::Sleep(20);	// TODO: figure out proper way to poll data with 30FPS
	}
	
	

	//.FO. wait for saving thread....
	if (pthK->pImageWriter)
	{
		std::cout << "wait for saving thread..." << std::endl;
		pthK->m_bStopSaveThread = true;
		std::cout << "right before waiting for pImageWriter handle... " << pthK->pImageWriter->getThreadHandle() << std::endl;
		WaitForSingleObject(pthK->pImageWriter->getThreadHandle(),INFINITE);
		std::cout << "saving thread done..." << std::endl;
	}

	// set stop even when loop is done
	SetEvent(pthK->hStopEvent);

	return 0;
}

/**
* Capture callback
*/
void KinectThread::capture ()
{
	// grab current timestamp
	UnixTimer currentTime;
	m_timestampSeconds = currentTime.seconds;
	m_timestampMicroSeconds = currentTime.microseconds;

	if (!m_pMultiSourceFrameReader)
	{
		return;
	}

	if (bCaptureSkeleton && !m_pBodyFrameReader)
	{
		return;
	}

	WaitForSingleObject (kinectMutex, INFINITE);

	IMultiSourceFrame* pMultiSourceFrame = NULL;
	IColorFrame* pColorFrame = NULL;
	IDepthFrame* pDepthFrame = NULL;
	IInfraredFrame* pInfraredFrame = NULL;
	IBodyIndexFrame* pBodyIndexFrame = NULL;

	IBodyFrame *pBodyFrame = NULL;



	// obtain multi source frame
	HRESULT hr = m_pMultiSourceFrameReader->AcquireLatestFrame(&pMultiSourceFrame);
	
	if (SUCCEEDED(hr))
	{
		
		// get color frame:
		IColorFrameReference* pColorFrameReference = NULL;
		if (SUCCEEDED(hr))
		{
			hr = pMultiSourceFrame->get_ColorFrameReference(&pColorFrameReference);
		}
		if (SUCCEEDED(hr))
		{
			hr = pColorFrameReference->AcquireFrame(&pColorFrame);
		}
		SafeRelease(pColorFrameReference);

		// get depth frame:
		IDepthFrameReference* pDepthFrameReference = NULL;

		hr = pMultiSourceFrame->get_DepthFrameReference(&pDepthFrameReference);
		if (SUCCEEDED(hr))
		{
			hr = pDepthFrameReference->AcquireFrame(&pDepthFrame);
		}
		SafeRelease(pDepthFrameReference);

		// get infrared
		IInfraredFrameReference* pInfraredFrameReference = NULL;

		hr = pMultiSourceFrame->get_InfraredFrameReference(&pInfraredFrameReference);
		if (SUCCEEDED(hr))
		{
			hr = pInfraredFrameReference->AcquireFrame(&pInfraredFrame);
		}
		SafeRelease(pInfraredFrameReference);

		// get body index:
		IBodyIndexFrameReference* pBodyIndexFrameReference = NULL;

		hr = pMultiSourceFrame->get_BodyIndexFrameReference(&pBodyIndexFrameReference);
		if (SUCCEEDED(hr))
		{
			hr = pBodyIndexFrameReference->AcquireFrame(&pBodyIndexFrame);
		}
		SafeRelease(pBodyIndexFrameReference);



		/*
		// This approach would be best to acquire skeleton frame at the same time as the rest
		// but for some unknown reason it does not work. AcuireFrame always seems to E_FAIL;

		// skeleton

		IBodyFrameReference* pBodyFrameReference = NULL;

		hr = pMultiSourceFrame->get_BodyFrameReference(&pBodyFrameReference);
		
		if (SUCCEEDED(hr))
		{
			hr = pBodyFrameReference->AcquireFrame(&pBodyFrame);
		}
		SafeRelease(pBodyFrame);
		*/

		// Alternative approach, use separate body reader
		hr = m_pBodyFrameReader->AcquireLatestFrame(&pBodyFrame);

		if (SUCCEEDED(hr))
		{
			if (bCaptureSkeleton)
			{
				updateSkeletons(pBodyFrame);
			}
		}



		// color stream, wait for next
		if (bCaptureColor) 
		{
			updateCaptureColor(pColorFrame);
		} // end of bCaptureColor

		// get depth stream and copy to buffer
		if (bCaptureDepth) 
		{
			updateCaptureDepth(pDepthFrame);
		} // end of bCaptureDepth

		if (bCaptureInfrared) 
		{
			updateCaptureInfrared(pInfraredFrame);
		} // end of bCaptureDepth

		if (bTrackPlayer)
		{
			updateCaptureBodyIndex(pBodyIndexFrame);
		}

		if (false && bColorMapping && bCaptureColor && bCaptureDepth)
		{
			ProcessColorMapping();
		}

		//.FO. add image frame to the frame writer
		if (pImageWriter) pImageWriter->addFrame(m_currentFrame, &m_cvImageBufferRGB, &m_cvDepthBufferRaw);

		//.FO. set event that new frame has been processed:
		EnterCriticalSection(&criticalSection);
		SetEvent(hNewFrameEvent);
		// let's update frame number here whenever a valid frame is obtained from Kinect
		m_currentFrame ++;
		LeaveCriticalSection(&criticalSection);

		// get the frame rate and update the frame rate counter every second
		//double timeDouble = static_cast<double>(m_timestampSeconds - m_oldTimestampSeconds) + static_cast<double>(m_timestampMicroSeconds - m_oldTimestampMicroSeconds)*1e-6;

		double currentTime = double (clock ()) / double (CLOCKS_PER_SEC);
		m_collectedFrames++;
		double deltaTime = 2.0;		// time in seconds
		if (currentTime-m_oldTimeDouble > deltaTime)
		{
			m_fps = static_cast<double>(m_collectedFrames) / deltaTime;
			m_oldTimeDouble = currentTime;
			m_collectedFrames = 0;
		}


#if 0
		DepthSpacePoint pt;
		CameraSpacePoint cpt;
		UINT16 depth;
		pt.X = 100;
		pt.Y = 100;
		depth = 2000.0;
		m_pCoordinateMapper->MapDepthPointsToCameraSpace(1, &pt, 1, &depth, 1, &cpt);
		std::cout << "pt = [" << pt.X << ", " << pt.Y << ", " << depth << "]" << std::endl;
		std::cout << "cpt = [" << cpt.X << ", " << cpt.Y << ", " << cpt.Z << "]" << std::endl;

#endif

		

	} // end of get multisource


	// take care of memory
	SafeRelease(pColorFrame);
	SafeRelease(pDepthFrame);
	SafeRelease(pInfraredFrame);
	SafeRelease(pBodyIndexFrame);
	SafeRelease(pBodyFrame);
	
	SafeRelease(pMultiSourceFrame);
	
	//.FO.
	//// set event that new frame has been processed:
	//EnterCriticalSection(&criticalSection);
	//SetEvent(hNewFrameEvent);
	//LeaveCriticalSection(&criticalSection);

	ReleaseMutex (kinectMutex);
}

//Functions related to capturing colored images and depth images
void KinectThread::updateCaptureColor(IColorFrame* pColorFrame)
{

    if (pColorFrame)
    {
        INT64 nTime = 0;
        IFrameDescription* pFrameDescription = NULL;
        int nWidth = 0;
        int nHeight = 0;
        ColorImageFormat imageFormat = ColorImageFormat_None;
        UINT nBufferSize = 0;
        RGBQUAD *pBuffer = NULL;

        hr = pColorFrame->get_RelativeTime(&nTime);

        if (SUCCEEDED(hr))
        {
            hr = pColorFrame->get_FrameDescription(&pFrameDescription);
        }

        if (SUCCEEDED(hr))
        {
            hr = pFrameDescription->get_Width(&nWidth);
        }

        if (SUCCEEDED(hr))
        {
            hr = pFrameDescription->get_Height(&nHeight);
        }

        if (SUCCEEDED(hr))
        {
            hr = pColorFrame->get_RawColorImageFormat(&imageFormat);
        }

        if (SUCCEEDED(hr))
        {
            if (imageFormat == ColorImageFormat_Bgra)
            {
                hr = pColorFrame->AccessRawUnderlyingBuffer(&nBufferSize, reinterpret_cast<BYTE**>(&pBuffer));
            }
            else if (m_pColorRGBX)
            {
                pBuffer = m_pColorRGBX;
                nBufferSize = cColorWidth * cColorHeight * sizeof(RGBQUAD);
                hr = pColorFrame->CopyConvertedFrameDataToArray(nBufferSize, reinterpret_cast<BYTE*>(pBuffer), ColorImageFormat_Bgra);            
            }
            else
            {
                hr = E_FAIL;
            }
        }

        if (SUCCEEDED(hr))
        {
            ProcessColor(nTime, pBuffer, nWidth, nHeight);
        }

        SafeRelease(pFrameDescription);
    }

}

void KinectThread::ProcessColor(INT64 nTime, RGBQUAD* pBuffer, int nWidth, int nHeight) 
{
    

    // Make sure we've received valid data
    if (pBuffer && (nWidth == cColorWidth) && (nHeight == cColorHeight))
    {
        // Draw the data with Direct2D
        //m_pDrawColor->Draw(reinterpret_cast<BYTE*>(pBuffer), cColorWidth * cColorHeight * sizeof(RGBQUAD));
		cv::Mat imgbuf(cv::Size(nWidth, nHeight), CV_8UC4, reinterpret_cast<BYTE*>(pBuffer));

		EnterCriticalSection(&criticalSection);
		cv::cvtColor(imgbuf, m_cvImageBufferRGB, CV_BGRA2BGR);
		LeaveCriticalSection(&criticalSection);
    }
}

void KinectThread::updateCaptureDepth(IDepthFrame* pDepthFrame)
{

	if (pDepthFrame)
    {
        INT64 nTime = 0;
        IFrameDescription* pFrameDescription = NULL;
        int nWidth = 0;
        int nHeight = 0;
        USHORT nDepthMinReliableDistance = 0;
        USHORT nDepthMaxReliableDistance = 0;
        UINT nBufferSize = 0;
        UINT16 *pBuffer = NULL;

        hr = pDepthFrame->get_RelativeTime(&nTime);

        if (SUCCEEDED(hr))
        {
            hr = pDepthFrame->get_FrameDescription(&pFrameDescription);
        }

        if (SUCCEEDED(hr))
        {
            hr = pFrameDescription->get_Width(&nWidth);
        }

        if (SUCCEEDED(hr))
        {
            hr = pFrameDescription->get_Height(&nHeight);
        }

        if (SUCCEEDED(hr))
        {
            hr = pDepthFrame->get_DepthMinReliableDistance(&nDepthMinReliableDistance);
        }

        if (SUCCEEDED(hr))
        {
            hr = pDepthFrame->get_DepthMaxReliableDistance(&nDepthMaxReliableDistance);
        }

        if (SUCCEEDED(hr))
        {
            hr = pDepthFrame->AccessUnderlyingBuffer(&nBufferSize, &pBuffer);            
        }

        if (SUCCEEDED(hr))
        {
            ProcessDepth(nTime, pBuffer, nWidth, nHeight, nDepthMinReliableDistance, nDepthMaxReliableDistance);
        }

        SafeRelease(pFrameDescription);
    }

}

void KinectThread::ProcessDepth(INT64 nTime, UINT16* pBuffer, int nWidth, int nHeight, USHORT nMinDepth, USHORT nMaxDepth)
{
    
    // Make sure we've received valid data
    if (m_pDepthRGBX && pBuffer && (nWidth == cDepthWidth) && (nHeight == cDepthHeight))
    {
        RGBQUAD* pRGBX = m_pDepthRGBX;

		const USHORT* pBufferStart = pBuffer;
        // end pixel is start + width*height - 1
        const UINT16* pBufferEnd = pBuffer + (nWidth * nHeight);

        while (pBuffer < pBufferEnd)
        {
            USHORT depth = *pBuffer;

            // To convert to a byte, we're discarding the most-significant
            // rather than least-significant bits.
            // We're preserving detail, although the intensity will "wrap."
            // Values outside the reliable depth range are mapped to 0 (black).

            // Note: Using conditionals in this loop could degrade performance.
            // Consider using a lookup table instead when writing production code.
            BYTE intensity = static_cast<BYTE>((depth >= nMinDepth) && (depth <= nMaxDepth) ? (depth % 256) : 0);

            pRGBX->rgbRed   = intensity;
            pRGBX->rgbGreen = intensity;
            pRGBX->rgbBlue  = intensity;

            ++pRGBX;
            ++pBuffer;
        }

		
		EnterCriticalSection(&criticalSection);
		// store to 16 bit opencv structure
		cv::Mat depthraw(cv::Size(nWidth, nHeight), CV_16U, (USHORT*)(pBufferStart));
		m_cvDepthBufferRaw = depthraw.clone();

		// convert to visible gray map
		cv::Mat depthbuf(cv::Size(nWidth, nHeight), CV_8UC4, reinterpret_cast<USHORT*>(m_pDepthRGBX));
		cv::cvtColor(depthbuf, m_cvDepthBuffer, CV_BGRA2RGB);
		LeaveCriticalSection(&criticalSection);

    }
}


void KinectThread::updateCaptureInfrared(IInfraredFrame* pInfraredFrame)
{

	// process IR
	if (pInfraredFrame)
	{
		INT64 nTime = 0;
		IFrameDescription* pFrameDescription = NULL;
		int nWidth = 0;
		int nHeight = 0;
		UINT nBufferSize = 0;
		UINT16 *pBuffer = NULL;

		hr = pInfraredFrame->get_RelativeTime(&nTime);

		if (SUCCEEDED(hr))
		{
			hr = pInfraredFrame->get_FrameDescription(&pFrameDescription);
		}

		if (SUCCEEDED(hr))
		{
			hr = pFrameDescription->get_Width(&nWidth);
		}

		if (SUCCEEDED(hr))
		{
			hr = pFrameDescription->get_Height(&nHeight);
		}

		if (SUCCEEDED(hr))
		{
			hr = pInfraredFrame->AccessUnderlyingBuffer(&nBufferSize, &pBuffer);            
		}

		if (SUCCEEDED(hr))
		{
			ProcessInfrared(nTime, pBuffer, nWidth, nHeight);
		}

		SafeRelease(pFrameDescription);
	}


}


/** 
* Processing of infrared data to become readable as it is 16 bit originally
*/
void KinectThread::ProcessInfrared(INT64 nTime, const UINT16* pBuffer, int nWidth, int nHeight)
{
	if (m_pInfraredRGBX && pBuffer && (nWidth == cInfraredWidth) && (nHeight == cInfraredHeight))
	{
		RGBQUAD* pDest = m_pInfraredRGBX;

		// end pixel is start + width*height - 1
		const UINT16* pBufferEnd = pBuffer + (nWidth * nHeight);

		while (pBuffer < pBufferEnd)
		{
			// normalize the incoming infrared data (ushort) to a float ranging from 
			// [InfraredOutputValueMinimum, InfraredOutputValueMaximum] by
			// 1. dividing the incoming value by the source maximum value
			float intensityRatio = static_cast<float>(*pBuffer) / InfraredSourceValueMaximum;

			// 2. dividing by the (average scene value * standard deviations)
			intensityRatio /= InfraredSceneValueAverage * InfraredSceneStandardDeviations;

			// 3. limiting the value to InfraredOutputValueMaximum
			intensityRatio = min(InfraredOutputValueMaximum, intensityRatio);

			// 4. limiting the lower value InfraredOutputValueMinimum
			intensityRatio = max(InfraredOutputValueMinimum, intensityRatio);

			// 5. converting the normalized value to a byte and using the result
			// as the RGB components required by the image
			byte intensity = static_cast<byte>(intensityRatio * 255.0f); 
			pDest->rgbRed = intensity;
			pDest->rgbGreen = intensity;
			pDest->rgbBlue = intensity;

			++pDest;
			++pBuffer;
		}

		cv::Mat imgbuf(cv::Size(nWidth, nHeight), CV_8UC4, reinterpret_cast<BYTE*>(m_pInfraredRGBX));

		EnterCriticalSection(&criticalSection);
		cv::cvtColor(imgbuf, m_cvInfraredBuffer, CV_BGRA2BGR);
		LeaveCriticalSection(&criticalSection);

	}
}


void KinectThread::updateCaptureBodyIndex(IBodyIndexFrame* pBodyIndexFrame)
{
	if (pBodyIndexFrame)
	{
		INT64 nTime = 0;
		IFrameDescription* pFrameDescription = NULL;
		int nBodyIndexWidth = 0;
		int nBodyIndexHeight = 0;
		UINT nBodyIndexBufferSize = 0;
		BYTE *pBodyIndexBuffer = NULL;

		hr = pBodyIndexFrame->get_RelativeTime(&nTime);

		if (SUCCEEDED(hr))
		{
			hr = pBodyIndexFrame->get_FrameDescription(&pFrameDescription);
		}

		if (SUCCEEDED(hr))
		{
			hr = pFrameDescription->get_Width(&nBodyIndexWidth);
		}

		if (SUCCEEDED(hr))
		{
			hr = pFrameDescription->get_Height(&nBodyIndexHeight);
		}

		if (SUCCEEDED(hr))
		{
			hr = pBodyIndexFrame->AccessUnderlyingBuffer(&nBodyIndexBufferSize, &pBodyIndexBuffer);   
			ProcessBodyIndex(nTime, pBodyIndexBuffer, nBodyIndexWidth, nBodyIndexHeight);
	
		}

		SafeRelease(pFrameDescription);
	}

}


void KinectThread::ProcessBodyIndex(INT64 nTime, BYTE* pBuffer, int nWidth, int nHeight)
{
	// Make sure we've received valid data
	if (m_pDepthRGBX && pBuffer && (nWidth == cDepthWidth) && (nHeight == cDepthHeight))
	{
		const BYTE* pBufferStart = pBuffer;

		EnterCriticalSection(&criticalSection);
		// store to 8 bit opencv structure
		cv::Mat indexraw(cv::Size(nWidth, nHeight), CV_8U, (BYTE*)(pBufferStart));
		m_cvBodyIndexBufferRaw = indexraw.clone();
		LeaveCriticalSection(&criticalSection);
	}

}




void KinectThread::updateSkeletons(IBodyFrame *pBodyFrame)
{
	if (pBodyFrame)
	{
		INT64 nTime = 0;
		hr = pBodyFrame->get_RelativeTime(&nTime);
		IBody* ppBodies[BODY_COUNT] = {0};

		if (SUCCEEDED(hr))
		{
			hr = pBodyFrame->GetAndRefreshBodyData(_countof(ppBodies), ppBodies);
		}

		if (SUCCEEDED(hr))
		{
			// get body plane
			Vector4 plane;
			pBodyFrame->get_FloorClipPlane(&plane);
			updateCameraToWorld(&m_matCameraToWorld, plane.w, plane.x, plane.y, plane.z);

			ProcessBody(nTime, BODY_COUNT, ppBodies);
		}

		for (int i = 0; i < _countof(ppBodies); ++i)
		{
			SafeRelease(ppBodies[i]);
		}

	}

}



void KinectThread::ProcessBody(INT64 nTime, int nBodyCount, IBody** ppBodies)
{
	if (!m_pSkeletonSet) return;	// TODO: we should throw some errors here
	if (m_pSkeletonSet->size() != BODY_COUNT) return; // TODO: we should throw some errors here

	// set skeletonSet header stuff
	m_pSkeletonSet->setTimeStamp(m_timestampSeconds, m_timestampMicroSeconds);
	m_pSkeletonSet->setFrameNumber(m_currentFrame);
	m_pSkeletonSet->setTimeStampDouble( double (clock ()) / double (CLOCKS_PER_SEC));
	m_pSkeletonSet->setCameraToWorldTransform(m_matCameraToWorld);

	for (int i = 0; i < nBodyCount; ++i)		// IDs must be assigned from 1..6 tracking specific skeleton/user
	{
		IBody* pBody = ppBodies[i];
		if (pBody)
		{
			// set skeleton header parameters
			Skeleton *pSkeleton = &m_pSkeletonSet->at(i);
			BOOLEAN bTracked = false;
			hr = pBody->get_IsTracked(&bTracked);

			pSkeleton->setTracked(static_cast<bool>(bTracked));
			UINT64 id;
			pBody->get_TrackingId(&id);
			pSkeleton->setId(static_cast<int>(id));

			//pSkeleton->setVersion(3.0f);

			if (SUCCEEDED(hr) && bTracked)
			{
				Joint joints[JointType_Count]; 

				HandState leftHandState = HandState_Unknown;
				HandState rightHandState = HandState_Unknown;

				pBody->get_HandLeftState(&leftHandState);
				pBody->get_HandRightState(&rightHandState);

				hr = pBody->GetJoints(_countof(joints), joints);
				if (SUCCEEDED(hr))
				{
				
					// we copy joint data one-by-one
					for (int j = 0; j < _countof(joints); ++j)
					{

						// joint position
						//pSkeleton->setJoint(static_cast<Skeleton::JointType>(j), 1000.0f*joints[j].Position.X, 1000.0f*joints[j].Position.Y, 1000.0f*joints[j].Position.Z);

						// transform all joints to new c.s.
						//pSkeleton->setJoint(static_cast<Skeleton::JointType>(j)
						Eigen::Vector4f jointPt = Eigen::Vector4f(1000.0f*joints[j].Position.X,1000.0f*joints[j].Position.Y, 1000.0f*joints[j].Position.Z, 1.0f);
						//jointPt.head(3) = pSkeleton->getJoint(static_cast<Skeleton::JointType>(j));
						Eigen::Vector4f jointPtTrans = m_matCameraToWorld * jointPt;
						pSkeleton->setJoint(static_cast<Skeleton::JointType>(j), jointPtTrans.head(3));
						
						// joint projection to color image. TOOD: add option of what to project to
						ColorSpacePoint pt = BodyToColor(joints[j].Position, cColorWidth, cColorHeight);
						pSkeleton->setJoint2D(static_cast<Skeleton::JointType>(j), pt.X, pt.Y);

						// quaternion. TODO: Does not seem to be implemented yet!
						/*pSkeleton->setJointQuaternion(static_cast<Skeleton::JointType>(j), joints[j].TrackingState
							)
*/
						// joint tracking status
						float status = 0.0f;
						if (joints[j].TrackingState == TrackingState_Tracked) status = 1.0f;
						else if (joints[j].TrackingState == TrackingState_Inferred) status = 0.5f;
						else status = 0.0f;
						pSkeleton->setJointScore(static_cast<Skeleton::JointType>(j), status);
					}
				}
				
			}
		}
	}


	// save skeletons
	if (m_bSaveSkeletons) {

		// check if file is open, if not try creating
		if (streamPoses == 0) {
			streamPoses = new std::ofstream(skeleton_save_path);
			if (streamPoses == 0) 
				throw Exception ("Cannot open skeleton file %s", std::string(skeleton_save_path).c_str());
		}
		// TODO: Add flags to pass from main. ROTATIONS don't seem to be implemented yet!
		m_pSkeletonSet->save (*streamPoses, (io:: SkeletonIOFlags)(/*io:: SkeletonIOFlags::POSE_IO_BINARY |*/io::SkeletonIOFlags::POSE_IO_TRANSFORM | io:: SkeletonIOFlags::POSE_IO_JOINT_DETECTION_SCORES /*| io:: SkeletonIOFlags::POSE_IO_ROTATIONS*/));
	}

	// buffer skeletons
	if (m_bBufferSkeletons)
	{
		SkeletonSet *p = m_pSkeletonSet;
		m_skeletonSequenceBuffer->push_back(*p);
	}

}



ColorSpacePoint KinectThread::BodyToColor(const CameraSpacePoint& bodyPoint, int width, int height)
{
	// Calculate the body's position on the screen
	//DepthSpacePoint depthPoint = {0};
	//m_pCoordinateMapper->MapCameraPointToDepthSpace(bodyPoint, &depthPoint);
	ColorSpacePoint depthPoint = {0};
	m_pCoordinateMapper->MapCameraPointToColorSpace(bodyPoint, &depthPoint);

	float screenPointX = static_cast<float>(depthPoint.X * width) / cColorWidth;
	float screenPointY = static_cast<float>(depthPoint.Y * height) / cColorHeight;

	ColorSpacePoint outPoint;
	outPoint.X = screenPointX;
	outPoint.Y = screenPointY;
	return outPoint;
}


void KinectThread::ProcessColorMapping()
{


	if (!m_pCoordinateMapper && !m_pDepthCoordinates)
	{
		EnterCriticalSection(&criticalSection);

		UINT16* pDepthBuffer = (UINT16*)m_cvDepthBufferRaw.data;

		// get mapping for color for the particular depth frame
		HRESULT hr = m_pCoordinateMapper->MapColorFrameToDepthSpace(cDepthWidth * cDepthHeight, (UINT16*)pDepthBuffer, cColorWidth * cColorHeight, m_pDepthCoordinates);

		cv::Mat rgbSmall(cDepthHeight, cDepthWidth, CV_8UC3);
		rgbSmall = cv::Scalar(0, 0, 0);


		if (SUCCEEDED(hr))
		{
			BYTE* pRGB = (BYTE*)m_cvImageBufferRGB.data;
			BYTE* pOut = (BYTE*)m_cvImageMappedBuffer.data;
			const BYTE* ptr = (BYTE*)m_cvImageMappedBuffer.data;

			// loop over output pixels
			for (int colorIndex = 0; colorIndex < (cColorWidth*cColorHeight); ++colorIndex)
			{
				// default setting source to copy from the background pixel

				//const RGBQUAD* pSrc = m_pBackgroundRGBX + colorIndex;

				// get pointer to color image
				BYTE black[3] = {0, 0, 0};
				const BYTE* pSrc = black; //pRGB + 3*colorIndex;	// 3-channel image?!

				DepthSpacePoint p = m_pDepthCoordinates[colorIndex];

				// Values that are negative infinity means it is an invalid color to depth mapping so we
				// skip processing for this pixel
				if (p.X != -std::numeric_limits<float>::infinity() && p.Y != -std::numeric_limits<float>::infinity())
				{
					int depthX = static_cast<int>(p.X + 0.5f);
					int depthY = static_cast<int>(p.Y + 0.5f);

					if ((depthX > 0 && depthX < cDepthWidth) && (depthY > 0 && depthY < cDepthHeight))
					{
						BYTE *pBodyIndexBuffer = (BYTE*)m_cvBodyIndexBufferRaw.data;
						BYTE player = pBodyIndexBuffer[depthX + (depthY * cDepthWidth)];

						// if we're tracking a player for the current pixel, draw from the color camera
						//if (player != 0xff)
						{
							// set source for copy to the color pixel
							pSrc = pRGB + 3*colorIndex;

							int colorSmallIndex = depthX + (depthY * cDepthWidth);
							//std::cout << colorSmallIndex << " " << std::endl;

							((BYTE*)rgbSmall.data)[3*colorSmallIndex]  = *pSrc;
							((BYTE*)rgbSmall.data)[3*colorSmallIndex + 1]  = *(pSrc + 1);
							((BYTE*)rgbSmall.data)[3*colorSmallIndex + 2]  = *(pSrc + 2);

						}


					}
				}

				// write output
				((BYTE*)m_cvImageMappedBuffer.data)[3*colorIndex]  = *pSrc;
				((BYTE*)m_cvImageMappedBuffer.data)[3*colorIndex + 1]  = *(pSrc + 1);
				((BYTE*)m_cvImageMappedBuffer.data)[3*colorIndex + 2]  = *(pSrc + 2);

			}
		}
		
		//	cv::namedWindow("hello");
		//	cv::imshow("hello", rgbSmall);
		//	cv::waitKey(1);

		LeaveCriticalSection(&criticalSection);

	} // end of if m_pCoordinateMapper
}




/**
 *	Get color buffer
 */
void KinectThread::getColor(cv::Mat &image) {
	EnterCriticalSection(&criticalSection);
	image = m_cvImageBufferRGB.clone();
	LeaveCriticalSection(&criticalSection);
}

/**
 *	Get color buffer mapped
 */
void KinectThread::getColorToDepth(cv::Mat &image) {
	EnterCriticalSection(&criticalSection);
	image = m_cvImageMappedBuffer.clone();
	LeaveCriticalSection(&criticalSection);
}

/**
 *	Get depth buffer
 */
void KinectThread::getDepth(cv::Mat &depths) /*grayscale*/ {
	EnterCriticalSection(&criticalSection);
	depths = m_cvDepthBuffer.clone();
	LeaveCriticalSection(&criticalSection);
}

/**
 *	Get raw depth buffer
 */
void KinectThread::getDepthRaw(cv::Mat &depths)  /*grayscale*/{
	EnterCriticalSection(&criticalSection);
	depths = m_cvDepthBufferRaw.clone();
	LeaveCriticalSection(&criticalSection);
}

/**
 *	Get infrared image
 */
void KinectThread::getInfrared(cv::Mat &ir) {
	EnterCriticalSection(&criticalSection);
	ir = m_cvInfraredBuffer.clone();
	LeaveCriticalSection(&criticalSection);
}

/**
 *	Get copy of the skeleton set
 */
void KinectThread::getSkeletons (SkeletonSet &skeletons)
{
	EnterCriticalSection(&criticalSection);
	SkeletonSet* p = m_pSkeletonSet;

	// copy header:
	skeletons.clear();
	skeletons.setHeader(p->getHeader());

	// pass only visible skeletons
	for (size_t i = 0; i < m_pSkeletonSet->size(); i++)
		if (m_pSkeletonSet->at(i).isTracked()) skeletons.push_back(m_pSkeletonSet->at(i));
	// copy all
	//SkeletonSet* p = m_pSkeletonSet;
	//skeletons = *p;
	LeaveCriticalSection(&criticalSection);
}

/**
* Start/stop skeleton saving
*/
void KinectThread::saveSkeletons(std::string path, bool on, bool bIsFolder) 
{
	if (bIsFolder)
		skeleton_save_path = path + "\\poses.txt";
	else
		skeleton_save_path = path;
	m_bSaveSkeletons = on;
}

/**
* Start image saving
*/
void KinectThread::startSavingImages(std::string folder)
{
	m_bSaveImageFrames = true;
	m_imageSavePath = folder;
}

/**
* Stop image saving
*/
void KinectThread::stopSavingImages()
{
	m_bSaveImageFrames = false;
}


/**
 *	Updates camera to world transform from input plane parameters
 */
void KinectThread::updateCameraToWorld(Eigen::Matrix4f *matrix, float w, float x, float y, float z)
{
	 // ground plane normal will be the z-axis
	Eigen::Vector3f z_axis = Eigen::Vector3f(x, y, z);
	Eigen::Vector3f x_axis = Eigen::Vector3f(1.0f, 0.0f, 0.0f);
	Eigen::Vector3f y_axis = z_axis.cross(x_axis);

	matrix->row(0).head(3) = x_axis;
	matrix->row(1).head(3) = y_axis;
	matrix->row(2).head(3) = z_axis;

	(*matrix)(2, 3) = w * 1000.0f;
}