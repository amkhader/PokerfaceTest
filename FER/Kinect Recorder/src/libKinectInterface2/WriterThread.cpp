#include "libKinectInterface2.h"
#include "WriterThread.h"

#include <iomanip>

CRITICAL_SECTION g_CriticalSectionBuff;

ThreadServer::ThreadServer(HANDLE handle, ImageFrame *pFrame, ThreadConfig *cfg) :
	hNewFrameEvent(handle),
	image(pFrame)

{

	// set up flags
	bStop = cfg->bStop;
	bSave = cfg->bSave;


	// folder for saving images (check if it exists).
	std::string *save_path = cfg->imagePath;
	pSavePath = cfg->imagePath;

	FrameBuffer.clear();

	hStopLoop = CreateEvent(NULL,TRUE,FALSE,TEXT("StopLoop"));
	if (hStopLoop == NULL)
	{
		printf("CreateEvent failed (%d)\n",GetLastError());
		return;
	}
}

ThreadServer::~ThreadServer()
{
}



void ThreadServer::StartUpBuff()
{
	static volatile bool csInitialisedBuff = false;
	if (!csInitialisedBuff)
	{
		InitializeCriticalSection(&g_CriticalSectionBuff);
		csInitialisedBuff = true;
	}

	bool lbStop = false;

	unsigned stId;
	HANDLE sh = (HANDLE) _beginthreadex( NULL, 0, ThreadServerStaticStartupSave, this, CREATE_SUSPENDED, &stId);
	ResumeThread(sh);

	ImageFrame *dummy;
	CStopWatch timer;
	timer.startTimer();

	while (!lbStop)
	{
		std::cout << "Before" <<std::endl;
		// wait for handle event
		DWORD dwTimeout = WaitForSingleObject(hNewFrameEvent, INFINITE);
		std::cout << "after" <<std::endl;
		timer.stopTimer();
		//std::cout << FrameBuffer.size()  <<  " (" << timer.getElapsedTime() << ")" << std::endl;
		timer.startTimer();

		if (!(*bStop))
		{
			if (*bSave)
			{
				// check if image is valid, otherwise skip it
				//.FO. if (image->colour.size() > 0 && image->depth.size() > 0)
				if (!image->colour.empty() && !image->depth.empty())
				{
					dummy = new ImageFrame(image);

					EnterCriticalSection( &g_CriticalSectionBuff );
					FrameBuffer.push_back(dummy);
					std::cout << "Frame Buffer Size: " << FrameBuffer.size() << std::endl;
					LeaveCriticalSection( &g_CriticalSectionBuff );
				}

			}
			else
			{
				::Sleep(1);
			}
		}
		else
		{
			std::cout << "lbStop is set to true!" << std::endl;
			lbStop = true;
		}

	}

	std::cout << "Setting event hStopLoop" << std::endl;
	SetEvent(hStopLoop);
	std::cout << "... set event hStopLoop" << std::endl;

	WaitForSingleObject(sh,INFINITE);
}

CRITICAL_SECTION g_CriticalSectionSave;

