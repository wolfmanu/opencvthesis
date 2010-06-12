/****************************************************************************
ViewerLib.cpp : Defines the Image Viewer DLL application.
written by PJ Arends
pja@telus.net

For updates check http://www.codeproject.com/tools/imageviewer.asp

-----------------------------------------------------------------------------
This code is provided as is, with no warranty as to it's suitability or usefulness
in any application in which it may be used.

This code may be used in any way you desire. This file may be redistributed by any
means as long as it is not sold for profit, and providing that this notice and the
author's name are included. Any modifications not made by the original author should
be clearly marked as such to remove any confusion between the original version and
any other versions.

If any bugs are found and fixed, a note to the author explaining the problem and
fix would be nice.
-----------------------------------------------------------------------------
****************************************************************************/

#define _WIN32_WINNT 0x0501     // Windows XP
#define WIN32_LEAN_AND_MEAN     // Exclude rarely-used stuff from Windows headers
#include <windows.h>
#include <commctrl.h>           // for HIMAGELIST
#include <tchar.h>
#include <sstream>
#include <string>
#include <iomanip>
#include <atlbase.h>            // for CA2W string conversion function and ATLTRACE macro
#include <sys\timeb.h>
#include <math.h>
#include "resource.h"
#include "..\common files\pja_dc.h"
#include "..\common files\pja_bitmap.h"
#include "..\common files\pja_format.h"
#include "..\common files\tstl.h"

#include "IVDllWindow.h"
CIVDllWindow Window;

HMODULE DllModule = NULL;

/////////////////////////////////////////////////////////////////////////////
//
//  DllMain
//    The dll's entry point
//
//  Parameters :
//    None
//
//  Returns :
//    TRUE
//
/////////////////////////////////////////////////////////////////////////////

BOOL APIENTRY DllMain(HMODULE hModule, DWORD, LPVOID)
{
    DllModule = hModule;
    return TRUE;
}

/////////////////////////////////////////////////////////////////////////////
//
//  CurrentTime
//    Retrieves the current system time
//
//  Parameters :
//    None
//
//  Returns :
//    A wide string containing the current system time
//
/////////////////////////////////////////////////////////////////////////////

std::wstring CurrentTime()
{
    _timeb TimeB;
    _ftime_s(&TimeB);

    std::wstring String = _wctime(&TimeB.time);

    std::wostringstream str;
    str << String.substr(0, 10) << L',' << String.substr(19, 5);
    str << String.substr(10, 9) << L'.' << std::setw(3) << std::setfill(L'0') << TimeB.millitm << std::ends;

    return str.str().c_str();
}

/////////////////////////////////////////////////////////////////////////////
//
//  class BGBrush
//    This class is used to retrieve a HBRUSH that is the same colour as that saved
//    as the background colour by the user of the Image Viewer utility.
//
/////////////////////////////////////////////////////////////////////////////

class BGBrush
{
    HBRUSH m_Brush;
    COLORREF m_Colour;

public:
    BGBrush()
    {
        // default brush colour
        m_Colour = RGB(192, 192, 192);

        HKEY hKey = NULL;
        if (ERROR_SUCCESS == RegOpenKeyEx(HKEY_CURRENT_USER,
                                          _T("Software\\PJ Arends\\Image Viewer\\Settings"),
                                          0,
                                          KEY_READ,
                                          &hKey))
        {
            DWORD Size = sizeof(COLORREF);
            RegQueryValueEx(hKey,
                            _T("Background Colour"),
                            NULL,
                            NULL,
                            (LPBYTE)&m_Colour,
                            &Size);
            RegCloseKey(hKey);
        }
        m_Brush = CreateSolidBrush(m_Colour);
    }
    
    ~BGBrush()
    {
        DeleteObject(m_Brush);
    }

    operator HBRUSH()
    {
        return m_Brush;
    }

    operator COLORREF()
    {
        // returns the ideal text colour that will contrast with this brush
        // technique by John Simmons http://www.codeproject.com/useritems/IdealTextColor.asp
        int Grey = (GetRValue(m_Colour) * 299 + GetGValue(m_Colour) * 587 + GetBValue(m_Colour) * 114) / 1000;
        return Grey > 105 ? RGB(0, 0, 0) : RGB(255, 255, 255);
    }
};

/////////////////////////////////////////////////////////////////////////////
//
//  AreWindowsReady
//    Creates the dll's hidden window and checks if the app's hidden window
//    is also present. If these two windows are not present and properly
//    setup the dll will fail to show any output.
//
//  Parameters :
//    None
//
//  Returns :
//    true if the windows are properly setup
//    false if the windows are not setup
//
/////////////////////////////////////////////////////////////////////////////

bool AreWindowsReady()
{
    if (!Window.Create())
    {
        ATLTRACE(_T("ImageViewer.dll : ImageViewer dll window not created"));
        return false;
    }

    if (!IsWindow(Window))
    {
        ATLTRACE(_T("ImageViewer.dll : Image Viewer application not running\n"));
        return false;
    }

    return true;
}

/////////////////////////////////////////////////////////////////////////////
//
//  SendImageToViewer
//    Convert the supplied HBITMAP to a DIBSection, and pass it on
//    to the Image Viewer application.
//
//  Parameters :
//    TheBitmap   [in] - The bitmap to send
//    File        [in] - The complete path to the source file that called this function
//    Line        [in] - The line number in the source file from where the call was made
//    Function    [in] - The name of the function from where the call was made
//    Text        [in] - User defined text
//    pRegionRect [in] - For regions only, the region's bounding rectangle
//
//  Returns :
//    ERROR_SUCCESS if the image was sent to the viewer, a Win32 error code otherwise
//
//  Note :
//    The Image Viewer application has to be able to read the data that is sent to it.
//    If you change this function to either add data to, remove data from, or otherwise
//    alter it, then you will also have to change the CImageData class in the Viewer
//    Application project to reflect the changes made here. You may also have to change
//    the CRecieverWindow::OnCopyData() function.
//
/////////////////////////////////////////////////////////////////////////////

