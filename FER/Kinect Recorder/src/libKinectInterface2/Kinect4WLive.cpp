#include "libKinectInterface2.h"


// Kinect mutex...	 Stepan used it for multiple Kinect synchronization. MS does not support multiple cameras with Kinect 2 and probably never will.
HANDLE kinectMutex;
int Kinect4WLive::m_kinectCount = 0;



/** 
* Constructor, creates thread
*/
Kinect4WLive::Kinect4WLive():
	m_kinectThread(nullptr)
{



}

/** 
* Destructor
*/
Kinect4WLive::~Kinect4WLive()
{
	if (m_kinectThread)
	{
		m_kinectThread->stop();
		WaitForSingleObject(m_kinectThread->getStopEvent(), 5000);
		delete m_kinectThread;
		m_kinectThread = nullptr;
	}
}

/**
* Camera initialization
*/
bool Kinect4WLive::init (int index)
{

	kinectMutex = CreateMutex (NULL, true, "KinectMutex");

	int count = 0;

	// Create Kinect capture thread
	//index, capture_color = true, bool capture_depth = true, bool capture_skeleton = false, bool capture_audio = false); 
	// TODO: Make these flags optional
	if (m_kinectThread != nullptr) 
	{
		delete m_kinectThread;
		m_kinectThread = nullptr;
	}

	if (!m_kinectThread)
		m_kinectThread = new KinectThread(index, true, true, true, true);
	

	ReleaseMutex (kinectMutex);

	return true;

}

/** 
* Shutdown camera
*/
void Kinect4WLive::shutDown(void)
{
	if (m_kinectThread) 
		m_kinectThread->stop();

}


/**
 *	Get current frame number
 */
long Kinect4WLive::getFrameNumber()
{
	if (m_kinectThread) return m_kinectThread->getFrameNumber();
	else return 0;
}


/**
 *	Get PFS count
 */
double Kinect4WLive::getFPS()
{
	if (m_kinectThread) return m_kinectThread->getFPS();
	else return 0;
}


/** 
* Get color image in HD
* TODO: add options to request specific size....
*/
void Kinect4WLive::getColorImage(cv::Mat &image)
{
	if (m_kinectThread)
	{
		m_kinectThread->getColor(image);
	}
}

/** 
* Get depth image (as RGB)
*/
void Kinect4WLive::getDepthImage(cv::Mat &image)
{
	if (m_kinectThread)
	{
		m_kinectThread->getDepth(image);
	}
}


/**
 *	Get depth image
 */
void Kinect4WLive::getDepthImageRaw(cv::Mat &image)
{
	if (m_kinectThread)
	{
		m_kinectThread->getDepthRaw(image);
	}
}

/** 
* Get color image with background subtracted based on depth and segmentation
*/
void Kinect4WLive::getColorToDepthImage(cv::Mat &image)
{
	if (m_kinectThread)
	{
		m_kinectThread->getColorToDepth(image);
	}
}

/** 
* Get infrared image buffer
*/
void Kinect4WLive::getInfrared(cv::Mat &image)
{
	if (m_kinectThread)
	{
		m_kinectThread->getInfrared(image);
	}
}

/**
 *	Get current skeleton (set) frame 
 */
void Kinect4WLive::getSkeletons(SkeletonSet &skeletons)
{
	if (m_kinectThread)
	{
		m_kinectThread->getSkeletons(skeletons);
	}
}

/**
 *	Start saving skeletons. Input file location or folder
 */
void Kinect4WLive::startSkeletonSaving(std::string fileLocation, bool bIsFolder)
{
	if (m_kinectThread)
	{
		m_kinectThread->saveSkeletons(fileLocation, true, bIsFolder);
	}
}

/**
 *	Stop skeleton saving
 */
void Kinect4WLive::stopSkeletonSaving()
{
	if (m_kinectThread)
	{
		m_kinectThread->saveSkeletons("", false);
	}
}

/**
 *	Start buffering skeleton to skeletonSequence vector
 */
void Kinect4WLive::startSkeletonBuffering()
{
	if (m_kinectThread)
	{

	}
}

/**
 *	Stop skeleton buffering
 */
void Kinect4WLive::stopSkeletonBuffering()
{
	if (m_kinectThread)
	{

	}
}

/**
 *	Get skeleton sequence buffer
 */
void Kinect4WLive::getSkeletonSeqenceBuffer(SkeletonSetSequence &skeletonSequence)
{
	if (m_kinectThread)
	{

	}
}

/**
 *	Start saving images in the thread
 */
void Kinect4WLive::startSavingImages(std::string folder)
{
	if (m_kinectThread)
	{
		m_kinectThread->startSavingImages(folder);
	}
}

/**
 *	Stop saving images
 */
void Kinect4WLive::stopSavingImages()
{
	if (m_kinectThread)
	{
		m_kinectThread->stopSavingImages();
	}
}


/** 
* Return handle for new frame
*/
HANDLE Kinect4WLive::getNewFrameEvent()
{
	if (m_kinectThread)
		return m_kinectThread->getNewFrameEvent();
	else
		return nullptr;
}