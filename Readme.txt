vlc:
	#	get latest vlc kit from http://nightlies.videolan.org/build/win32/last/
		1.2.0, because it contains vlc_video_set_format_cb()
	#	build libvlc.lib for vc.
		open a konsole, call: dumpbin /EXPORTS libvlc.dll
		either pipe that into a file(and remove anything but the funames),
		or better, mark the region containing the function names with the mouse and copy/paste that.
		add a line containing just the word EXPORTS at the top, save under libvlc.def
		then call: lib /DEF:libvlc.def   this will generate libvlc.lib.


projects:
	win32 vlc test, just a window:
		vlc_min.cpp

	opencv-hog detector, using vlc:
		hog.cpp
		VlcPlayer.cpp
		VlcPlayer.h
		
	face reco 2.2 style, using vlc for input:
		face22_vlc.cpp
		FaceReco22.cpp
		FaceReco22.h
		VlcPlayer.cpp
		VlcPlayer.h
		
	face reco 2.2 style, using opencv for input:
		face22_opencv.cpp
		face22_vlc.cpp
		FaceReco22.cpp
		FaceReco22.h
		


please create a data folder first (or point the path given to train to a valid one).

the exe, the vlc-dlls and the plugin dir have to be in the same dir !
 
 
