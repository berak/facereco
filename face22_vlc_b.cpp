// libs: libvlc.lib opencv_calib3d220.lib opencv_contrib220.lib opencv_core220.lib opencv_features2d220.lib opencv_ffmpeg220.lib opencv_flann220.lib opencv_gpu220.lib opencv_highgui220.lib opencv_imgproc220.lib opencv_legacy220.lib opencv_ml220.lib opencv_objdetect220.lib opencv_ts220.lib opencv_video220.lib 


#include <stdio.h>
#include <stdlib.h>

#include "FaceReco22.h"
#include "VlcCapture.h"
#include "VlcFaceReco.h"

#include <opencv/cv.h>
#include <opencv/highgui.h>


//
//  keys:
//  <any> revert to neutral
//  'd' start detection (after training)
//  't' train on images in datadir
//  'r' start recording images ( every 3rd frame )
//      hitting any key after that will stop recording and ask you for a name 
//		on the console. give empty name to discard the pics
//  'k' type a name on console to move a person from datadir to datab
//  'l' type a name on console to move a person from datab to datadir
//  'i' info for datadir
//  '-' '+' adjust recognizing threshold
//  'p' change input stream, type url on konsole.
//  ESC quit
//

int main( int argc, char** argv )
{
	char * url = argc>1 ? argv[1] : "dshow://";
	int w = argc>2 ? atoi(argv[2]) : 640;
	int h = argc>3 ? atoi(argv[3]) : 480;
	cv::namedWindow(";)", CV_WINDOW_AUTOSIZE);

	VlcFaceReco faceReco("E:\\code\\opencv22\\data\\haarcascades\\haarcascade_frontalface_alt.xml");

	VlcCapture cap;
	cap.start( url, w, h );

	//player.start( "dshow://", 640, 480 );				// first usbcam found
	//player.start( "udp://@233.3.3.3", 1024, 567 );	// my telly, sending multicast

	char pname[600];
	while ( faceReco.state )
	{
		cv::Mat & frame = cap.frame();
		if ( ! frame.empty() )
		{
			cv::Mat & res = faceReco.processImg( frame );
			cv::imshow(";)", res );
		}

		int k = cvWaitKey(30);
		if ( k > 0 )
		{
			if ( faceReco.state == VlcFaceReco::RECORD )
			{
				faceReco.state = VlcFaceReco::NEUTRAL; 
				printf("\nperson> ");
				gets(pname);
				if ( pname[0] != 0 && pname[0] != '\r' && pname[0] != '\n' )
				{
					for ( int i=0; i<faceReco.records.size(); ++i )
					{
						faceReco.reco.appendPerson( faceReco.records[i], pname );
					}
				}
			}
			faceReco.state = VlcFaceReco::NEUTRAL; // hitting space will stop anything

			if ( k == 0x1b )	
			{
				cap.pause(true);
				break;
			}
			if ( k == 'd' )	
			{
				faceReco.state = VlcFaceReco::DETECT;
			}
			if ( k == 'r' )
			{
				faceReco.records.clear();
				faceReco.state = VlcFaceReco::RECORD;
			}
			if ( k == 'k' ) // k is fo kill.
			{
				printf("kill> ");
				gets(pname);
				if ( pname[0] != 0 && pname[0] != '\r' && pname[0] != '\n' )
				{
					char cmd[500];
					sprintf(cmd, "person.bat remove %s", pname );
					system(cmd);
				}
			}
			if ( k == 'l' ) // restore
			{
				printf("restore> ");
				gets(pname);
				if ( pname[0] != 0 && pname[0] != '\r' && pname[0] != '\n' )
				{
					char cmd[500];
					sprintf(cmd, "person.bat restore %s", pname );
					system(cmd);
				}
			}
			if ( k == 'i' )
			{
				std::string pi = personDirInfo();
				printf("%s\n", pi.c_str());
			}
			if ( k == 't' )
			{
				faceReco.reco.train("data");
				faceReco.reco.saveEigens(16);
			}
			if ( k == '-' )	
			{
				faceReco.thresh *= 0.99f;
				printf("thresh %f4.4\n", faceReco.thresh );
			}
			if ( k == '+' )	
			{
				faceReco.thresh *= 1.01f;
				printf("thresh %f4.4\n", faceReco.thresh );
			}
			if ( k == 'p' ) // change stream
			{
				//cap.pause(true);
				printf("stream> ");
				gets(pname);
				if ( pname[0] != 0 && pname[0] != '\r' && pname[0] != '\n' )
				{
					cap.start( pname );
				}
				cap.pause(false);
			}
			printf("mode %c\n", faceReco.state );
		}
	}

	return 0;
}