LRESULT SendImageToViewer (HBITMAP TheBitmap,
                           LPCWSTR File,
                           UINT Line,
                           LPCWSTR Function,
                           LPCWSTR Text,
                           LPCRECT pRegionRect)
{
    // Check that the windows are properly setup
    if (!AreWindowsReady())
    {
        return ERROR_NOT_READY;
    }

    // Validate the parameters
    if (!TheBitmap)
    {
        ATLTRACE(_T("ImageViewer.dll : SendImageToViewer - A NULL HBITMAP handle was passed in\n"));
        return ERROR_INVALID_HANDLE;
    }

    if (NULL == File)
    {
        File = _T("");
    }

    if (NULL == Function)
    {
        Function = _T("");
    }

    if (NULL == Text)
    {
        Text = _T("");
    }

    // Create the bitmap that gets displayed in the Image Viewer application
    // by making a copy of the supplied bitmap 'TheBitmap'
    pja::CBitmap Image(TheBitmap, true);
    if (NULL == Image)
    {
        ATLTRACE(_T("ImageViewer.dll : SendImageToViewer - Failed to copy the supplied image\n"));
        return ERROR_FUNCTION_FAILED;
    }

    // Create the memory dc needed to extract the bitmap's bits
    pja::CCompatibleDC ImageDC(NULL);
    if (NULL == ImageDC)
    {
        ATLTRACE(_T("ImageViewer.dll : SendImageToViewer - Failed to create the memory device context\n"));
        return ERROR_FUNCTION_FAILED;
    }

    SelectObject(ImageDC, Image);

    // Calculate the size of the memory needed for the data buffer
    size_t ColourTableSize = 0;
    if (Image.BitsPixel() < 16)
    {
        ColourTableSize = sizeof(RGBQUAD) * 256;
    }
    
    // size of the bitmaps bit table
    DWORD BitsSize = ((Image.Width() * Image.BitsPixel() + 31) & (~31)) / 8 * Image.Height();

    DWORD InfoSize = sizeof(BITMAPINFO);

    // Build the information string
    std::wostringstream stream;
    stream << File << std::endl;
    stream << Line << std::endl;
    stream << Function << std::endl;
    stream << CurrentTime() << std::endl;
    stream << Text << std::ends;
    size_t len = wcslen(stream.str().c_str()) + 1;
    
    // Create the data buffer that will be sent via the WM_COPYDATA message
    DWORD DataBufferSize = InfoSize + ColourTableSize + BitsSize + len * sizeof(wchar_t) + sizeof(DWORD) + sizeof(RECT);
    BYTE *DataBuffer = new BYTE[DataBufferSize];
    if (NULL == DataBuffer)
    {
        ATLTRACE(_T("ImageViewer.dll : SendImageToViewer - Failed to allocate memory for the data buffer\n"));
        return ERROR_FUNCTION_FAILED;
    }

    // Assume success for the rest of the operation
    LRESULT ReturnCode = ERROR_SUCCESS;

    // Fill the data buffer with data
    memset(DataBuffer, 0, DataBufferSize);
    BYTE *DataInsertionPointer = DataBuffer;

    // The ID of the current process goes into the data buffer first
    DWORD ProcessID = GetCurrentProcessId();
    *reinterpret_cast<DWORD *>(DataInsertionPointer) = ProcessID;

    // The region rectangle goes in second
    DataInsertionPointer += sizeof(DWORD);

    if (NULL != pRegionRect)
    {
        memcpy_s(DataInsertionPointer, sizeof(RECT), pRegionRect, sizeof(RECT));
    }
    else
    {
        memset(DataInsertionPointer, 0, sizeof(RECT));
    }

    DataInsertionPointer += sizeof(RECT);

    // The descriptive text goes into the data buffer third
    wcscpy_s(reinterpret_cast<wchar_t *>(DataInsertionPointer), len, stream.str().c_str());
    DataInsertionPointer += len * sizeof(wchar_t);

    // The BITMAPINFO structure goes into the DataBuffer fourth
    BITMAPINFO *pBitmapInfo = reinterpret_cast<BITMAPINFO *>(DataInsertionPointer);
    pBitmapInfo->bmiHeader.biSize          = sizeof(BITMAPINFOHEADER);
    pBitmapInfo->bmiHeader.biHeight        = Image.Height();
    pBitmapInfo->bmiHeader.biWidth         = Image.Width();
    pBitmapInfo->bmiHeader.biPlanes        = 1;
    pBitmapInfo->bmiHeader.biBitCount      = Image.BitsPixel();
    pBitmapInfo->bmiHeader.biCompression   = BI_RGB;
    pBitmapInfo->bmiHeader.biSizeImage     = BitsSize;

    DataInsertionPointer += InfoSize;

    if (ColourTableSize)
    {   // If the bitmap has a colour table, it goes into the Databuffer fifth
        GetDIBColorTable(ImageDC, 0, 1 << Image.BitsPixel(), reinterpret_cast<RGBQUAD *>(DataInsertionPointer));
        DataInsertionPointer += ColourTableSize;
    }

    // The bitmap data bits go into the Databuffer last
    if (!GetDIBits(ImageDC, Image, 0, Image.Height(), DataInsertionPointer, pBitmapInfo, DIB_RGB_COLORS))
    {
        ATLTRACE(_T("ImageViewer.dll : SendImageToViewer - GetDIBits() failed to get the bits from the DIB\n"));
        ReturnCode = ERROR_FUNCTION_FAILED;
    }
    else
    {
        // setup the COPYDATASTRUCT
        COPYDATASTRUCT CopyDataStruct;
        CopyDataStruct.dwData = 0xFEB11965;
        CopyDataStruct.cbData = DataBufferSize;
        CopyDataStruct.lpData = DataBuffer;

        if (IsWindow(Window))
        {   // Viewer window active, so send it the bitmap data
            LRESULT retval = SendMessageTimeout(Window,
                                                WM_COPYDATA,
                                                NULL,
                                                reinterpret_cast<LPARAM>(&CopyDataStruct),
                                                SMTO_BLOCK | SMTO_ABORTIFHUNG,
                                                200,
                                                reinterpret_cast<PDWORD_PTR>(&ReturnCode));
            if (!retval)
            {
                DWORD LastError = GetLastError();
                ATLTRACE(_T("ImageViewer.dll : SendImageToViewer - SendMessageTimeout() failed to ")
                                  _T("send the WM_COPYDATA message to the Image Viewer\n"));
                if (0 == LastError || ERROR_TIMEOUT == LastError)
                {
                    ATLTRACE(_T("                                      SendMessageTimeout timed out.\n"));
                    ReturnCode = ERROR_TIMEOUT;
                }
                else
                {
                    ATLTRACE(_T("                                      GetLastError() returned : 0x%08X\n"), LastError);
                    ReturnCode = ERROR_FUNCTION_FAILED;
                }
            }
            else if (ERROR_SUCCESS != ReturnCode)
            {
                ATLTRACE(_T("ImageViewer.dll : SendImageToViewer - Bitmap not accepted by the Image Viewer\n"));
                if (ERROR_INVALID_DATA == ReturnCode)
                {
                    ATLTRACE(_T("                                      CopyDataStruct corrupted\n"));
                    ReturnCode = ERROR_FUNCTION_FAILED;
                }
            }
        }
        else
        {
            ATLTRACE(_T("ImageViewer.dll : SendImageToViewer - Image Viewer not running\n"));
            ReturnCode = ERROR_NOT_READY;
        }
    }

    delete[] DataBuffer;
    return ReturnCode;
}

