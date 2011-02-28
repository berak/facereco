#include "opencv2/highgui/highgui.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"

//
//
// minimal vlc/opencv2.2  harrr face detector
//
//


#include <stdio.h>
#include <stdlib.h>
#include "VlcCapture.h"



int main()
{
	const char *faceCascadeFilename = "E:\\code\\opencv22\\data\\haarcascades\\haarcascade_frontalface_alt.xml";
	cv::CascadeClassifier cascade;
	cv::namedWindow("hi",0);

	VlcCapture player;
	//player.start( call_face, "dshow://", 640, 480 );			// first usbcam found on xp.
	player.start( call_face, "udp://@233.3.3.3", 1024, 567 );	// my telly, sending multicast

	while( true )
	{
		cv::Mat img = player.frame();
		if ( ! img.emty() )
		{
			cv::Mat grey;
			cv::cvtColor( img, grey, CV_BGR2GRAY );

			if ( cascade.empty() )
				cascade.load( faceCascadeFilename );

			std::vector<cv::Rect> faces;
			cascade.detectMultiScale( grey, faces, 1.1, 2, CV_HAAR_DO_ROUGH_SEARCH|CV_HAAR_SCALE_IMAGE,cv::Size(30, 30), cv::Size(45, 45) );
			for (size_t i=0; i<faces.size(); ++i) 
				cv::rectangle(img, faces[i].tl(), faces[i].br(), cv::Scalar(0,255,0), 2);
			
			cv::imshow( "hi", img );
		}
			
		int key = cv::waitKey(20);
		if ( key == 0x1b )
			break;
	}
	return 0;
}

