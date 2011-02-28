#ifndef __FaceReco22_onboard__
#define __FaceReco22_onboard__

//////////////////////////////////////////////////////////////////////////////////////
// 
//  Pca - Eigenfaces ported to opencv2.2, using cv::stuff anywhere. no more legacy code!
//
// 
//  internally, it works like this:
//	step 1, train the pca:
//		 1.a: load the training images and setup a person db
//		 1.b: calculate the eigenvectors for them( do the pca )
//		 1.c: using the eigenvectors, project all of the training-images to pca space, needed for later comparison.
//       (hmmm, so far, the eigenvalues are never used again..)
//
//  (later..)
//  step 2, classify an image with the face candidate, from a file or haar-face detectors:
//		 2.a: using the eigenvectors, project the test img to pca-space.
//		 2.b: find the preprojected train-img with the least distance to that 
//		 2.c: lookup, to which person this train-img belonged.
//
//
//  use it like:
//
//		FaceReco22 reco( "HaaaarCasCade.xml" );
//      // train on images from datadir:
//		reco.train("/p4p4/data");
//
//		// later, check candidate:
//		cv::Rect rect;
//		cv::Mat face = reco.detect( imgFromCaptureOrFile, rect );
//		if ( reco.classify( face, id, name, confidence ) )
//			printf( "%3.3f : %d %s\n", confidence, id, name.c_str() );
//
//  results get better with rising numbers, you'll need at least 10 persons, 30 pix each.
//
//////////////////////////////////////////////////////////////////////////////////////


#include "opencv2/highgui/highgui.hpp"
#include "opencv2/objdetect/objdetect.hpp"
#include "opencv2/imgproc/imgproc.hpp"


class FaceReco22
{
	enum											// Default dimensions for faces
	{												// att facedb: 120 x 90
		faceWidth  = 90,			
		faceHeight = 90 			
	};


	cv::PCA pca;
	cv::CascadeClassifier cascade;

	std::vector< int > personIds;					// array of person ids (indexed by eigenvector).
	std::vector< std::string > personNames;			// array of person names (indexed by the personId).
	std::vector< cv::Mat > projectedTrainFaces;		// pre-projected training faces. the projected testpic will be compared to those.

public:

	FaceReco22( const char *haarFaceCascadeFilename );
	~FaceReco22()	{}

	int nFaces()	{ return projectedTrainFaces.size(); }
	int nPersons()	{ return personNames.size(); }
	int nEigens()	{ return personIds.size(); }



	//
	// helper, resize & histogram correct test-faces to the internal format.
	// used to import images, that were not recorded by this app.
	// NO need to call that on the faces returned from detect.
	//
	cv::Mat convert( const cv::Mat & face_img );


	//
	// haar - detect a face.
	// returns cropped, converted face img (or an empty one)
	//
	cv::Mat detect( const cv::Mat & camera_img, cv::Rect & rect );


	//
	// project to pca-space and check the distance between the projected img and all preprocessed training images
	//
	// expects cropped, converted face img
	//
	// Return the confidence level based on the l2 norm.
	// it is directly related to the number of training samples showing that person, for ~300 trainfaces:
	//     confidence > 0.8  : probably the same 
	//     confidence > 0.6  : human face
	//
	bool classify( const cv::Mat & face_img, int & person, std::string & name, float & confidence );


	//
	// save a (converted) person img to the data folder under "person_6423782837.ppm"
	//
	int appendPerson( const cv::Mat & face_img, const char *name );


	//
	// Read contents of an imagedir, train on all those images listed.
	// filenames start with the person name, followed by '_' , followed by random junk.
	//
	int train( const char * dirname );

	void saveEigens(int n=0);
};

// helper:
std::vector<std::string> readdir( const char * dmask );
std::string personDirInfo();

#endif // __FaceReco22_onboard__

