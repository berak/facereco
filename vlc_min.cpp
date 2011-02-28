
#include <opencv/highgui.h>
#include "VlcCapture.h"


int main(int argc, char **argv)
{

	char * startup = argc>1 ? argv[1] : "dshow://";
	int w = argc>2 ? atoi(argv[2]) : 640;
	int h = argc>3 ? atoi(argv[3]) : 480;

	cv::namedWindow("hi",1);

	VlcCapture player;
	player.start( startup, w, h );			// first usbcam found
	//player.start( "udp://@233.3.3.3", 1024, 567 );	// my telly, sending multicast

	while( true )
	{
		cv::Mat frame = player.frame();
		if ( ! frame.empty() )
			cv::imshow( "hi", frame );
		int key = cv::waitKey(20);
		if ( key == 0x1b )
			break;
	}
	return 0;
}

