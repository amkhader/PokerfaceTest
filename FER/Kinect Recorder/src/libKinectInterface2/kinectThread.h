#pragma once



// OpenCV Headers
#include "opencv/cv.h"
#include "opencv/highgui.h"
#include <opencv2/opencv.hpp>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>

#include <Eigen/Eigen>

#include "Kinect.h"

#include "WriterThread.h"

// forward declarations
class Kinect4WLive;
class SkeletonSet;
class SkeletonSetSequence;
class ImageWriter;
class ThreadConfig;

/*
* Kinect thread class
*/
class KinectThread {

public:

	EIGEN_MAKE_ALIGNED_OPERATOR_NEW;	/// must add this for the alignment issue of EIGEN 

	KinectThread(int index = 0, bool capture_color = true, bool capture_depth = true, bool capture_skeleton = false, bool capture_audio = false); 
	~KinectThread();

	/// startup function
	void initialize();

	/// stop capture
	void stop();

	/// capture callback, obtains color & depth & skeleton depending on flags
	void capture();

	/// get current frame number
	long getFrameNumber() { return m_currentFrame; }

	// get fps
	double getFPS() { return m_fps; }

	/// get color buffer
	void getColor(cv::Mat &image);

	/// get color buffer mapped
	void getColorToDepth(cv::Mat &image);

	/// get depth buffer
	void getDepth(cv::Mat &depths);
	
	/// get raw depth buffer
	void getDepthRaw(cv::Mat &depths);

	/// get infrared image
	void getInfrared(cv::Mat &ir);

	/// Get copy of the skeleton set
	void getSkeletons(SkeletonSet &skeletons);

	/// get stop event
	HANDLE getStopEvent() { return hStopEvent;}

	// new frame event. TODO: does not work yet
	HANDLE getNewFrameEvent() { return hNewFrameEvent; }

	void saveSkeletons(std::string path, bool on, bool bIsFolder = true);	//TODO: shoud be bool

	void startSavingImages(std::string folder);

	void stopSavingImages();

protected:
	HRESULT initializekinect();
	//static DWORD WINAPI captureThreadFunc(LPVOID lpParam);
	static unsigned __stdcall KinectThread::captureThreadFunc(void * pThis);



	friend class Kinect4WLive;
	//Tracking :: CameraParameters m_kinectCP;			///< Camera parameters for 3D

private:

	/// Copy constructor - disabled
	KinectThread(const KinectThread &that);
	

	/// Map color image to depth image
	void mapImageToDepths();

	cv::Mat mapImageToDepths8 (const cv::Mat &image, const cv::Mat &depths);
	cv::Mat mapImageToDepths (const cv::Mat &image, const cv::Mat &depths);

	HRESULT hr;

	int cam_index;		///< Kinect camera index

	// flags
	volatile bool bColorMapping;
	volatile bool bTrackPlayer;
	volatile bool bCaptureSkeleton;
	volatile bool bCaptureDepth;
	volatile bool bCaptureColor;
	volatile bool bCaptureInfrared;
	volatile bool bCaptureAudio;
	volatile bool bThreadExit;

	// use high resolution (depth: 640x480, color: 640x480)
	bool bUseHighRes;

	// thread handles
	HANDLE hCaptureThread;
	HANDLE hCaptureMutex;
	HANDLE hStartEvent;
	HANDLE hStopEvent;

	HANDLE hKinectReadyEvent;

	// Kinect handles
	HANDLE skeletonEvent;
	HANDLE depthEvent;
	HANDLE colourEvent;
	HANDLE depthStreamHandle;
	HANDLE colourStreamHandle;

	HANDLE hNewFrameEvent;

	CRITICAL_SECTION criticalSection;

	// buffers
	cv::Mat m_cvDepthBuffer;
	cv::Mat m_cvDepthBufferRaw;
	cv::Mat m_cvImageBufferRGB;
	cv::Mat m_cvImageMappedBuffer;
	cv::Mat m_cvInfraredBuffer;
	cv::Mat m_cvBodyIndexBufferRaw;

