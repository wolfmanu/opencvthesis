// ViewerDemoView.cpp : implementation of the CViewerDemoView class
//

#include "stdafx.h"
#include "ViewerDemo.h"

#include "ViewerDemoDoc.h"
#include "ViewerDemoView.h"

#include <shlwapi.h>

#include "..\common files\pja_dc.h"

#define ACTIVATE_VIEWER
#include "ImageViewer.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CViewerDemoView

IMPLEMENT_DYNCREATE(CViewerDemoView, CView)

BEGIN_MESSAGE_MAP(CViewerDemoView, CView)
END_MESSAGE_MAP()

// CViewerDemoView construction/destruction

CViewerDemoView::CViewerDemoView()
{
    // TODO: add construction code here

}

CViewerDemoView::~CViewerDemoView()
{
}

BOOL CViewerDemoView::PreCreateWindow(CREATESTRUCT& cs)
{
    // TODO: Modify the Window class or styles here by modifying
    //  the CREATESTRUCT cs

    return CView::PreCreateWindow(cs);
}

// CViewerDemoView drawing

void CViewerDemoView::OnDraw(CDC* pDC)
{
    CViewerDemoDoc* pDoc = GetDocument();
    ASSERT_VALID(pDoc);
    if (!pDoc)
        return;

    // TODO: add draw code for native data here
    // This will fail because we are passing a screen DC
    // to the ShowDC function. Only memory DCs will work.
    // Check the debugger output window.
    ShowDC(*pDC);

    pja::CCompatibleDC memDC(*pDC);

    // This will show a 1 x 1 pixel monochrome image
    ShowDC2(memDC, _T("This single pixel monochrome bitmap is what is in a memory DC that does not have a bitmap selected into it."));

    CBitmap bmp;
    bmp.LoadBitmap(IDB_BITMAP1);

    // We will now show the bitmap that was just loaded
    ShowBitmap(bmp);

    SelectObject(memDC, bmp);

    HICON hIcon;
    for (int x = IDI_ICON1; x <= IDI_ICON4; ++x)
    {
        hIcon = (HICON)LoadImage(AfxGetInstanceHandle(),
                                 MAKEINTRESOURCE(x),
                                 IMAGE_ICON,
                                 0,
                                 0,
                                 LR_DEFAULTCOLOR | LR_SHARED);

        // Here we show the icon that was just loaded.
        // The AND mask is on the left,
        // the XOR mask is in the center, and
        // the icon is drawn on the right.
        ShowIcon2(hIcon, _T("Another icon that is used to build the final image."));

        DrawIcon(memDC, 0, 0, hIcon);

        // Here we show the contents of the in memory DC
        ShowDC(memDC);
    }

    BitBlt(*pDC, 20, 20, 48, 48, memDC, 0, 0, SRCCOPY);

    // Just for the heck of it, we will now show that you can
    // use the ShowIcon function to show cursors.
    HCURSOR Wait = LoadCursor(NULL, IDC_WAIT);
    ShowIcon2(Wait, _T("This is a cursor that was loaded just for the fun of it :o)"));

    // And Image Lists
    CImageList ImageList;
    ImageList.Create(IDR_MAINFRAME, 16, 1, RGB(192, 192, 192));

    ShowImageList2(ImageList, -1, ILD_MASK, _T("ImageList from the demo app's toolbar using ILD_MASK flag for the bottom row"));
    ShowImageList2(ImageList, -1, ILD_SELECTED, _T("ImageList from the demo app's toolbar using ILD_SELECTED flag for the bottom row"));
    ImageList.DeleteImageList();

    // Now a font
    CFont Font;
    LOGFONT lf = {0};
    lf.lfHeight = -20;
    _tcscpy(lf.lfFaceName, _T("Lucinda"));

    for (int x = 0; x <= 3600; x += 300)
    {
        lf.lfEscapement = x;
        lf.lfOrientation = lf.lfEscapement;
        Font.DeleteObject();
        Font.CreateFontIndirect(&lf);
        ShowGraphic(_T("Lucinda"), Font);
    }


    // Now a bitmap file. Loading it as a DIBSection ensures it keeps it's colour depth
    HBITMAP hbmp = (HBITMAP)LoadImage(NULL, _T("Chicken24.bmp"), IMAGE_BITMAP, 0, 0, LR_CREATEDIBSECTION | LR_LOADFROMFILE);
    ShowBitmap2(hbmp, _T("This is a 24 bit per pixel bitmap loaded from a file"));

    // Now lets show a region
    CRgn Region;
    POINT pts[] = { {40, 0}, {10, 60}, {80, 20}, {0, 20}, {70, 60} };
    Region.CreatePolygonRgn(pts, _countof(pts), ALTERNATE);
    LRESULT ret = ShowRegion3(Region, hbmp, _T("Region on a bitmap"));
    int offset = -100;
    Region.OffsetRgn(offset, offset);
    ShowRegion2(Region, _T("This is a region on the default background"));

    for (int x = 0; x < 40; ++x)
    {
        ret = ShowGraphic(Region, hbmp, _T("The region's offset is %d, %d"), offset, offset);
        offset += 10;
        Region.OffsetRgn(10, 10);
    }
    ShowGraphic(Region);

    DeleteObject(hbmp);
    DeleteObject(Region);
}


// CViewerDemoView diagnostics

#ifdef _DEBUG
void CViewerDemoView::AssertValid() const
{
    CView::AssertValid();
}

void CViewerDemoView::Dump(CDumpContext& dc) const
{
    CView::Dump(dc);
}

CViewerDemoDoc* CViewerDemoView::GetDocument() const // non-debug version is inline
{
    ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CViewerDemoDoc)));
    return (CViewerDemoDoc*)m_pDocument;
}
#endif //_DEBUG


// CViewerDemoView message handlers
