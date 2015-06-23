#include "../libSkeleton/libSkeleton.h"
#include "../libKinectInterface2/libKinectInterface2.h"

#include "../Common/configio.h"

#include <iostream>
#include <direct.h>
#include <limits>

// Input parameters from command line
struct Params : ConfigIO
{
	Params :: Params (void);
	std::string outputFile;
};

Params :: Params (void)
{
	ADD_PARAMETER_VAL (outputFile, "output_live.txt");
}
Params params;


cv::Scalar bodyColors[6] = {cv::Scalar(255, 255, 0),
							cv::Scalar(255, 0, 255),
							cv::Scalar(0, 255, 255),
							cv::Scalar(100, 255, 0),
							cv::Scalar(0, 255, 100),
							cv::Scalar(100, 255, 100)};


/// Drawing functions for skeleton
void bodySegment(cv::Mat img, const Eigen::Vector2f &start, const Eigen::Vector2f &end, const cv::Scalar &color = cv::Scalar( 255, 255, 0 ))
{
	// check if any is zero, zero, skip it if it is
	if (start.x() == -std::numeric_limits<float>::infinity() || start.y() == -std::numeric_limits<float>::infinity() || 
		end.x() == -std::numeric_limits<float>::infinity() || end.y() == -std::numeric_limits<float>::infinity())
		return;

	int thickness = 4;
	int lineType = 8;
	cv::line( img,
		cv::Point(static_cast<int>(start.x()), static_cast<int>(start.y())),
		cv::Point(static_cast<int>(end.x()), static_cast<int>(end.y())),
		color,
		thickness,
		lineType );
}

void bodyJoint(cv::Mat img, Eigen::Vector2f point )
{
	if (point.x() == std::numeric_limits<float>::infinity() || point.y() == std::numeric_limits<float>::infinity())
		return;

	int thickness = 4;
	int radius = 10;
	cv::circle(img, 
			cv::Point(static_cast<int>(point.x()), static_cast<int>(point.y())), 
			radius, 
			cv::Scalar(255, 255, 255), 
			thickness);

}

void DrawSkeletons(cv::Mat img, const SkeletonSet &skeletons)
{
	// TODO: Color me differently

	for (size_t i = 0; i < skeletons.size(); i++)
	{
		if (!skeletons.at(i).isTracked()) continue;

		const Skeleton *sk = &skeletons.at(i);
		// draw head & body
		bodySegment(img, sk->getJoint2D(Skeleton::Joint_Head), sk->getJoint2D(Skeleton::Joint_Neck), bodyColors[i]);
		bodySegment(img, sk->getJoint2D(Skeleton::Joint_Neck), sk->getJoint2D(Skeleton::Joint_SpineShoulder), bodyColors[i]);
		bodySegment(img, sk->getJoint2D(Skeleton::Joint_SpineShoulder), sk->getJoint2D(Skeleton::Joint_SpineMid), bodyColors[i]);
		bodySegment(img, sk->getJoint2D(Skeleton::Joint_SpineMid), sk->getJoint2D(Skeleton::Joint_SpineBase), bodyColors[i]);
		bodySegment(img, sk->getJoint2D(Skeleton::Joint_SpineShoulder), sk->getJoint2D(Skeleton::Joint_ShoulderRight), bodyColors[i]);
		bodySegment(img, sk->getJoint2D(Skeleton::Joint_SpineShoulder), sk->getJoint2D(Skeleton::Joint_ShoulderLeft), bodyColors[i]);
		bodySegment(img, sk->getJoint2D(Skeleton::Joint_SpineBase), sk->getJoint2D(Skeleton::Joint_HipRight), bodyColors[i]);
		bodySegment(img, sk->getJoint2D(Skeleton::Joint_SpineBase), sk->getJoint2D(Skeleton::Joint_HipLeft), bodyColors[i]);

		// Right arm
		bodySegment(img, sk->getJoint2D(Skeleton::Joint_ShoulderRight), sk->getJoint2D(Skeleton::Joint_ElbowRight), bodyColors[i]);
		bodySegment(img, sk->getJoint2D(Skeleton::Joint_ElbowRight), sk->getJoint2D(Skeleton::Joint_WristRight), bodyColors[i]);
		bodySegment(img, sk->getJoint2D(Skeleton::Joint_WristRight), sk->getJoint2D(Skeleton::Joint_HandRight), bodyColors[i]);
		bodySegment(img, sk->getJoint2D(Skeleton::Joint_HandRight), sk->getJoint2D(Skeleton::Joint_HandTipRight), bodyColors[i]);
		bodySegment(img, sk->getJoint2D(Skeleton::Joint_HandRight), sk->getJoint2D(Skeleton::Joint_ThumbRight), bodyColors[i]);

		// Left arm
		bodySegment(img, sk->getJoint2D(Skeleton::Joint_ShoulderLeft), sk->getJoint2D(Skeleton::Joint_ElbowLeft), bodyColors[i]);
		bodySegment(img, sk->getJoint2D(Skeleton::Joint_ElbowLeft), sk->getJoint2D(Skeleton::Joint_WristLeft), bodyColors[i]);
		bodySegment(img, sk->getJoint2D(Skeleton::Joint_WristLeft), sk->getJoint2D(Skeleton::Joint_HandLeft), bodyColors[i]);
		bodySegment(img, sk->getJoint2D(Skeleton::Joint_HandLeft), sk->getJoint2D(Skeleton::Joint_HandTipLeft), bodyColors[i]);
		bodySegment(img, sk->getJoint2D(Skeleton::Joint_HandLeft), sk->getJoint2D(Skeleton::Joint_ThumbLeft), bodyColors[i]);

		// Right Leg
		bodySegment(img, sk->getJoint2D(Skeleton::Joint_HipRight), sk->getJoint2D(Skeleton::Joint_KneeRight), bodyColors[i]);
		bodySegment(img, sk->getJoint2D(Skeleton::Joint_KneeRight), sk->getJoint2D(Skeleton::Joint_AnkleRight), bodyColors[i]);
		bodySegment(img, sk->getJoint2D(Skeleton::Joint_AnkleRight), sk->getJoint2D(Skeleton::Joint_FootRight), bodyColors[i]);

		// Left Leg
		bodySegment(img, sk->getJoint2D(Skeleton::Joint_HipLeft), sk->getJoint2D(Skeleton::Joint_KneeLeft), bodyColors[i]);
		bodySegment(img, sk->getJoint2D(Skeleton::Joint_KneeLeft), sk->getJoint2D(Skeleton::Joint_AnkleLeft), bodyColors[i]);
		bodySegment(img, sk->getJoint2D(Skeleton::Joint_AnkleLeft), sk->getJoint2D(Skeleton::Joint_FootLeft), bodyColors[i]);

		// draw joints
		for (size_t k = 0; k < sk->getJointCount(); k++)
			bodyJoint(img, sk->getJoint2D(static_cast<Skeleton::JointType>(k)));

	}
}


