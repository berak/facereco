#include "FaceReco22.h"


// e:/data/*.pgm
#ifdef _WIN32
	std::vector<std::string> readdir( const char * dmask ) 
	{
		std::vector<std::string> vec;
		HANDLE hFind;
		WIN32_FIND_DATA FindFileData;
		if ((hFind = FindFirstFile(dmask, &FindFileData)) != INVALID_HANDLE_VALUE)
		{
			do {
				vec.push_back( FindFileData.cFileName );
			} while(FindNextFile(hFind, &FindFileData));
			FindClose(hFind);
		}
		return vec;
	}
#else
	#include <dirent.h>
	std::vector<std::string> readdir( const char * dmask ) 
	{
		std::vector<std::string> vec;
		DIR *hdir = opendir(dmask);
		while(dirent *entry = readdir(hdir) )
			vec.push_back(entry->d_name);
		closedir(hdir);
		return vec;
	}
#endif



FaceReco22::FaceReco22( const char *haarFaceCascadeFilename )
{
	if ( ! cascade.load( haarFaceCascadeFilename ) )
	{
		printf("ERROR: Could not load Haar cascade Face detection classifier in '%s'.\n", haarFaceCascadeFilename);
		exit(1);
	}
}


cv::Mat FaceReco22::convert( const cv::Mat & mat )
{
	cv::Mat r1,r2;
	cv::resize( mat, r1, cv::Size( faceWidth, faceHeight ) );

	if ( r1.channels() > 1 )
		cv::cvtColor( r1, r2, CV_BGR2GRAY);
	else
		r2 = r1;

	cv::equalizeHist( r1, r2 );
	return r2;
}

cv::Mat FaceReco22::detect( const cv::Mat & img, cv::Rect & rect )
{
	std::vector<cv::Rect> faces;

	cv::Mat grey;
	if ( img.channels() > 1 )
		cv::cvtColor( img, grey, CV_RGB2GRAY );
	else
		grey = img;

	cascade.detectMultiScale( grey, faces, 1.1, 3, 
		CV_HAAR_FIND_BIGGEST_OBJECT	| CV_HAAR_DO_ROUGH_SEARCH  ,
		cv::Size(20, 20) );

	if ( faces.size() )
	{
		rect = faces[0];
		return convert( grey( rect ) );
	}
	return cv::Mat();
}


bool FaceReco22::classify( const cv::Mat & img, int & person, std::string & name, float & confidence )
{
	if (nEigens() < 1)
		return 0;

	cv::Mat projectedTestFace = pca.project( img.reshape(1,1) );

	double leastDistSq = DBL_MAX;
	int iNearest = -1;

	// find nearest neighbour:
	for(int iTrain=0; iTrain<nFaces(); iTrain++)
	{
		double distSq = cv::norm( projectedTestFace, projectedTrainFaces[iTrain], CV_L2 );
		if(distSq < leastDistSq)
		{
			leastDistSq = distSq;
			iNearest = iTrain;
		}
	}

	confidence = 1.0f - (float)sqrt( leastDistSq / (float)(nFaces() * nEigens()) );

	if ( iNearest > -1 )
	{
		person = personIds[ iNearest ];
		name = personNames[person - 1];
		return true;
	}
	return false;
}


// split by '_', return front.
std::string parseName( const char * fileName )
{
	char personName[256];
	strcpy(personName,fileName);
	char* split=strchr(personName,'_');
	if ( ! split )
		return "";
	*split=0;
	return personName;
}


