
:: Set environmental variables FOR KINECT 2.0

@echo off

echo 
SET JSON_ROOT=C:\Libraries\jsoncpp-master
SET EIGEN_ROOT=C:\Libraries\PCL\Eigen
SET OPENCV_HOME=C:\Libraries\OpenCV_2.4.9


echo on
SETX JSON_ROOT "%JSON_ROOT%"
SETX EIGEN_ROOT "%EIGEN_ROOT%"
SETX OPENCV_HOME "%OPENCV_HOME%"


echo on