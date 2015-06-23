#include "../libKinectInterface2/libKinectInterface2.h"

#include "../libSkeleton/libSkeleton.h"

#include <iostream>
#include <iomanip>
#include <sstream>


int main()
{

	// test out skeleton io stuff
	Skeleton skeleton;
	std::string frame;
	
	SkeletonSet skeletonSet;
#if 0
	skeletonSet.push_back(skeleton);
	skeletonSet.push_back(skeleton);

	//json skeleton frame
	CJsonSerializer::Serialize(&skeleton, frame);
	std::cout << "Skeleton: " << std::endl;
	std::cout << frame << std::endl;

	//json skeleton set
	CJsonSerializer::Serialize(&skeletonSet, frame);
	std::cout << "SkeletonSet: " << std::endl;
	std::cout << frame << std::endl;
	std::cout << "Press <Enter> to continue." << std::endl;
	std::getline(std::cin, std::string());

	// Kinect save/load to text
	SkeletonSetSequence seq;
	for (size_t i = 0; i < 5; i++) {
		seq.push_back(skeletonSet);
		skeletonSet.setFrameNumber(i);
	}
	std::cout << "Write IO:" << std::endl;
	int status = seq.save("output.txt", io::POSE_IO_NOFLAGS);
	std::cout << "Done." << std::endl;
	status == 0 ? std::cout << "ok :)" << std::endl : std::cout << ":(" << std::endl;
	std::cout << "Press <Enter> to continue." << std::endl;
	std::getline(std::cin, std::string());

#endif

	// test Kinect
	Kinect4WLive kl;
	kl.init();


	cv::namedWindow("Color", cv::WINDOW_NORMAL);
	cv::namedWindow("Depth", cv::WINDOW_NORMAL);
	cv::namedWindow("IR", cv::WINDOW_NORMAL);
	cv::namedWindow("ColorMap", cv::WINDOW_NORMAL);
	cv::resizeWindow("Color", 570, 320);
	cv::resizeWindow("ColorMap", 570, 320);

	kl.startSkeletonSaving("output_live.txt", false);


	bool bSavePressed = false;
	CStopWatch timer;
	std::cout << "Press <Space> to save a frame." << std::endl;

	// start the loop
	while (GetAsyncKeyState(VK_ESCAPE)>=0)
	{
		cv::Mat image;
		kl.getColorImage(image);
		cv::imshow("Color", image);

		cv::Mat imageMap;
		kl.getColorToDepthImage(imageMap);
		cv::imshow("ColorMap", imageMap);

		cv::Mat depth;
		kl.getDepthImage(depth);
		cv::imshow("Depth", depth);

		cv::Mat ir;
		kl.getInfrared(ir);
		cv::imshow("IR", ir);


		// check for keystrokes to grab image
		if (!bSavePressed && GetAsyncKeyState(VK_SPACE) < 0)
		{
			bSavePressed = true;
			timer.startTimer();
			std::stringstream ss;
			long currentFrame = kl.getFrameNumber();
			ss << std::setw(5) << std::setfill('0') << currentFrame;
			std::string frameString = ss.str();

			cv::imwrite(std::string("rgb" + frameString + ".jpg"), image);
			cv::imwrite(std::string("ir" + frameString + ".png"), ir);
			
			cv::Mat depth16;
			kl.getDepthImageRaw(depth16);
			cv::imwrite(std::string("depth" + frameString + ".png"), depth16);
			
			ss << std::setw(5) << std::setfill('0') << currentFrame;
			std::cout << "Saving frame #" << currentFrame << "." << std::endl;
		}

		timer.stopTimer();
		if (bSavePressed && timer.getElapsedTime() > 0.5)
		{
			bSavePressed = false;
		}

		cv::waitKey(1);



		//json skeleton set
/*		kl.getSkeletons(skeletonSet);
		CJsonSerializer::Serialize(&skeletonSet, frame);
		std::cout << "SkeletonSet: " << std::endl;
		std::cout << frame << std::endl;
*/
		::Sleep(30);


	}

	kl.stopSkeletonSaving();

	cv::destroyAllWindows();

	kl.shutDown();

	//std::cout<<"i am in" << std::endl;
	//KinectThread k (0,true,false,false,false);
	////KinectThread::captureThreadFunc(nullptr);
	return 0;
}