int FaceReco22::train( const char * dirname )
{
	personIds.clear();
	personNames.clear();
	projectedTrainFaces.clear();

	// ---------------------------------------------------------------------------------------------------
	//
	// step 1, load train images (from dir):
	//
	char dirmask[200];
	sprintf(dirmask, "%s/*.pgm",dirname);
	std::vector<std::string> fnames = readdir(dirmask);
	if ( fnames.empty() )
	{
		fprintf(stderr, "No Files in %s\n", dirmask);
		return 0;
	}
	int nTrainFaces = fnames.size();

	// load images, resolve person names / ids:
	std::vector<cv::Mat> images;
	int count[255]={0};
	for(int i=0; i<nTrainFaces; i++)
	{
		// split filename into person name and random junk
		std::string personName = parseName(fnames[i].c_str());
		int personNumber=-1;
		for ( size_t p=0; p<personNames.size(); ++p )
		{	// if it's a known person, resolve id.
			if ( personName == personNames[p] )
			{
				personNumber = p + 1;
				break;
			}
		}
		if ( personNumber==-1 )
		{	// else add a new one:
			personNumber = personNames.size();
			personNames.push_back( personName );
		}
		// save person id for this image:
		personIds.push_back( personNumber );
		count[ personNumber ] ++;
		// now load the image:
		std::string imgPath( dirname );
		images.push_back( cv::imread(imgPath + "/" + fnames[i], CV_LOAD_IMAGE_GRAYSCALE) );
	}

	printf("Data loaded from '%s': (%d images of %d people).\n", dirmask, nTrainFaces, nPersons());
	for (size_t i=0; i<personNames.size(); i++) 
		printf(" <%s %d> ", personNames[i].c_str(), count[i+1]);
	printf("\n.");
	

	// ---------------------------------------------------------------------------------------------------
	//
	// step 2: do pca on them: 
	//
	// collect the train images into rows of a big mat:
	int nPixel = faceWidth * faceHeight;
	cv::Mat desc_mat( nTrainFaces, nPixel, CV_32FC1);
	for(int i = 0; i < nTrainFaces; i++)
	{
		uchar* im  = images[i].data;
		float* ptr = desc_mat.ptr<float>(i);
		for (int j = 0; j < nPixel; j++)
			*ptr++ = float( im[j] ) / 255.0f;
	}
	printf(".");

	pca( desc_mat, cv::Mat(), CV_PCA_DATA_AS_ROW, nEigens() );

	cv::norm( pca.eigenvectors, CV_L1 );


	// ---------------------------------------------------------------------------------------------------
	//
	// step 3: project all of the test faces to pca space, keep them for later comparison:
	//
	for ( int i=0; i<nTrainFaces; i++ )
	{
		cv::Mat proj = pca.project( images[i].reshape(1,1) );
		projectedTrainFaces.push_back( proj );
		
		if ( i%100 == 0 ) printf("."); // give some feedback.
	}

	printf(".\ntraining done.\n" );
	return nFaces();
}



int FaceReco22::appendPerson( const cv::Mat & img, const char *name )
{
	char cstr[400];
	int id = cv::getTickCount() % 999999; // pseudo - random
	sprintf(cstr, "data/%s_%d.pgm", name, id);

	cv::imwrite( cstr, img );
	printf("append %s\n", cstr );
	return id;
}


// you're so vain.
void FaceReco22::saveEigens( int n )
{
	int nf = std::min(n,nEigens());
	for ( int i=0; i<nf; ++i )
	{
		cv::Mat rs = pca.eigenvectors.row(i).reshape(1,faceHeight); 

		// norm to min/max loc:
		double minVal, maxVal;
		cv::minMaxLoc(rs, &minVal, &maxVal);
		// Deal with NaN and extreme values.
		if (cvIsNaN(minVal) || minVal < -1e30)
			minVal = -1e30;
		if (cvIsNaN(maxVal) || maxVal > 1e30)
			maxVal = 1e30;
		if (maxVal-minVal == 0.0f)
			maxVal = minVal + 0.001;	// remove potential divide by zero errors.

		cv::Mat res;
		cv::convertScaleAbs( rs, res, 255.0 / (maxVal - minVal), - minVal * 255.0 / (maxVal-minVal));

		char fn[200];
		sprintf(fn, "eigen_%04d.pgm", i );
		cv::imwrite( fn, res );
	}
}


std::string personDirInfo()
{
	char buf[30];
	std::map< std::string, int > cnt;
	std::vector< std::string > pix = readdir("data\\*.pgm");
	for ( int i=0; i<pix.size(); i++ )
		cnt[ parseName(pix[i].c_str()) ] ++;
	std::string res = "[";
	res += itoa(cnt.size(),buf,10);
	res += + ":";
	res += itoa(pix.size(),buf,10);
	res += + "] ";
	for ( std::map< std::string, int >::iterator it = cnt.begin(); it != cnt.end(); ++it )
	{
		res += "<" + it->first + ":";
		res += itoa(it->second,buf,10);
		res += "> ";
	}
	return res;
}
