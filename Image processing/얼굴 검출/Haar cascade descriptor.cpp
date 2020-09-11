#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <opencv2/imgproc/imgproc.hpp>     // cv::Canny()
#include <opencv2/core/core.hpp>

#include <stdio.h>
#include <iostream>

using namespace cv;
using namespace std;

#pragma comment(lib, "opencv_core2413d.lib")
#pragma comment(lib,"opencv_objdetect2413d.lib")
#pragma comment(lib, "opencv_highgui2413d.lib")
#pragma comment(lib, "opencv_imgproc2413d.lib")

int main(int argc, char* argv[])
{
	VideoCapture cap(0); // open the video camera no. 0

	if (!cap.isOpened())  // if not success, exit program
	{
		cout << "Cannot open the video file" << endl;
		return -1;
	}

	double dWidth = cap.get(CV_CAP_PROP_FRAME_WIDTH); //get the width of frames of the video
	double dHeight = cap.get(CV_CAP_PROP_FRAME_HEIGHT); //get the height of frames of the video

	cout << "Frame size : " << dWidth << " x " << dHeight << endl;

	namedWindow("기본영상", CV_WINDOW_AUTOSIZE); //create a window called "MyVideo"
	namedWindow("검출영상", CV_WINDOW_AUTOSIZE); //create a window called "MyVideo"

	while (1)
	{
		Mat frame, result;

		bool bSuccess = cap.read(frame); // read a new frame from video

		if (!bSuccess) //if not success, break loop
		{

			cout << "Cannot read a frame from video file" << endl;
			break;
		}

		imshow("기본영상", frame); //show the frame in "MyVideo" window
		// Mat copy
		result = frame.clone();
		// Load Face cascade (.xml file)
		CascadeClassifier face_cascade;
		face_cascade.load("D:/opencv2.4.13/sources/data/haarcascades/haarcascade_frontalface_alt2.xml");

		// Detect faces
		std::vector<Rect> faces;
		face_cascade.detectMultiScale(result, faces, 1.1, 2, 0 | CV_HAAR_SCALE_IMAGE, Size(30, 30));

		// Draw circles on the detected faces
		for (int i = 0; i < faces.size(); i++)
		{
			Point center(faces[i].x + faces[i].width*0.5, faces[i].y + faces[i].height*0.5);
			ellipse(result, center, Size(faces[i].width*0.5, faces[i].height*0.5), 0, 0, 360, Scalar(255, 0, 0), 2, 8, 0);
		}

		//edge = 255 - edge; // 화면 반전
		imshow("검출영상", result); //show the frame in "Result" window

		if (waitKey(30) == 27) //wait for 'esc' key press for 30ms. If 'esc' key is pressed, break loop
		{
			cout << "esc key is pressed by user" << endl;
			break;
		}
	}

	waitKey(0);
	return 0;
}