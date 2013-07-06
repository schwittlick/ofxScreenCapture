#pragma once

#include <iostream>
#include <ole2.h>
#include <olectl.h>
#include "ofMain.h"
#include <time.h>
#include <omp.h>

/*
 * author: marcel schwittlick (mrzl)
 * date: 7/5/2013
 *
 * notes:
 * as of the 7/5/2013 its working for windows only so far. tested on windows8.
*/
class ofxScreenCapture
{
public:
	ofxScreenCapture(void);
	~ofxScreenCapture(void);

	void update( void );
	void getImage( ofImage& im ); // benchmarked: ~70-80ms for 1920x1080

	bool equals( ofImage* first, ofImage* second ); // utility to compare two ofimages

private:
	RGBQUAD *screenPixels;
	int screen_x, screen_y;
	HBITMAP screen_bitmap;
	HDC compatible_screen_context;
	HDC screen_context;
	BITMAPINFOHEADER info;
};