/////////////////////////////////////////////////////////////////////////////
//
//  CalculateRect
//    This function is used to calculate the rectangle needed to contain
//    a rotated image. Currently it is only called by the FontViewW function
//    when the font is supposed to be drawn at an angle other than 0 degrees.
//
//    This code was adapted from the "Image Rotation in .NET" article written
//    by James T. Johnson at http://www.codeproject.com/csharp/rotateimage.asp
//
/////////////////////////////////////////////////////////////////////////////

void CalculateRect(RECT &rc, long Angle, POINT &Offset)
{
    const double pi = 3.1415926535897932384626433832795; // from windows calculator
    const double pi2 = pi / 2.0;

    double OldWidth = double(rc.right - rc.left);
    double OldHeight = double(rc.bottom - rc.top);

    Angle = Angle % 3600;

    double Theta = (Angle / 10.0) * pi / 180.0;
    double LockedTheta = Theta;

    while (LockedTheta < 0.0)
    {
        LockedTheta += 2 * pi;
    }

    double aTop, oTop;
    double aBottom, oBottom;

    if ((LockedTheta >= 0.0 && LockedTheta <= pi2) ||
        (LockedTheta >= pi && LockedTheta < (pi + pi2)))
    {
        aTop = fabs(cos(LockedTheta)) * OldWidth;
        oTop = fabs(sin(LockedTheta)) * OldWidth;

        aBottom = fabs(cos(LockedTheta)) * OldHeight;
        oBottom = fabs(sin(LockedTheta)) * OldHeight;
    }
    else
    {
        aTop = fabs(sin(LockedTheta)) * OldWidth;
        oTop = fabs(cos(LockedTheta)) * OldWidth;

        aBottom = fabs(sin(LockedTheta)) * OldHeight;
        oBottom = fabs(cos(LockedTheta)) * OldHeight;
    }

    double NewWidth = aTop + oBottom;
    double NewHeight = oTop + aBottom;

    long nWidth = long(ceil(NewWidth));
    long nHeight = long(ceil(NewHeight));

    if (LockedTheta >= 0.0 && LockedTheta <= pi2)
    {
        rc.right = rc.left + nWidth;
        rc.bottom = rc.top + nHeight;
        Offset.y += long(oTop);
    }
    else if (LockedTheta > pi2 && LockedTheta < pi)
    {
        rc.right = rc.left + nHeight;
        rc.bottom = rc.top + nWidth;
        Offset.y += nWidth;
        Offset.x += long(oTop);
    }
    else if (LockedTheta >= pi && LockedTheta < pi + pi2)
    {
        rc.right = rc.left + nWidth;
        rc.bottom = rc.top + nHeight;
        Offset.x += nWidth;
        Offset.y += long(aBottom);
    }
    else
    {
        rc.right = rc.left + nHeight;
        rc.bottom = rc.top + nWidth;
        Offset.x += long(aBottom);
    }
}

