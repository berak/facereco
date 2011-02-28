#include "VlcFaceReco.h"

VlcFaceReco::~VlcFaceReco() {}
VlcFaceReco::VlcFaceReco( const char *haarPath) 
	: reco(haarPath)
	, state(NEUTRAL)
	, thresh(0.835f)
	, id(0)
	, name("")
	, confidence(0)
 {}



cv::Mat VlcFaceReco::processImg( cv::Mat & img )
{
	cv::Mat frame;
	cv::resize( img, frame, cv::Size(400,300) );

	if ( state != NEUTRAL )		
	{
		cv::Rect rect;
		cv::Mat face = reco.detect( frame, rect );
		if ( !face.empty() )
		{
			if ( state == DETECT )
			{

				reco.classify( face, id, name, confidence );

				char *pre = ( confidence > thresh ) ? "***  " :  "     ";
				printf( "%s(%3.3f) : %2d %-12s [%3d %3d %3d %3d]\n", 
					pre, confidence, id, name.c_str(),
					rect.x, rect.y, rect.width, rect.height  );

				if ( confidence > thresh )
				{
					char text[256];
					sprintf(text, "%s %3.3f", name.c_str(), confidence);
					cv::putText(frame, text, cvPoint(rect.x, rect.y + rect.height + 15),CV_FONT_HERSHEY_PLAIN,1.0, CV_RGB(0,255,255));
				}
			}

			if ( state == RECORD )
			{
				static int _let_them_move_a_bit=0;
				if ( ++ _let_them_move_a_bit % 3 == 1 )
				{
					printf(".");
					records.push_back( face );
				}
			}

			// green rects..
			cv::rectangle( frame, cv::Point(rect.x,rect.y),cv::Point(rect.x+rect.width,rect.y+rect.height),cv::Scalar(0,255,0));
		}
	}
//	cv::imshow( ";)",frame);
	return frame;
}
