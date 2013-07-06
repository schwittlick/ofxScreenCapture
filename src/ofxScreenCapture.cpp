#include "ofxScreenCapture.h"

ofxScreenCapture::ofxScreenCapture(void)
{
	
	hScreen = GetDC(NULL);
    ScreenX = GetDeviceCaps(hScreen, HORZRES);
    ScreenY = GetDeviceCaps(hScreen, VERTRES);
    hdcMem = CreateCompatibleDC (hScreen);
    hBitmap = CreateCompatibleBitmap(hScreen, ScreenX, ScreenY);
    HGDIOBJ hOld = SelectObject(hdcMem, hBitmap);
    BitBlt(hdcMem, 0, 0, ScreenX, ScreenY, hScreen, 0, 0, SRCCOPY);
    SelectObject(hdcMem, hOld);

    bmi;
    bmi.biSize = sizeof(BITMAPINFOHEADER);
    bmi.biPlanes = 1;
    bmi.biBitCount = 32;
    bmi.biWidth = ScreenX;
    bmi.biHeight = -ScreenY;
    bmi.biCompression = BI_RGB;
    bmi.biSizeImage = 0;// 3 * ScreenX * ScreenY;

	pPixels = new RGBQUAD[ScreenX * ScreenY];
}


ofxScreenCapture::~ofxScreenCapture(void)
{
	delete pPixels;
}

void ofxScreenCapture::update( void ) 
{
	hScreen = GetDC(NULL);
	ScreenX = GetDeviceCaps(hScreen, HORZRES);
	ScreenY = GetDeviceCaps(hScreen, VERTRES);
	hdcMem = CreateCompatibleDC (hScreen);
	hBitmap = CreateCompatibleBitmap(hScreen, ScreenX, ScreenY);
	HGDIOBJ hOld = SelectObject(hdcMem, hBitmap);
	BitBlt(hdcMem, 0, 0, ScreenX, ScreenY, hScreen, 0, 0, SRCCOPY);
	SelectObject(hdcMem, hOld);

	GetDIBits(hdcMem, hBitmap, 0, ScreenY, pPixels, (BITMAPINFO*)&bmi, DIB_RGB_COLORS);
	ReleaseDC(NULL,hScreen);
	DeleteDC(hdcMem);

}

void ofxScreenCapture::getImage( ofImage& im ) 
{
	im.allocate(ScreenX, ScreenY, OF_IMAGE_COLOR_ALPHA);
	
	for(int y = 0; y < ScreenY; ++y )
	{
		for( int x = 0; x < ScreenX; ++x )
		{
			int index = y * ScreenX + x;

			RGBQUAD p = pPixels[index];
			int r = p.rgbRed;
			int g = p.rgbGreen;
			int b = p.rgbBlue;

			ofColor c(r, g, b, 255);
			im.setColor(x, y, c);
		}
	}
}


bool ofxScreenCapture::equals( ofImage* first, ofImage* second )
{
	bool ret = true;
	ofPixels* first_image_pixelsref = &first->getPixelsRef();
	ofPixels* second_image_pixelsref = &second->getPixelsRef();
	unsigned char* first_image_pixelsref_array = first_image_pixelsref->getPixels();
	unsigned char* second_image_pixelsref_array = second_image_pixelsref->getPixels();
#pragma omp parallel num_threads(8)
	{
		int ct = omp_get_thread_num();
		int num = omp_get_num_threads();
		int pt = first->getHeight() / num;
		int start = ct * pt;
		int end = (ct + 1) * pt;
		for( int y=start; y < end; ++y )
		{
			for (int x = 0; x < first->getWidth(); ++x )
			{
				int index = y * first->getWidth() + x;
				int *firstColor = (int*)(&first_image_pixelsref_array[index]);
				int *secondColor = (int*)(&second_image_pixelsref_array[index]);
				if(&firstColor != &secondColor )
				{
					ret = false;
				}
			}
		}
	}
	return ret;
}