#ifdef __cplusplus
extern "C" {
#endif

/////////////////////////////////////////////////////////////////////////////
//
// GetBGBitmap
//   Returns a HBITMAP of the required size, filled with the background colour
//
//  Parameters :
//    Width  [in] - The required width of the bitmap.
//    Height [in] - The required height of the bitmap.
//
//  Returns :
//    An HBITMAP handle to the new bitmap
//
//  Note :
//    Call DeleteObject on the bitmap when you are done with it
//
/////////////////////////////////////////////////////////////////////////////

__declspec(dllexport) HBITMAP GetBGBitmap(UINT Width, UINT Height)
{
    HBITMAP hBitmap = CreateCompatibleBitmap(pja::CScreenDC(), Width, Height);
    pja::CCompatibleDC DC;
    SelectObject(DC, hBitmap);
    RECT rc = {0, 0, Width, Height};
    FillRect(DC, &rc, BGBrush());

    return hBitmap;
}

/////////////////////////////////////////////////////////////////////////////
//
//  BitmapView
//    Calls SendImageToViewer to send the supplied bitmap to the ImageViewer
//
//  Parameters :
//    TheBitmap [in] - The bitmap to send
//    File      [in] - The complete path to the source file that called this function
//    Line      [in] - The line number in the source file from where the call was made
//    Function  [in] - The name of the function from where the call was made
//    Text      [in] - User defined text
//
//  Returns :
//    ERROR_SUCCESS if the image was sent to the viewer, a Win32 error code otherwise
//
//  Note :
//    There are two versions of this function, BitmapViewA is the ANSI version
//    and BitmapViewW is the UNICODE version
//
/////////////////////////////////////////////////////////////////////////////

__declspec(dllexport) LRESULT BitmapViewW (HBITMAP TheBitmap,
                                           LPCWSTR File,
                                           UINT Line,
                                           LPCWSTR Function,
                                           LPCWSTR Text)
{
    return SendImageToViewer(TheBitmap, File, Line, Function, Text, NULL);
}

__declspec(dllexport) LRESULT BitmapViewA (HBITMAP TheBitmap,
                                           LPCSTR File,
                                           UINT Line,
                                           LPCSTR Function,
                                           LPCSTR Text)
{
    return SendImageToViewer(TheBitmap, CA2W(File), Line, CA2W(Function), CA2W(Text), NULL);
}

/////////////////////////////////////////////////////////////////////////////
//
//  IconView
//    Convert the supplied HICON to a HBITMAP with the AND mask on the right,
//    the XOR mask in the middle, and the icon on the left. Then pass the newly
//    created HBITMAP onto the BitmapView function which sends it to the ViewerApp.
//
//  Parameters :
//    TheIcon  [in] - The icon to send
//    File     [in] - The complete path to the source file that called this function
//    Line     [in] - The line number in the source file from where the call was made
//    Function [in] - The name of the function from where the call was made
//    Text     [in] - User defined text
//
//  Returns :
//    ERROR_SUCCESS if the icon was sent to the viewer, a Win32 error code otherwise
//
//  Note :
//    There are two versions of this function, IconViewA is the ANSI version
//    and IconViewW is the UNICODE version
//
/////////////////////////////////////////////////////////////////////////////

__declspec(dllexport) LRESULT IconViewW (HICON TheIcon,
                                         LPCWSTR File,
                                         UINT Line,
                                         LPCWSTR Function,
                                         LPCWSTR Text)
{
    // Check that the windows are properly setup
    if (!AreWindowsReady())
    {
        return ERROR_NOT_READY;
    }

    // Validate the parameters
    if (!TheIcon)
    {
        ATLTRACE(_T("ImageViewer.dll : IconViewW - A NULL HICON handle was passed in\n"));
        return ERROR_INVALID_HANDLE;
    }

    ICONINFO info = {0};
    if (!GetIconInfo(TheIcon, &info))
    {
        ATLTRACE(_T("ImageViewer.dll : IconViewW - GetIconInfo() failed to get information from supplied HICON\n"));
        return GetLastError();
    }

    pja::CBitmap Colour(info.hbmColor);
    pja::CBitmap Mask(info.hbmMask);
    if (NULL == Mask)
    {
        ATLTRACE(_T("ImageViewer.dll : IconViewW - Failed to initialize mask\n"));
        return ERROR_FUNCTION_FAILED;
    }

    pja::CCompatibleDC MaskAndColourDC(NULL);
    if (NULL == MaskAndColourDC)
    {
        ATLTRACE(_T("ImageViewer.dll : IconViewW - Failed to initialize first memory dc\n"));
        return ERROR_FUNCTION_FAILED;
    }

    int width = 0;
    int height = 0;

    if (NULL != Colour)
    {
        SelectObject(MaskAndColourDC, Colour);
        height = Colour.Height();
        width = Colour.Width() * 3;
    }
    else
    {
        SelectObject(MaskAndColourDC, Mask);
        height = Mask.Height() / 2;
        width = Mask.Width() * 3;
    }

    // Create the bitmap that gets displayed in the Image Viewer application
    pja::CBitmap Image(MaskAndColourDC, width, height);
    if (NULL == Image)
    {
        ATLTRACE(_T("ImageViewer.dll : IconViewW - Failed to create bitmap\n"));
        return ERROR_FUNCTION_FAILED;
    }

    // Create the memory device context used to draw onto the bitmap
    pja::CCompatibleDC ImageDC(NULL);
    if (NULL == ImageDC)
    {
        ATLTRACE(_T("ImageViewer.dll : IconViewW - Failed to create memory dc\n"));
        return ERROR_FUNCTION_FAILED;
    }

    SelectObject(ImageDC, Image);

    // Get the bitmap ready for display
    RECT rc = {0, 0, width, height};
    FillRect(ImageDC, &rc, NULL != Colour ? BGBrush() : (HBRUSH)GetStockObject(WHITE_BRUSH));

    SelectObject(MaskAndColourDC, Mask);
    BitBlt(ImageDC, 0, 0, Mask.Width(), height, MaskAndColourDC, 0, 0, SRCCOPY);

    if (NULL == Colour)
    {
        BitBlt(ImageDC, Mask.Width(), 0, Mask.Width(), height, MaskAndColourDC, 0, height, SRCCOPY);
    }
    else
    {
        SelectObject(MaskAndColourDC, Colour);
        BitBlt(ImageDC, Mask.Width(), 0, Mask.Width(), Mask.Height(), MaskAndColourDC, 0, 0, SRCCOPY);
    }

    DrawIconEx(ImageDC, Mask.Width() * 2, 0, TheIcon, 0, 0, 0, 0, DI_NORMAL);

    // Call BitmapViewW to send the bitmap to the Image Viewer
    return BitmapViewW(Image, File, Line, Function, Text);
}

__declspec(dllexport) LRESULT IconViewA (HICON TheIcon,
                                         LPCSTR File,
                                         UINT Line,
                                         LPCSTR Function,
                                         LPCSTR Text)
{
    return IconViewW(TheIcon, CA2W(File), Line, CA2W(Function), CA2W(Text));
}

/////////////////////////////////////////////////////////////////////////////
//
//  ImageListView
//    This function was written by Jörgen Sigvardsson <jorgen@profitab.com>
//    and provided here on August 30, 2004.
//
//    Convert the supplied HIMAGELIST to a HBITMAP with three rows of images.
//    The top row will be the images drawn normally, the middle row will be
//    the images drawn transparently, and the bottom row will be the images
//    drawn with the supplied custom flag.
//
//  Parameters :
//    hImageList   [in] - The HIMAGELIST to send
//    iImg         [in] - The index of the image in the image list, use -1 to
//                          draw the entire image list
//    nCustomFlags [in] - The custom drawing flag for the bottom row
//    File         [in] - The complete path to the source file that called this function
//    Line         [in] - The line number in the source file from where the call was made
//    Function     [in] - The name of the function from where the call was made
//    Text         [in] - User defined text
//
//  Returns :
//    ERROR_SUCCESS if the image list was sent to the viewer, a Win32 error code otherwise
//
//  Note :
//    There are two versions of this function, ImageListViewA is the ANSI version
//    and ImageListViewW is the UNICODE version
//
/////////////////////////////////////////////////////////////////////////////

// Need this for ImageList_* functions
#pragma comment(lib, "comctl32.lib")

__declspec(dllexport) LRESULT ImageListViewW(HIMAGELIST hImageList,
                                             int iImg,
                                             UINT nCustomFlags,
                                             LPCWSTR File,
                                             UINT Line,
                                             LPCWSTR Function,
                                             LPCWSTR Text)
{
    // Check that the windows are properly setup
    if (!AreWindowsReady())
    {
        return ERROR_NOT_READY;
    }

    // Validate the parameters
    if (!hImageList)
    {
        ATLTRACE(_T("ImageViewer.dll : ImageListViewW - A NULL HIMAGELIST was passed in\n"));
        return ERROR_INVALID_HANDLE;
    }

    IMAGEINFO info;
    if (!ImageList_GetImageInfo(hImageList, 0, &info))
    {
        ATLTRACE(_T("ImageViewer.dll : ImageListViewW - ImageList_GetImageInfo returned an error\n"));
        return ERROR_FUNCTION_FAILED;
    }

    int nImageCount = ImageList_GetImageCount(hImageList);
    if (iImg != -1 && iImg < 0 && iImg >= nImageCount)
    {
        ATLTRACE(_T("ImageViewer.dll : ImageListViewW - Recieved an invalid image index\n"));
        return ERROR_FUNCTION_FAILED;
    }

    if (iImg != -1) // If image index is not -1, we're just drawing one image
        nImageCount = 1;

    int nImageWidth, nImageHeight;
    if (!ImageList_GetIconSize(hImageList, &nImageWidth, &nImageHeight))
    {
        ATLTRACE(_T("ImageViewer.dll : ImageListViewW - ImageList_GetIconSize returned zero\n"));
        return ERROR_FUNCTION_FAILED;
    }

    // Create the bitmap that gets displayed in the Image Viewer application
    pja::CBitmap Image(NULL, nImageWidth * nImageCount, nImageHeight * 3);
    if (NULL == Image)
    {
        ATLTRACE(_T("ImageViewer.dll : ImageListViewW - Failed to create bitmap\n"));
        return ERROR_FUNCTION_FAILED;
    }

    // Create the memory device context used to draw onto the bitmap
    pja::CCompatibleDC ImageDC(NULL);
    if (NULL == ImageDC)
    {
        ATLTRACE(_T("ImageViewer.dll : ImageListViewW - Failed to create memory dc\n"));
        return ERROR_FUNCTION_FAILED;
    }

    SelectObject(ImageDC, Image);

    // Get the bitmap ready for display
    RECT rcFill = { 0, 0, nImageCount * nImageWidth, nImageHeight * 3 };
    FillRect(ImageDC, &rcFill, BGBrush());

    int i = 0;
    // first row is ILD_NORMAL
    for (i = 0; i < nImageCount; ++i)
        ImageList_Draw(hImageList, iImg == -1 ? i : iImg, ImageDC, i * nImageWidth, 0, ILD_NORMAL);

    // second row is ILD_TRANSPARENT
    for (i = 0; i < nImageCount; ++i)
        ImageList_Draw(hImageList, iImg == -1 ? i : iImg, ImageDC, i * nImageWidth, nImageHeight, ILD_TRANSPARENT);

    // third row is whatever custom flags say
    for (i = 0; i < nImageCount; ++i)
        ImageList_Draw(hImageList, iImg == -1 ? i : iImg, ImageDC, i * nImageWidth, 2 * nImageHeight, nCustomFlags);

    // Call BitmapViewW to send the bitmap to the Image Viewer
    return BitmapViewW(Image, File, Line, Function, Text);
}

__declspec(dllexport) LRESULT ImageListViewA(HIMAGELIST hImageList,
                                             int iImg,
                                             UINT nCustomFlags,
                                             LPCSTR File,
                                             UINT Line,
                                             LPCSTR Function,
                                             LPCSTR Text)
{
    return ImageListViewW(hImageList, iImg, nCustomFlags, CA2W(File), Line, CA2W(Function), CA2W(Text));
}

/////////////////////////////////////////////////////////////////////////////
//
//  RegionView
//    This function was originally written by waldermort <waldermort@hotmail.com>
//    and provided here on November 7, 2006.
//
//    Draw the supplied HRGN onto the supplied bitmap by inverting the colours
//    on the bitmap that correspond with the region. If the supplied bitmap is
//    NULL then a default bitmap is generated for the region.
//
//  Parameters :
//    TheRegion     [in] - The region to display
//    TheBackground [in] - The bitmap to display the region on
//    File          [in] - The complete path to the source file that called this function
//    Line          [in] - The line number in the source file from where the call was made
//    Function      [in] - The name of the function from where the call was made
//    Text          [in] - User defined text
//
//  Returns :
//    ERROR_SUCCESS if the region was sent to the viewer, a Win32 error code otherwise
//
//  Note :
//    There are two versions of this function, RegionViewA is the ANSI version
//    and RegionViewW is the UNICODE version
//
/////////////////////////////////////////////////////////////////////////////

__declspec(dllexport) LRESULT RegionViewW (HRGN TheRegion,
                                           HBITMAP TheBackground,
                                           LPCWSTR File,
                                           UINT Line,
                                           LPCWSTR Function,
                                           LPCWSTR Text)
{
    // Check that the windows are properly setup
    if (!AreWindowsReady())
    {
        return ERROR_NOT_READY;
    }

    // Validate the parameters
    if (!TheRegion)
    {
        ATLTRACE(_T("ImageViewer.dll : RegionViewW - A NULL HRGN handle was passed in\n"));
        return ERROR_INVALID_HANDLE;
    }

    RECT RegionRect = {0};
    if (GetRgnBox(TheRegion, &RegionRect) <= NULLREGION)
    {
        ATLTRACE(_T("ImageViewer.dll : RegionViewW - The Region is empty\n"));
        return ERROR_INVALID_DATA;
    }

    HRGN TempRegion = CreateRectRgn(0, 0, 1, 1);
    if (ERROR == CombineRgn(TempRegion, TheRegion, NULL, RGN_COPY))
    {
        ATLTRACE(_T("ImageViewer.dll : RegionViewW - Failed to copy the supplied region\n"));
        return ERROR_FUNCTION_FAILED;
    }

    RECT ImageRect = RegionRect;

    // Create the bitmap that gets displayed in the Image Viewer application
    pja::CBitmap Image(TheBackground, true);

    if (NULL != Image)
    {
        RECT Union = {0};
        if (UnionRect(&Union, &RegionRect, Image.Rect()) && !EqualRect(&Union, Image.Rect()))
        {
            pja::CBitmap BiggerBitmap(NULL, Union.right - Union.left, Union.bottom - Union.top);
            if (NULL != BiggerBitmap)
            {
                pja::CCompatibleDC BiggerDC(NULL);
                SelectObject(BiggerDC, BiggerBitmap);
                FillRect(BiggerDC, BiggerBitmap.Rect(), BGBrush());

                pja::CCompatibleDC BitmapDC(NULL);
                SelectObject(BitmapDC, Image);
                BitBlt(BiggerDC,
                       -Union.left,
                       -Union.top,
                       Image.Width(),
                       Image.Height(),
                       BitmapDC,
                       0, 0,
                       SRCCOPY);

                OffsetRgn(TempRegion, -Union.left, -Union.top);
            }
            Image = BiggerBitmap;
        }
    }

    if (NULL == Image)
    {
        OffsetRgn(TempRegion, -ImageRect.left, -ImageRect.top);
        OffsetRect(&ImageRect, -ImageRect.left, -ImageRect.top);
        Image = pja::CBitmap(NULL, ImageRect.right, ImageRect.bottom);
    }

    if (NULL == Image)
    {
        ATLTRACE(_T("ImageViewer.dll : RegionViewW - Failed to create bitmap\n"));
        return ERROR_FUNCTION_FAILED;
    }

    // Create the memory device context used to draw onto the bitmap
    pja::CCompatibleDC ImageDC(NULL);
    if (NULL == ImageDC)
    {
        ATLTRACE(_T("ImageViewer.dll : RegionViewW - Failed to create memory dc\n"));
        return ERROR_FUNCTION_FAILED;
    }

    SelectObject(ImageDC, Image);

    // Get the bitmap ready for display
    if (NULL == TheBackground)
    {
        FillRect(ImageDC, &ImageRect, BGBrush());
    }

    InvertRgn(ImageDC, TempRegion);
    HBRUSH FrameBrush = CreateSolidBrush(BGBrush());
    FrameRgn(ImageDC, TempRegion, FrameBrush, 1, 1);
    DeleteObject(FrameBrush);

    // Call SendImageToViewer to send the bitmap to the Image Viewer
    return SendImageToViewer(Image, File, Line, Function, Text, &RegionRect);
}

__declspec(dllexport) LRESULT RegionViewA (HRGN TheRegion,
                                           HBITMAP TheBackground,
                                           LPCSTR File,
                                           UINT Line,
                                           LPCSTR Function,
                                           LPCSTR Text)
{
    return RegionViewW(TheRegion, TheBackground, CA2W(File), Line, CA2W(Function), CA2W(Text));
}

/////////////////////////////////////////////////////////////////////////////
//
//  FontView
//    Draw the supplied font 
//
//  Parameters :
//    TheFont  [in] - The font to send
//    Sample   [in] - The sample text to be used to display the font
//    File     [in] - The complete path to the source file that called this function
//    Line     [in] - The line number in the source file from where the call was made
//    Function [in] - The name of the function from where the call was made
//    Text     [in] - User defined text
//
//  Returns :
//    ERROR_SUCCESS if the font was sent to the viewer, a Win32 error code otherwise
//
//  Note :
//    There are two versions of this function, FontViewA is the ANSI version
//    and FontViewW is the UNICODE version
//
//    This function works best when the font's orientation and escapement are both set
//    to zero. When the orientation and escapement are the same but not zero then there
//    is the possiblity of some clipping occurring. When the orientation and escapement
//    are different then all bets are off.
//
/////////////////////////////////////////////////////////////////////////////

__declspec(dllexport) LRESULT FontViewW(HFONT TheFont,
                                        LPCWSTR Sample,
                                        LPCWSTR File,
                                        UINT Line,
                                        LPCWSTR Function,
                                        LPCWSTR Text)
{
    // Check that the windows are properly setup
    if (!AreWindowsReady())
    {
        return ERROR_NOT_READY;
    }

    // Validate the parameters
    if (!TheFont)
    {
        ATLTRACE(_T("ImageViewer.dll : FontViewW - A NULL HFONT handle was passed in\n"));
        return ERROR_INVALID_HANDLE;
    }

    LOGFONT LogFont = {0};
    if (0 == GetObject(TheFont, sizeof(LOGFONT), &LogFont))
    {
        ATLTRACE(_T("ImageViewer.dll : FontViewW - Failed to get the LOGFONT structure from the HFONT handle\n"));
        return ERROR_FUNCTION_FAILED;
    }

    // Add the values of the LOGFONT structure onto the end of the Text string parameter
    typedef std::map<BYTE, LPCTSTR> LogFontData;

    LogFontData CharSets;
    CharSets[ANSI_CHARSET]          = _T("ANSI_CHARSET");
    CharSets[BALTIC_CHARSET]        = _T("BALTIC_CHARSET");
    CharSets[CHINESEBIG5_CHARSET]   = _T("CHINESEBIG5_CHARSET");
    CharSets[DEFAULT_CHARSET]       = _T("DEFAULT_CHARSET");
    CharSets[EASTEUROPE_CHARSET]    = _T("EASTEUROPE_CHARSET");
    CharSets[GB2312_CHARSET]        = _T("GB2312_CHARSET");
    CharSets[GREEK_CHARSET]         = _T("GREEK_CHARSET");
    CharSets[HANGUL_CHARSET]        = _T("HANGUL_CHARSET");
    CharSets[MAC_CHARSET]           = _T("MAC_CHARSET");
    CharSets[OEM_CHARSET]           = _T("OEM_CHARSET");
    CharSets[RUSSIAN_CHARSET]       = _T("RUSSIAN_CHARSET");
    CharSets[SHIFTJIS_CHARSET]      = _T("SHIFTJIS_CHARSET");
    CharSets[SYMBOL_CHARSET]        = _T("SYMBOL_CHARSET");
    CharSets[TURKISH_CHARSET]       = _T("TURKISH_CHARSET");
    CharSets[VIETNAMESE_CHARSET]    = _T("VIETNAMESE_CHARSET");
    CharSets[JOHAB_CHARSET]         = _T("JOHAB_CHARSET");
    CharSets[ARABIC_CHARSET]        = _T("ARABIC_CHARSET");
    CharSets[HEBREW_CHARSET]        = _T("HEBREW_CHARSET");
    CharSets[THAI_CHARSET]          = _T("THAI_CHARSET");

    LogFontData OutPrecisions;
    OutPrecisions[OUT_CHARACTER_PRECIS] = _T("OUT_CHARACTER_PRECIS");
    OutPrecisions[OUT_DEFAULT_PRECIS]   = _T("OUT_DEFAULT_PRECIS");
    OutPrecisions[OUT_DEVICE_PRECIS]    = _T("OUT_DEVICE_PRECIS");
    OutPrecisions[OUT_OUTLINE_PRECIS]   = _T("OUT_OUTLINE_PRECIS");
    OutPrecisions[OUT_PS_ONLY_PRECIS]   = _T("OUT_PS_ONLY_PRECIS");
    OutPrecisions[OUT_RASTER_PRECIS]    = _T("OUT_RASTER_PRECIS");
    OutPrecisions[OUT_STRING_PRECIS]    = _T("OUT_STRING_PRECIS");
    OutPrecisions[OUT_STROKE_PRECIS]    = _T("OUT_STROKE_PRECIS");
    OutPrecisions[OUT_TT_ONLY_PRECIS]   = _T("OUT_TT_ONLY_PRECIS");
    OutPrecisions[OUT_TT_PRECIS]        = _T("OUT_TT_PRECIS");

    LogFontData ClipPrecisions;
    ClipPrecisions[CLIP_CHARACTER_PRECIS]   = _T("CLIP_CHARACTER_PRECIS");
    ClipPrecisions[CLIP_DEFAULT_PRECIS]     = _T("CLIP_DEFAULT_PRECIS");
    //ClipPrecisions[CLIP_DFA_DISABLE]        = _T("CLIP_DFA_DISABLE");
    ClipPrecisions[CLIP_EMBEDDED]           = _T("CLIP_EMBEDDED");
    ClipPrecisions[CLIP_LH_ANGLES]          = _T("CLIP_LH_ANGLES");
    ClipPrecisions[CLIP_MASK]               = _T("CLIP_MASK");
    ClipPrecisions[CLIP_STROKE_PRECIS]      = _T("CLIP_STROKE_PRECIS");
    ClipPrecisions[CLIP_TT_ALWAYS]          = _T("CLIP_TT_ALWAYS");

    LogFontData Qualities;
    Qualities[ANTIALIASED_QUALITY]          = _T("ANTIALIASED_QUALITY");
    Qualities[CLEARTYPE_QUALITY]            = _T("CLEARTYPE_QUALITY");
    Qualities[CLEARTYPE_NATURAL_QUALITY]    = _T("CLEARTYPE_NATURAL_QUALITY");
    Qualities[DEFAULT_QUALITY]              = _T("DEFAULT_QUALITY");
    Qualities[DRAFT_QUALITY]                = _T("DRAFT_QUALITY");
    Qualities[NONANTIALIASED_QUALITY]       = _T("NONANTIALIASED_QUALITY");
    Qualities[PROOF_QUALITY]                = _T("PROOF_QUALITY");

    LogFontData Pitches;
    Pitches[DEFAULT_PITCH]  = _T("DEFAULT_PITCH");
    Pitches[FIXED_PITCH]    = _T("FIXED_PITCH");
    Pitches[VARIABLE_PITCH] = _T("VARIABLE_PITCH");

    LogFontData Families;
    Families[FF_DECORATIVE] = _T("FF_DECORATIVE");
    Families[FF_DONTCARE]   = _T("FF_DONTCARE");
    Families[FF_MODERN]     = _T("FF_MODERN");
    Families[FF_ROMAN]      = _T("FF_ROMAN");
    Families[FF_SCRIPT]     = _T("FF_SCRIPT");
    Families[FF_SWISS]      = _T("FF_SWISS");

    std::tstring TextString;
    if (NULL != Text && 0 != wcslen(Text))
    {
        TextString = CW2T(Text);
        TextString += _T("/n/n");
    }

    pja::CFormat cf;
    TextString += cf.MakeMessage(DllModule, IDS_LOGFONT1, 8,
                                 cf.TS(LogFont.lfHeight),
                                 cf.TS(LogFont.lfWidth),
                                 cf.TS(LogFont.lfEscapement),
                                 cf.TS(LogFont.lfOrientation),
                                 cf.TS(LogFont.lfWeight),
                                 cf.TS(LogFont.lfItalic ? _T("TRUE") : _T("FALSE")),
                                 cf.TS(LogFont.lfUnderline ? _T("TRUE") : _T("FALSE")),
                                 cf.TS(LogFont.lfStrikeOut ? _T("TRUE") : _T("FALSE")));

    TextString += cf.MakeMessage(DllModule, IDS_LOGFONT2, 7,
                                 cf.TS((*CharSets.find(LogFont.lfCharSet)).second),
                                 cf.TS((*OutPrecisions.find(LogFont.lfOutPrecision)).second),
                                 cf.TS((*ClipPrecisions.find(LogFont.lfClipPrecision)).second),
                                 cf.TS((*Qualities.find(LogFont.lfQuality)).second),
                                 cf.TS((*Pitches.find(LogFont.lfPitchAndFamily & 0x03)).second),
                                 cf.TS((*Families.find(LogFont.lfPitchAndFamily & 0xF0)).second),
                                 cf.TS(LogFont.lfFaceName));

    // Set the text used to display the font on the sample image
    std::tstring ImageText = _T("The quick brown fox jumps over the lazy dog. 1234567890");

    if (NULL != Sample && 0 != _tcslen(Sample))
    {
        std::tstring SampleText = Sample;
        std::tstring::size_type pos = SampleText.find_first_of(_T("\r\n"));
        if (pos != std::tstring::npos)
        {
            SampleText = SampleText.substr(0, pos);
        }

        if (SampleText.length() > 0)
        {
            ImageText = SampleText;
        }
    }

    // Create the bitmap that gets displayed in the Image Viewer application
    pja::CBitmap ImageBitmap;

    // Create the memory device context used to draw onto the bitmap
    pja::CCompatibleDC ImageDC(NULL);
    if (NULL == ImageDC)
    {
        ATLTRACE(_T("ImageViewer.dll : FontViewW - Failed to create memory dc\n"));
        return ERROR_FUNCTION_FAILED;
    }

    // Get the bitmap ready for display
    SelectObject(ImageDC, TheFont);

    RECT ImageRect = {0};

    // get the required height of the bitmap
    TEXTMETRIC tm = {0};
    GetTextMetrics(ImageDC, &tm);
    ImageRect.bottom = tm.tmHeight;

    // Calculate the required width of the bitmap. Some characters in some fonts
    // have overhangs and/or underhangs so large that the second or even third
    // character from either end of the text can affect the size of the bitmap
    // needed to display the text.
    ABCFLOAT ABCWidths = {0};
    long left = 0;
    long right = 0;
    long pos = 0;

    for (std::tstring::iterator it = ImageText.begin(); it != ImageText.end(); ++it)
    {
        if (GetCharABCWidthsFloat(ImageDC, *(it), *(it), &ABCWidths))
        {
            pos += long(ABCWidths.abcfA);
            left = min(left, pos);
            pos += long(ABCWidths.abcfB);
            right = max(right, pos);
            pos += long(ABCWidths.abcfC);
        }
    }

    ImageRect.right = right - left;

    POINT Offset = {0};
    if (LogFont.lfEscapement % 3600 != 0)
    {
        CalculateRect(ImageRect, LogFont.lfEscapement, Offset);
    }

    // create the bitmap
    ImageBitmap = pja::CBitmap(NULL, ImageRect.right, ImageRect.bottom);
    if (NULL == ImageBitmap)
    {
        ATLTRACE(_T("ImageViewer.dll : FontViewW - Failed to create bitmap\n"));
        return ERROR_FUNCTION_FAILED;
    }

    // fill the backgound colour and draw the text on the bitmap
    SelectObject(ImageDC, ImageBitmap);

    BGBrush Background;
    FillRect(ImageDC, &ImageRect, Background);
    SetTextColor(ImageDC, Background);
    SetBkMode(ImageDC, TRANSPARENT);
    if (LogFont.lfEscapement % 3600 == 0)
    {
        // do not clip any underhang
        ImageRect.left -= left;
    }
    else
    {
        // adjust the starting point of the text so the text is visible
        ImageRect.left += Offset.x;
        ImageRect.top += Offset.y;
    }

    if (LogFont.lfEscapement != LogFont.lfOrientation)
    {
        SetGraphicsMode(ImageDC, GM_ADVANCED);
    }

    DrawText(ImageDC, ImageText.c_str(), -1, &ImageRect, DT_SINGLELINE | DT_NOPREFIX | DT_NOCLIP);

    // Call BitmapViewW to send the bitmap to the Image Viewer
    return BitmapViewW(ImageBitmap, File, Line, Function, CT2W(TextString.c_str()));
}

__declspec(dllexport) LRESULT FontViewA (HFONT TheFont,
                                         LPCSTR Sample,
                                         LPCSTR File,
                                         UINT Line,
                                         LPCSTR Function,
                                         LPCSTR Text)
{
    return FontViewW(TheFont, CA2W(Sample), CA2W(File), Line, CA2W(Function), CA2W(Text));
}

#ifdef __cplusplus
}
#endif

