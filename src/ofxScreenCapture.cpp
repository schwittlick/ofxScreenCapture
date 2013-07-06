#include "ofxScreenCapture.h"

ofxScreenCapture::ofxScreenCapture(void)
{
	screen_context = GetDC(NULL);
    screen_x = GetDeviceCaps(screen_context, HORZRES);
    screen_y = GetDeviceCaps(screen_context, VERTRES);
    compatible_screen_context = CreateCompatibleDC (screen_context);
    screen_bitmap = CreateCompatibleBitmap(screen_context, screen_x, screen_y);
    HGDIOBJ hOld = SelectObject(compatible_screen_context, screen_bitmap);
    BitBlt(compatible_screen_context, 0, 0, screen_x, screen_y, screen_context, 0, 0, SRCCOPY);
    SelectObject(compatible_screen_context, hOld);

    info;
    info.biSize = sizeof(BITMAPINFOHEADER);
    info.biPlanes = 1;
    info.biBitCount = 32;
    info.biWidth = screen_x;
    info.biHeight = -screen_y;
    info.biCompression = BI_RGB;
    info.biSizeImage = 0;

	screenPixels = new RGBQUAD[screen_x * screen_y];
}


ofxScreenCapture::~ofxScreenCapture(void)
{
	delete screenPixels;
}

void ofxScreenCapture::update( void ) 
{
	screen_context = GetDC(NULL);
	screen_x = GetDeviceCaps(screen_context, HORZRES);
	screen_y = GetDeviceCaps(screen_context, VERTRES);
	compatible_screen_context = CreateCompatibleDC (screen_context);
	screen_bitmap = CreateCompatibleBitmap(screen_context, screen_x, screen_y);
	HGDIOBJ hOld = SelectObject(compatible_screen_context, screen_bitmap);
	BitBlt(compatible_screen_context, 0, 0, screen_x, screen_y, screen_context, 0, 0, SRCCOPY);
	SelectObject(compatible_screen_context, hOld);

	GetDIBits(compatible_screen_context, screen_bitmap, 0, screen_y, screenPixels, (BITMAPINFO*)&info, DIB_RGB_COLORS);
	ReleaseDC(NULL,screen_context);
	DeleteDC(compatible_screen_context);

}

void ofxScreenCapture::getImage( ofImage& im ) 
{
	im.allocate(screen_x, screen_y, OF_IMAGE_COLOR_ALPHA);
	
	for(int y = 0; y < screen_y; ++y )
	{
		for( int x = 0; x < screen_x; ++x )
		{
			int index = y * screen_x + x;

			RGBQUAD p = screenPixels[index];
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
				int firstColor = first_image_pixelsref_array[index];
				int secondColor = second_image_pixelsref_array[index];

				if(firstColor != secondColor )
				{
					ret = false;
				}
			}
		}
	}
	return ret;
}
