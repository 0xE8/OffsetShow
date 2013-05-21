#include "opencv2/core/core.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/imgproc/imgproc_c.h"
#include <iostream>

using namespace cv;
using namespace std;

int main(int, char* [])
{
	cv::VideoCapture vcap;
	const char camaddr[]="rtsp://admin:12345@192.168.1.78:554/PSIA/Streaming/channels/1";
	CvCapture *capture;
    Mat fframe, frame, curr, prev, curr64f, prev64f, hann;
    int key = 0;
	//capture = cvCreateFileCapture(camaddr);
	
	//if(!vcap.open(camaddr))
	//{
	//	cout<<"Error connecting\n";
	//	system("pause");
	//	return 0;
	//}
	//vcap.set(CV_CAP_PROP_FRAME_WIDTH, 640);
	//vcap.set(CV_CAP_PROP_FRAME_HEIGHT, 480);
	//capture=cvCaptureFromFile(camaddr);
	//cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_WIDTH, 320);
	//cvSetCaptureProperty(capture, CV_CAP_PROP_FRAME_HEIGHT, 240);
    do
    {
		//frame = cvQueryFrame(capture); 
		capture=cvCaptureFromFile(camaddr);
		frame=cvRetrieveFrame(capture, cvGrabFrame(capture));
		//vcap.read(frame);
		//int h=frame.size().height;
		//int w=frame.size().width;
		//frame.empty();

        cvtColor(frame, curr, CV_RGB2GRAY); // градаци€ серого
        if(prev.empty())
        {
            prev = curr.clone();
            createHanningWindow(hann, curr.size(), CV_64F);
        }

        prev.convertTo(prev64f, CV_64F); 
        curr.convertTo(curr64f, CV_64F);

        Point2d shift = phaseCorrelate(prev64f, curr64f, hann); // коррел€ци€
        double radius = cv::sqrt(shift.x*shift.x + shift.y*shift.y); // сила смещени€

        if(radius > 5)
        {
            Point center(curr.cols >> 1, curr.rows >> 1);
            //cv::circle(frame, center, (int)radius, cv::Scalar(255, 0, 0), 3, CV_AA);
            cv::line(frame, center, Point(center.x + (int)shift.x, center.y + (int)shift.y), cv::Scalar(255, 255, 0), 3, CV_AA);
			
        }
		char message[64];
		char buff[8];
		_itoa_s(radius, buff, 8, 10);
		strcpy_s(message, 64, "Offset is: ");
		strcat_s(message, 64, buff);
		std::cout<<message<<"\n";
        imshow("Camera 01", frame);
		cvReleaseCapture(&capture);
        key = waitKey(500);
        prev = curr.clone();
    } while((char)key != 27); // Esc to exit...
	//cvReleaseCapture(&capture);
	//vcap.release();
    return 0;
}