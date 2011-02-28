#include <stdio.h>
#include "VlcCapture.h"
#include <opencv/highgui.h>

//
// if you start this in DEBUG mode, the hog detector needs some minutes to initialize.
//

int main()
{
	cv::HOGDescriptor hog;
	bool doScan = false;
	std::vector<cv::Rect> found, found_filtered;
	int f = 0;

	hog.setSVMDetector(cv::HOGDescriptor::getDefaultPeopleDetector());

	cv::namedWindow("hi",1);

	cv::Size win_size( 1024, 567);
	cv::HOGDescriptor cpu_hog(win_size, cv::Size(16, 16), cv::Size(8, 8), cv::Size(8, 8), 9, 1, -1, 
		cv::HOGDescriptor::L2Hys, 0.2, true, cv::HOGDescriptor::DEFAULT_NLEVELS);

	VlcCapture player;
	player.start(  "dshow://", 640, 480 );			// first usbcam found
	//player.start( "udp://@233.3.3.3", 1024, 567 );	// my telly, sending multicast

	while( true )
	{
		cv::Mat & img = player.frame();
		if ( ! img.empty() )
		{
			found.clear();
			found_filtered.clear();
			if ( doScan )
			{
				hog.detectMultiScale(img, found, 0, cv::Size(8,8), cv::Size(32,32), 1.05, 2);
				size_t i, j;
				for( i = 0; i < found.size(); i++ )
				{
					cv::Rect r = found[i];
					for( j = 0; j < found.size(); j++ )
						if( j != i && (r & found[j]) == r)
							break;
					if( j == found.size() )
						found_filtered.push_back(r);
				}
				for( i = 0; i < found_filtered.size(); i++ )
				{
					cv::Rect r = found_filtered[i];
					// the HOG detector returns slightly larger rectangles than the real objects.
					// so we slightly shrink the rectangles to get a nicer output.
					r.x += cvRound(r.width*0.1);
					r.width = cvRound(r.width*0.8);
					r.y += cvRound(r.height*0.07);
					r.height = cvRound(r.height*0.8);
					cv::rectangle(img, r.tl(), r.br(), cv::Scalar(0,255,0), 2);
					printf( "%d :\t%3d,%3d,%3d,%3d\n", f, r.x,r.y,r.width,r.height );
				}
				doScan = false;
				printf( "%d :\t%d :\t%d\n", f, found.size(), found_filtered.size() );
			}
			cv::imshow( "hi", img );
		}
		f ++;
		int key = cv::waitKey(10);
		if ( key == 0x1b )
			break;
		if ( key == ' ' )
			doScan = ! doScan;
	}
	return 0;
}

