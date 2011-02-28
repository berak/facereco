#ifndef __VlcFaceReco_onboard__
#define __VlcFaceReco_onboard__

#include "FaceReco22.h"
#include "VlcCapture.h"
//
//#include <opencv/cv.h>
//#include <opencv/highgui.h>
//


//
// bunch of data holding face recognition state
//
struct VlcFaceReco	
{

	enum Workmode
	{
		OFF = 0,
		NEUTRAL ='N',
		DETECT  ='D',
		RECORD  ='R'
	};


	int state;
	float thresh;

	int id;
	std::string name;
	float confidence;

	std::vector< cv::Mat > records;

	FaceReco22 	reco;


	VlcFaceReco( const char * thisHaarThing );
	~VlcFaceReco();


	cv::Mat processImg( cv::Mat & img );

};


#endif // __VlcFaceReco_onboard__