int main(int argc, const char ** argv)
{

	try 
	{

		// parse parameters from config file
		params.load ("kinect_recorder.cfg");

		// Pass parameters from the input
		params.parseArgs (argc - 1, argv + 1);
		params.print();

		std::string imageFolder = ".\\";

		// if output file is specified, create folder
		if (!params.outputFile.empty())
		{
			size_t pos = params.outputFile.find_last_of("\\");
			if (params.outputFile.find_last_of("\\") != string::npos)
			{
				imageFolder = params.outputFile.substr(0, pos);
				_mkdir (imageFolder.c_str());
			}
		}


		SkeletonSet skeletonSet;
		// test Kinect
		Kinect4WLive kl;
		kl.init();

		cv::namedWindow("Color", cv::WINDOW_KEEPRATIO);
	//	cv::namedWindow("Depth", cv::WINDOW_NORMAL);
	//	cv::namedWindow("IR", cv::WINDOW_NORMAL);
	//	cv::namedWindow("ColorMap", cv::WINDOW_NORMAL);
		cv::resizeWindow("Color", 960, 540);
	//	cv::resizeWindow("ColorMap", 570, 320);

		kl.startSkeletonSaving(params.outputFile, false);
		std::cout << "Saving skeleton to: " << params.outputFile << std::endl;

		kl.startSavingImages(imageFolder);
		std::cout << "Saving images to: " << imageFolder << std::endl;


		int ferda = 0; //.FO.

		// start the loop
		while (GetAsyncKeyState(VK_ESCAPE)>=0)
		{
			cv::Mat image;
			kl.getColorImage(image);

			kl.getSkeletons(skeletonSet);
			DrawSkeletons(image, skeletonSet);

			// then put the text itself
			std::stringstream ss;
			ss << "FPS: " << kl.getFPS() << " ferda: " << ++ferda; //.FO.
			cv::putText(image, ss.str(), cv::Point(50, 80), CV_FONT_HERSHEY_SIMPLEX, 1.5 ,cv::Scalar(0.0, 255.0, 0.0), 5, 8);

			cv::imshow("Color", image);
			cv::resizeWindow("Color", 960, 540);

/*			cv::Mat imageMap;
			kl.getColorToDepthImage(imageMap);
			cv::imshow("ColorMap", imageMap);

			cv::Mat depth;
			kl.getDepthImage(depth);
			cv::imshow("Depth", depth);

			cv::Mat ir;
			kl.getInfrared(ir);
			cv::imshow("IR", ir);
*/

			cv::waitKey(1);
			::Sleep(30);

		}

		kl.stopSkeletonSaving();
		std::cout << "Done saving skeleton." << std::endl;

		kl.stopSavingImages();
		std::cout << "Done saving images." << std::endl;

		cv::destroyAllWindows();

		kl.shutDown();

	}
	catch (const Exception &e)
	{
		cerr << (const char *) e << endl;
	}

	return 0;
}
