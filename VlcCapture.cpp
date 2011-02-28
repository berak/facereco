#include <stdio.h>
#include <stdlib.h>
#include "VlcCapture.h"

VlcCapture::VlcCapture()
	: inst(0)
	, mp(0)
	, pixel(0)
	, w(0)
	, h(0)
{
	const char * const vlc_args[] = {
		"-I", "dummy",		// Don't use any interface
		"--ignore-config",	// Don't use VLC's config
		"--no-audio",		// skip any audio track 
		"--verbose=1",		// change verbosity for debugging purpose
	};

	inst = libvlc_new(sizeof(vlc_args) / sizeof(vlc_args[0]), vlc_args);
	mp   = libvlc_media_player_new(inst);

	libvlc_video_set_callbacks( mp, &cb_lock, &cb_unlock, &cb_display, this );
}

VlcCapture::~VlcCapture()
{
	libvlc_media_player_stop (mp);
	libvlc_media_player_release (mp);

	libvlc_release (inst);
}

//
// whatever the choosen size was, need to alloc space for the whole input image here !
//
unsigned char * VlcCapture::updateSize()
{
	int W = libvlc_video_get_width( mp );
	int H = libvlc_video_get_height( mp );
	if ( !W || !H )
		return 0; // vlc will choke on this, but keep on playing.

	if ( pixel && (this->w != W || this->h != H) )
	{
		delete[] pixel;
		pixel = 0;
	}
	if ( !pixel )
	{
		pixel = new unsigned char [ W * H * 3 ];
		this->w = W;
		this->h = H;
	}
	return pixel;
}

//
// the callbacks for the pixel data:
//
void * VlcCapture::cb_lock(void *opaque, void **plane) 
{
	VlcCapture * p = (VlcCapture*)opaque;
	*plane = p->updateSize();
	return *plane;
}

void VlcCapture::cb_unlock(void *opaque, void *picture, void *const *plane) 
{
	VlcCapture * p = (VlcCapture*)opaque;
	unsigned char* pix = (unsigned char*)(picture);
	if ( pix )
	{
		p->critter.lock();
		p->img = cv::Mat( p->h, p->w, CV_8UC3, pix ); //.clone();
		p->critter.unlock();
	}
}


void VlcCapture::cb_display(void *opaque, void *picture) 
{
}


void VlcCapture::start( const char * url, int W, int H  )
{
	libvlc_media_player_pause (mp);
	libvlc_media_t *media = libvlc_media_new_location(inst, url);
	libvlc_media_player_set_media(mp, media);
	libvlc_media_release (media);

	libvlc_video_set_format( mp, "RV24", W,H, W * 3 );

	libvlc_media_player_play(mp);
}


void VlcCapture::pause( bool paused )
{
	if ( mp ) 
	{
		libvlc_media_player_set_pause(mp, paused);
	}
}


