/****************************************************************************
IVDllWindow.cpp : Implementation file for the Image Viewer DLL Window class.
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

#include "IVDllWindow.h"
#include <tchar.h>
#include <crtdbg.h>

#define DLLWINDOWCLASS _T("PJA_IMAGE_VIEWER_DLL_HIDDEN_WINDOW")

// These same messages are also registered in the file Reciever.cpp, part of the Viewer Application project.
const UINT Looking_for_viewer_app = RegisterWindowMessage(_T("LOOKING_FOR_VIEWER_APP_{FD4EA0C0-ECFC-11d5-B625-ED551C514422}"));
const UINT Viewer_app_window = RegisterWindowMessage(_T("VIEWER_APP_WINDOW_{FD4EA0C0-ECFC-11d5-B625-ED551C514422}"));

CIVDllWindow *CIVDllWindow::This = NULL;

CIVDllWindow::CIVDllWindow()
: DllWindow(NULL)
, ExeWindow(NULL)
{
    // One instance only
    _ASSERT(NULL == This);
    This = this;
}

CIVDllWindow::~CIVDllWindow()
{
    if (IsWindow(DllWindow))
    {
        DestroyWindow(DllWindow);
    }

    This = NULL;
}

BOOL CIVDllWindow::Create()
{
    if (!IsWindow(DllWindow) && RegisterWindowClass())
    {
        DllWindow = CreateWindow(DLLWINDOWCLASS,
                                 _T("Image Viewer Dll hidden window"),
                                 WS_POPUP,
                                 0, 0, 0, 0,
                                 NULL,
                                 NULL,
                                 GetModuleHandle(_T("ImageViewer.dll")),
                                 NULL);
        if (NULL != DllWindow)
        {
            // Find the image viewer app's image reciever window
            EnumWindows(Searcher, reinterpret_cast<LPARAM>(&ExeWindow));
        }
    }

    return IsWindow(DllWindow);
}

bool CIVDllWindow::RegisterWindowClass()
{
    WNDCLASS WndClass = {0};

    WndClass.hInstance = GetModuleHandle(_T("ImageViewer.dll"));
    WndClass.lpfnWndProc = DllWindowProc;
    WndClass.lpszClassName = DLLWINDOWCLASS;

    return RegisterClass(&WndClass) != 0;
}

CIVDllWindow::operator HWND()
{
    // return the Image Viewer app's image reciever window handle
    return ExeWindow;
}

BOOL CALLBACK CIVDllWindow::Searcher(HWND Window, LPARAM lp)
{
    // is 'Window' the one we are looking for?
    DWORD result;
    LRESULT ok = SendMessageTimeout(Window,
                                    Looking_for_viewer_app,
                                    0xFEB11965,
                                    0,
                                    SMTO_BLOCK | SMTO_ABORTIFHUNG,
                                    200,
                                    &result);

    if (ok == 0)
    {
        // error - continue search
        return TRUE;
    }

    if (result == Looking_for_viewer_app)
    {
        // yes, found it
        *(HWND*)lp = Window;
        return FALSE;
    }

    return TRUE;
}

LRESULT CALLBACK CIVDllWindow::DllWindowProc(HWND hWnd, UINT Msg, WPARAM wp, LPARAM lp)
{
    if (NULL != This && Viewer_app_window == Msg && 0xFEB11965 == wp)
    {
        // The image viewer app broadcasts the Viewer_app_window registered message
        // when it starts and when it ends. When the image viewer app starts the LPARAM
        // parameter is the handle of the image receiver window, and when it ends the
        // LPARAM parameter is NULL ( see the file Reciever.cpp for the functions
        // CReceiverThread::InitInstance() and CRecieverThread::ExitInstance() ).
        This->ExeWindow = reinterpret_cast<HWND>(lp);
    }

    return DefWindowProc(hWnd, Msg, wp, lp);
}