void ThreadServer::StartUpSave()
{
	static volatile bool csInitialisedSave = false;
	if (!csInitialisedSave)
	{
		InitializeCriticalSection(&g_CriticalSectionSave);
		csInitialisedSave = true;
	}

	//char logfile[128];
	//sprintf(logfile,"log_framelist_%d.txt",ID);
	//FILE *frames = fopen(logfile,"w");

	//IplImage *dummy_save = cvCreateImage(cvSize(imgCols,imgRows),imgDepth,imgChn);
	ImageFrame *dummy;

	int count_frame = 0;
	bool bExitFlag = false;

	bool bCheckFolder = true;

	while(WaitForSingleObject(hStopLoop,0) == WAIT_TIMEOUT)
	{
		// create folder if it does not exist		
		if (bCheckFolder) {
			if (CreateDirectoryA(pSavePath->c_str(),NULL) == 0)
			{
				if(ERROR_ALREADY_EXISTS ==GetLastError())
					printf("Folder '%s' already exists.\n", pSavePath->c_str());
			}
		bCheckFolder = false;
		}

		if (!FrameBuffer.empty())
		{
			EnterCriticalSection( &g_CriticalSectionBuff );
			//fprintf(frames,"%d\n",FrameBuffer.size());
			std::cout << "# Frames in Buffer: " << FrameBuffer.size() << std::endl;
			dummy = FrameBuffer.front();
			//dummy_save->imageData = dummy->iData;
			FrameBuffer.pop_front();
			LeaveCriticalSection( &g_CriticalSectionBuff );

			std::stringstream ss;
			ss << std::setw(5) << std::setfill('0') << dummy->index;
			img_name_color = std::string(pSavePath->c_str()) + "color" + ss.str() + ".jpg"; //.FO. sprintf_s(img_name_color,"%s\\color%05u.jpg", pSavePath->c_str(), dummy->index);
			img_name_depth = std::string(pSavePath->c_str()) + "depth" + ss.str() + ".png"; //.FO. sprintf_s(img_name_depth,"%s\\depth%05u.pgm", pSavePath->c_str(), dummy->index);

			EnterCriticalSection( &g_CriticalSectionSave );
			//std::cout << "Saving images: " << img_name_color <<  " , " << img_name_depth << std::endl;

			cv::imwrite(img_name_color, dummy->colour); //.FO. writeJPG(img_name_color, dummy->colour, 95);
			cv::imwrite(img_name_depth, dummy->depth);  // writePGM16(img_name_depth, dummy->depth);
			//::Sleep(50);
			
			::Sleep(0);
			LeaveCriticalSection( &g_CriticalSectionSave );

			count_frame++;

			//FrameList.pop_front(); // this causes trouble here! =.FO.=
			dummy->~ImageFrame();
			//delete [] dummy;
			dummy = NULL;
		}
		else
		{
			::Sleep(1);
		}
	}

	if (bSave) 
	{
		if (!FrameBuffer.empty())
			std::cout << "Image Writer Thread: Saving remaining images..." << std::endl;
	}
	while(!FrameBuffer.empty())
	{
		EnterCriticalSection( &g_CriticalSectionBuff );
		//fprintf(frames,"%d\n",FrameBuffer.size());
		dummy = FrameBuffer.front();
		//dummy_save->imageData = dummy->iData;
		FrameBuffer.pop_front();
		LeaveCriticalSection( &g_CriticalSectionBuff );

		std::stringstream ss;
		ss << std::setw(5) << std::setfill('0') << dummy->index;
		img_name_color = std::string(pSavePath->c_str()) + "color" + ss.str() + ".jpg"; //.FO. sprintf_s(img_name_color,"%s\\color%05u.jpg", pSavePath->c_str(), dummy->index);
		img_name_depth = std::string(pSavePath->c_str()) + "depth" + ss.str() + ".png"; //.FO. sprintf_s(img_name_depth,"%s\\depth%05u.pgm", pSavePath->c_str(), dummy->index);

		EnterCriticalSection( &g_CriticalSectionSave );
		//std::cout << "Saving images: " << img_name_color <<  " , " << img_name_depth << std::endl;
		cv::imwrite(img_name_color, dummy->colour); //.FO. writeJPG(img_name_color, dummy->colour, 95);
		cv::imwrite(img_name_depth, dummy->depth);  // writePGM16(img_name_depth, dummy->depth);
		//::Sleep(50);

		// output some dots for saved images
		printf("Thread: %u/%u\r", dummy->index, FrameBuffer.size());
		//std::cout << "Emptying buffer " << ID << " image saving index #" << dummy->index  << "(frame " << count_frame << ") / " << FrameBuffer.size() << std::endl;

		::Sleep(1);
		LeaveCriticalSection( &g_CriticalSectionSave );

		count_frame++;

		//FrameList.pop_front(); // this causes trouble here! =.FO.=
		//delete [] dummy;
		dummy->~ImageFrame();
		dummy = NULL;
	}

	//fclose(frames);

}