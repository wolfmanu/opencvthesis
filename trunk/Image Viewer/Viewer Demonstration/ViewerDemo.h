// Viewer Demonstration.h : main header file for the Viewer Demonstration application
//
#pragma once

#ifndef __AFXWIN_H__
    #error "include 'stdafx.h' before including this file for PCH"
#endif

#include "resource.h"       // main symbols


// CViewerDemoApp:
// See Viewer Demonstration.cpp for the implementation of this class
//

class CViewerDemoApp : public CWinApp
{
public:
    CViewerDemoApp();


// Overrides
public:
    virtual BOOL InitInstance();

// Implementation
    afx_msg void OnAppAbout();
    DECLARE_MESSAGE_MAP()
};

extern CViewerDemoApp theApp;