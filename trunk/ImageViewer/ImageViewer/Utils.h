#pragma once

class Utils
{
public:
	Utils(void);
	~Utils(void);

	/*static int CString2char(CString cstr, char** nstring)
	{
		size_t origsize = wcslen(cstr) + 1;
		size_t convertedChars = 0;
		*nstring=(char*)malloc(origsize);
		wcstombs_s(&convertedChars, *nstring, origsize, cstr, _TRUNCATE);
		return convertedChars;
	}*/

	static IplImage* DIB2IplImage(HBITMAP hbmp, HDC hdc = NULL) 
	{ 
		IplImage* Image = NULL; 

		bool mustrelease = false; 
		BITMAPINFO bmi; 
		BITMAPINFOHEADER* bmih = &(bmi.bmiHeader); 
		if (hdc == NULL) 
		{ 
			hdc = GetDC(NULL); 
			mustrelease = true; 
		} 
		ZeroMemory(bmih, sizeof(BITMAPINFOHEADER)); 
		bmih->biSize = sizeof(BITMAPINFOHEADER); 
		if (GetDIBits(hdc, hbmp, 0, 0, NULL, &bmi, DIB_RGB_COLORS)) 
		{ 
			int height = (bmih->biHeight > 0) ? bmih->biHeight : -bmih->biHeight; 
			Image = cvCreateImage(cvSize(bmih->biWidth, height), IPL_DEPTH_8U, 3); 
			Image->origin = (bmih->biHeight > 0); 
			bmih->biBitCount = 24; 
			bmih->biCompression = BI_RGB; 
			GetDIBits(hdc, hbmp, 0, height, Image->imageData, &bmi, DIB_RGB_COLORS); 
		} 

		if (mustrelease) 
			ReleaseDC(NULL, hdc); 

		/*cvShowImage("eee",Image);
		cvWaitKey();
		cvDestroyWindow("eee");*/
		return Image; 
	}

	static HBITMAP IplImage2DIB(IplImage Image)
	{
		int bpp = Image.nChannels * 8;
		assert(Image.width >= 0 && Image.height >= 0 &&
				(bpp == 8 || bpp == 24 || bpp == 32));
		CvMat dst;
		void* dst_ptr = 0;
		HBITMAP hbmp = NULL;
		unsigned char buffer[sizeof(BITMAPINFO) + 255*sizeof(RGBQUAD)];
		BITMAPINFO* bmi = (BITMAPINFO*)buffer;
		BITMAPINFOHEADER* bmih = &(bmi->bmiHeader);
        
		ZeroMemory(bmih, sizeof(BITMAPINFOHEADER));
		bmih->biSize = sizeof(BITMAPINFOHEADER);
		bmih->biWidth = Image.width;
		bmih->biHeight = Image.origin ? abs(Image.height) :-abs(Image.height);
		bmih->biPlanes = 1;
		bmih->biBitCount = bpp;
		bmih->biCompression = BI_RGB;
        
		if (bpp == 8) {
				RGBQUAD* palette = bmi->bmiColors;
				int i;
				for (i = 0; i < 256; i++) {
						palette[i].rgbRed = palette[i].rgbGreen = palette[i].rgbBlue = (BYTE)i;
						palette[i].rgbReserved = 0;
				}
		}
        
		hbmp = CreateDIBSection(NULL, bmi, DIB_RGB_COLORS, &dst_ptr, 0, 0);
		int type;
		if(Image.nChannels==1)
			type=CV_8UC1;
		else
			type=CV_8UC3;
		cvInitMatHeader(&dst, Image.height, Image.width, type,
				dst_ptr, (Image.width * Image.nChannels + 3) & -4);
		cvConvertImage(&Image, &dst, Image.origin ?  CV_CVTIMG_FLIP :0);
        
		return hbmp;
	}
};