	// Skeleton representation
	SkeletonSet *m_pSkeletonSet;
	SkeletonSetSequence *m_skeletonSequenceBuffer;

	// camera to world transform
	Eigen::Matrix4f m_matCameraToWorld;


	// Frame data
	LONG m_timestampSeconds;
	LONG m_timestampMicroSeconds;
	volatile LONG m_currentFrame;

	// for FPS
	double m_oldTimeDouble;
	int m_collectedFrames;
	volatile double m_fps;

	// Kinect geometry
	//Vector3 sumGroundPlaneNormal;
	float sumGroundPlaneOffset;
	int nGroundPlaneVotes;

	// Skeleton stuff
	int nPoses;			///< pose counter
	bool updateC2WTransform;

	
	// flags
	volatile bool m_bSaveSkeletons;
	volatile bool m_bBufferSkeletons;

	
	// Skeleton saving parameters
	std::string skeleton_save_path;
	volatile bool bSaveSkeleton;
	std::ofstream *streamPoses;
	std::ofstream *streamPoseTimes;
	int playerIndex;

	//.FO. image saving
	ImageWriter *pImageWriter;
	ThreadConfig threadConfig;
	unsigned int frameCounter;
	volatile bool m_bSaveImageFrames;
	volatile bool m_bStopSaveThread;
	std::string m_imageSavePath;


	//addition to match Kinect new API
	 // Current Kinect
    IKinectSensor*          m_pKinectSensor;
	ICoordinateMapper*      m_pCoordinateMapper;
	IMultiSourceFrameReader* m_pMultiSourceFrameReader;
	IBodyFrameReader*	m_pBodyFrameReader;

	RGBQUAD*                m_pColorRGBX;
    RGBQUAD*                m_pDepthRGBX;
	RGBQUAD*				m_pInfraredRGBX;
	DepthSpacePoint*		m_pDepthCoordinates;

	WAITABLE_HANDLE hFrameEvent;

	// Kinect data processing
	void updateCaptureColor(IColorFrame* pColorFrame);
	void updateCaptureDepth(IDepthFrame* pDepthFrame);
	void updateCaptureInfrared(IInfraredFrame* pInfraredFrame);
	void updateCaptureBodyIndex(IBodyIndexFrame* pBodyIndexFrame);
	void updateSkeletons(IBodyFrame *pBodyFrame);

	void ProcessColor(INT64 nTime, RGBQUAD* pBuffer, int nWidth, int nHeight);
	void ProcessDepth(INT64 nTime, UINT16* pBuffer, int nWidth, int nHeight, USHORT nMinDepth, USHORT nMaxDepth);
	void ProcessInfrared(INT64 nTime, const UINT16* pBuffer, int nWidth, int nHeight);
	void ProcessBodyIndex(INT64 nTime, BYTE* pBuffer, int nWidth, int nHeight);
	void ProcessColorMapping();
	void ProcessBody(INT64 nTime, int nBodyCount, IBody** ppBodies);
	ColorSpacePoint BodyToColor(const CameraSpacePoint& bodyPoint, int width, int height);

	void updateCameraToWorld(Eigen::Matrix4f *matrix, float w, float x, float y, float z);


	// Various constants related to the Kinect camera
	static const int        cColorWidth  = 1920;
    static const int        cColorHeight = 1080;
	static const int        cDepthWidth  = 512;
    static const int        cDepthHeight = 424;
	static const int		cInfraredWidth = 512;
	static const int		cInfraredHeight = 424;

	// parameters related to the infra red image (see example for the meaning)
	static const float InfraredSourceValueMaximum;
	static const float InfraredOutputValueMinimum;
	static const float InfraredOutputValueMaximum;
	static const float InfraredSceneValueAverage;
	static const float InfraredSceneStandardDeviations;


};

