#pragma once

#include <iostream>
#include <ole2.h>
#include <olectl.h>
#include "ofMain.h"
#include <time.h>

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

private:
	RGBQUAD *pPixels;
	int ScreenX, ScreenY;
	HDC hdcMem;
	HBITMAP hBitmap;
	HDC hScreen;
	BITMAPINFOHEADER bmi;
};

