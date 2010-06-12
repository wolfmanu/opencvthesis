/****************************************************************************
MainStatusBar.h : Header file for the CMainStatusBar class
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

// CMainStatusBar

class CMainStatusBar : public CStatusBar
{
    DECLARE_DYNAMIC(CMainStatusBar)

public:
    CMainStatusBar();
    virtual ~CMainStatusBar();
    BOOL Create(CWnd *pParent);
    virtual void DrawItem(LPDRAWITEMSTRUCT lpDrawItemStruct);

protected:
    CFont Font;
    void UpdateStatusBarPanes();

    afx_msg LRESULT OnThemeChanged(WPARAM, LPARAM);
    DECLARE_MESSAGE_MAP()
};


