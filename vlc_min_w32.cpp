 #include <stdio.h>
 #include <stdlib.h>
 #include <vlc/vlc.h>
 
 #include <windows.h>

LRESULT CALLBACK WindowCallBack(HWND hWnd,UINT msg,WPARAM w,LPARAM l)
{
	if ( msg==WM_DESTROY || (msg==WM_KEYDOWN && LOWORD(w)==0x1b) )
	{
		PostQuitMessage(0);
		return 0;
	}
	return DefWindowProc(hWnd,msg,w,l);
}

HWND OpenWindow()
{
	HINSTANCE inst=GetModuleHandle(NULL);
	WNDCLASS cls = {0};
	cls.hInstance=inst;
	cls.lpszClassName="p1234";
	cls.lpfnWndProc=WindowCallBack;
	cls.hbrBackground=(HBRUSH)GetStockObject(WHITE_BRUSH);
	RegisterClass(&cls);
	HWND hWnd = CreateWindow( "p1234","p1234", WS_OVERLAPPEDWINDOW|WS_CLIPSIBLINGS|WS_CLIPCHILDREN,
		    0,0,400,400,  NULL,NULL,inst,NULL );
	ShowWindow(hWnd,SW_SHOWNORMAL);
	return hWnd;
}

static void MainLoop(int sleepTime=0)
{
	MSG msg={0};
	while(msg.message!= WM_QUIT)
	{
		while(PeekMessage(&msg,0,0,0,PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}
		Sleep(sleepTime);
	}
}


struct VlcPlayer
{
	libvlc_instance_t * inst;
	libvlc_media_player_t *mp;

	VlcPlayer()
		: inst(0)
		, mp(0)
	{
	}
	~VlcPlayer()
	{
		if( mp ) 
		{
			libvlc_media_player_stop (mp);
			libvlc_media_player_release (mp);
		}
		if ( inst  ) libvlc_release (inst);
	}

	void start( const char * stream = "udp://@233.3.3.3", int wantedW=640, int wantedH=480  )
	{
		const char * const vlc_args[] = {
			"-I", "dummy", // Don't use any interface
			"--ignore-config", // Don't use VLC's config
			"--no-audio", // skip any audio track 
			"--verbose=2", // Be much more verbose then normal for debugging purpose
		};

		inst = libvlc_new(sizeof(vlc_args) / sizeof(vlc_args[0]), vlc_args);

		libvlc_media_t *m = libvlc_media_new_path (inst, stream);
		mp = libvlc_media_player_new_from_media (m);
		libvlc_media_release (m);

		HWND hwnd = OpenWindow();
		libvlc_media_player_set_hwnd (mp, hwnd);
		libvlc_media_player_play(mp);
	}

};



int main(int argc, char* argv[])
{
	char *startup = argc>1 ? argv[1] : "dshow://";
	VlcPlayer player;
	player.start(startup);

	MainLoop(100);
	return 0;
}

