#pragma once
/********************************************************************
	created:	2014/10/02
	filename: 	Kinect4WLive.h
	file path:	libKinectInterface2
	file base:	Kinect4WLive
	file ext:	h
	author:		Gregorij Kurillo
	
	purpose:	
*********************************************************************/


// Forward declarations
class KinectThread;
class SkeletonSetSequence;

/*
 *	Main class for Kinect v 2.0
 */
class Kinect4WLive
{

public:
	Kinect4WLive();
	~Kinect4WLive();

	/// Initialize Kinect camera
	bool init (int index = 0);

	/// Shutdown Kinect camera
	void shutDown(void);

	/// Get current frame number
	long getFrameNumber();

	/// Get FPS
	double getFPS();

	/// Get color Image as RGB
	void getColorImage(cv::Mat &image);

	/// Get depth image as RGB
	void getDepthImage(cv::Mat &image);

	/// Get raw 16-bit depth image. TODO: Add option to mask it?
	void getDepthImageRaw(cv::Mat &image);

	/// Get mapping of color to depth. TODO: Does not seem to work properly
	void getColorToDepthImage(cv::Mat &image);

	/// Get infra red image
	void getInfrared(cv::Mat &image);

	/// Get current skeleton (set) frame 
	void getSkeletons(SkeletonSet &skeletons);

	/// Start saving skeletons. Input file location or folder
	void startSkeletonSaving(std::string fileLocation, bool bIsFolder = true);

	/// Stop skeleton saving
	void stopSkeletonSaving();

	/// Start buffering skeleton to skeletonSequence vector
	void startSkeletonBuffering();
	
	/// Stop skeleton buffering
	void stopSkeletonBuffering();

	/// Get skeleton sequence buffer
	void getSkeletonSeqenceBuffer(SkeletonSetSequence &skeletonSequence);

	/// Start saving images. Input folder.
	void startSavingImages(std::string folder);

	/// Stop saving images.
	void stopSavingImages(); 

	/// Check for new frame event. TODO: Does not work.
	HANDLE getNewFrameEvent();


private:

	/// Copy constructor - disabled.
	Kinect4WLive(const Kinect4WLive& that);

	// variables
	static int m_kinectCount;			///< number of Kinects (only one supported in v 2.0)
	KinectThread *m_kinectThread;		///< Pointer to Kinect thread instance


};