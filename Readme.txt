vlc:
	#	build libvlc.lib for vc.
		open a konsole, call: dumpbin /EXPORTS libvlc.dll
		either pipe that into a file(and remove anything but the funames),
		or better, mark the region containing the function names with the mouse
		and copy/paste that to an empty file.
		add a line containing just the word EXPORTS at the top, save under libvlc.def
		then call: lib /DEF:libvlc.def   this will generate libvlc.lib.

projects:
	opencv / vlc test, just a window:
		vlc_min.cpp

	opencv-hog detector, using vlc:
		hog.cpp
		VlcCapture.cpp
		VlcCapture.h
		
	face reco 2.2 style, using vlc for input:
		face22_vlc.cpp
		FaceReco22.cpp
		FaceReco22.h
		VlcCapture.cpp
		VlcCapture.h
		VlcFaceReco.cpp
		VlcFaceReco.h


please create a data folder first (or point the path given to train to a valid one).

the exe, the vlc-dlls and the plugin dir have to be in the same dir !
 
 