/////////////////////////////////////////////////////////////////////////////
//
//  END OF VIEWERLIB.CPP
//
/////////////////////////////////////////////////////////////////////////////



/****************************************************************************

// This is a basic function template for adding a new "View" function to the
// library. Simply change the "<function name>", "<class name>", and "<object name>"
// texts in the template code below to something meaningful, and add the code
// needed where ever there is a line that starts with "// TODO:"

__declspec(dllexport) LRESULT <function name>ViewW(<class name> <object name>,
                                                   LPCWSTR File,
                                                   UINT Line,
                                                   LPCWSTR Function,
                                                   LPCWSTR Text)
{
    // Check that the windows are properly setup
    if (!AreWindowsReady())
    {
        return ERROR_NOT_READY;
    }

    // Validate the parameters

    // TODO: Add code here to validate the parameters passed into this function

    // Create the bitmap that gets displayed in the Image Viewer application
    pja::CBitmap Image;

    // TODO: Add code to initialize Image. May include changing the c'tor line above

    if (NULL == Image)
    {
        ATLTRACE(_T("ImageViewer.dll : <function name>ViewW - Failed to create bitmap\n"));
        return ERROR_FUNCTION_FAILED;
    }

    // Create the memory device context used to draw onto the bitmap
    pja::CCompatibleDC ImageDC(NULL);
    if (NULL == ImageDC)
    {
        ATLTRACE(_T("ImageViewer.dll : <function name>ViewW - Failed to create memory dc\n"));
        return ERROR_FUNCTION_FAILED;
    }

    SelectObject(ImageDC, Image); 

    // Get the bitmap ready for display

    // TODO: Add drawing code here that displays the object the way you want. Use ImageDC as the device context.

    // Call BitmapViewW to send the bitmap to the Image Viewer
    return BitmapViewW(Image, File, Line, Function, Text);
}

__declspec(dllexport) LRESULT <function name>ViewA (<class name> <object name>,
                                                    LPCSTR File,
                                                    UINT Line,
                                                    LPCSTR Function,
                                                    LPCSTR Text)
{
    return <function name>ViewW(<object name>, CA2W(File), Line, CA2W(Function), CA2W(Text));
}

****************************************************************************/

