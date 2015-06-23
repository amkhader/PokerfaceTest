#ifndef __THREADSERVER__
#define	__THREADSERVER__

#include <list>
#include <process.h>

extern CRITICAL_SECTION g_CriticalSectionBuff;

/**
* Thread configuration class for passing parameters to threads
*/
struct ThreadConfig {

	volatile bool *bStop;		///< Pointer to flag for stopping the thread
	volatile bool *bSave;		///< Pointer to flag for starting to save images
	volatile bool *bUpdateImageData;

	std::string *imagePath;		///< Pointer to path for the image saving folder
};

/**
* Image data object
*/
class ImageFrame
{
public:
	unsigned int index;
	unsigned long sec;
	unsigned long msec;
	cv::Mat colour; //.FO. ColourImage colour;
	cv::Mat depth;  //.FO. GrayImage16 depth;

public:
	ImageFrame()
	{
		index = 0;
		sec = 0;
		msec = 0;
	}

	ImageFrame(ImageFrame *imfr)
	{
		index = imfr->index;
		sec = imfr->sec;
		msec = imfr->msec;
		colour = imfr->colour;
		depth = imfr->depth;
	}

	~ImageFrame()
	{

	}

	/// update data
	void UpdateFrameInfo(unsigned int index_, unsigned long sec_, unsigned long msec_) {
		index = index_;
		sec = sec_;
		msec = msec_;
	}

	//.FO. void UpdateFrameData(const ColourImage &color, const GrayImage16 &depth)
	void UpdateFrameData(const cv::Mat &color, const cv::Mat &depth)
	{
		this->colour = color;
		this->depth = depth;
	}

};

class ThreadServer
{
public:

private:
	int ID;
	int TID;
	int CID;
	int port;
	ImageFrame *image;
	//GlobalConfig *cfg;
	volatile bool *bStop;
	volatile bool *bSave;
	volatile bool *bExit;
	volatile bool *bUpdateImageData;		///< Flag to signal that image size/type has changed and buffers must be updated

	HANDLE hNewFrameEvent;
	cv::Mat *pColourImage; //.FO. ColourImage *pColourImage;
	cv::Mat *pGrayImage;   //.FO. GrayImage16 *pGrayImage;


	std::string *pSavePath;
	std::string img_name_color; //.FO. char img_name_color[1024];
	std::string img_name_depth; //.FO. char img_name_depth[1024];

	std::list<ImageFrame*>FrameBuffer;

	HANDLE hStopLoop;

public:
	// Constructor, will save images for color and depth map
	ThreadServer(HANDLE hNewFrameEvent, ImageFrame *pFrame, ThreadConfig *cfg);
	~ThreadServer();

	void StartUpBuff();
	static unsigned __stdcall ThreadServerStaticStartUp(void * pThis)
	{
		ThreadServer *pthF = (ThreadServer*)pThis;
		pthF->StartUpBuff();

		return 1; // the thread exit code
	}

private:
	void StartUpSave();
	static unsigned __stdcall ThreadServerStaticStartupSave(void * pThis)
	{
		ThreadServer* pthF = (ThreadServer*)pThis;
		pthF->StartUpSave();

		return 1;
	}

};


/**
*	Main object for saving
*/
struct ImageWriter {
	//.FO. ImageWriter(ColourImage *pColourImage, GrayImage16 *pGrayImage,ThreadConfig *cfg) :
	ImageWriter(cv::Mat *pColourImage, cv::Mat *pGrayImage,ThreadConfig *cfg) :
		pFrame(nullptr)
	{

		/// Create event to notify saving thread that a new frame has arrived
		hNewFrameEvent = CreateEvent(NULL, FALSE, FALSE, TEXT("NewImageFrameHere")); // Note: Only one thread can use this, afterwards it will get reset!!!

		pFrame = new ImageFrame();

		// create ThreadServer object that will start appropriate threads
		pThread = new ThreadServer(hNewFrameEvent, pFrame, cfg);
		handle = (HANDLE) _beginthreadex( NULL, 0, ThreadServer::ThreadServerStaticStartUp, pThread, CREATE_SUSPENDED, &tID);
	}

	~ImageWriter() {
		WaitForSingleObject(handle,INFINITE);
		CloseHandle(handle);
		CloseHandle(hNewFrameEvent);

		if (pThread) 
		{
			delete pThread;
			pThread = nullptr;
		}

		if (pFrame)
		{
			delete pFrame;
			pFrame = nullptr;
		}
	}

	//.FO. void addFrame(unsigned int frameNumber, ColourImage *color, GrayImage16 *depth)
	void addFrame(unsigned int frameNumber, cv::Mat *color, cv::Mat *depth)
	{
		EnterCriticalSection(&g_CriticalSectionBuff);
		pFrame->UpdateFrameInfo(frameNumber, 0, 0);
		pFrame->UpdateFrameData(*color, *depth);
		LeaveCriticalSection(&g_CriticalSectionBuff);

		SetEvent(hNewFrameEvent);
	}

	HANDLE getThreadHandle() { return handle;}


private:
	ThreadServer *pThread;
	unsigned int tID;
	HANDLE handle;
	HANDLE hNewFrameEvent;
	ImageFrame *pFrame;
};


#endif
