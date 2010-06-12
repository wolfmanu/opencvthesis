/****************************************************************************
IVDllWindow.h : Header file for the Image Viewer DLL Window class.
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

#pragma once

#include <windows.h>

class CIVDllWindow
{
private:
    // DllWindow: The handle of a hidden window that is created by the ImageViewer dll
    //            and attached to a process that is using the image viewer utility.
    //            It's job is to recieve the Viewer_app_window registered message from
    //            the image viewer utility when the utility starts up or shuts down
    //            and then set the ExeWindow handle accordingly.
    HWND DllWindow;

    // ExeWindow: This is the handle of the image viewer utility's hidden image reciever
    //            window. The ImageViewer dll sends a WM_COPYDATA message to this window
    //            whenever it has to send an image to the image viewer utility.
    HWND ExeWindow;

    // static pointer used in static CALLBACK functions
    static CIVDllWindow *This;

protected:
    bool RegisterWindowClass();
    static LRESULT CALLBACK DllWindowProc(HWND, UINT, WPARAM, LPARAM);
    static BOOL CALLBACK Searcher(HWND, LPARAM);

public:
    CIVDllWindow();
    ~CIVDllWindow();

    BOOL Create();

    operator HWND();
};
