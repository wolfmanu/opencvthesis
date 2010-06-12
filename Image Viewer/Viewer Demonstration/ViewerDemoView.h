// ViewerDemoView.h : interface of the CViewerDemoView class
//


#pragma once


class CViewerDemoView : public CView
{
protected: // create from serialization only
    CViewerDemoView();
    DECLARE_DYNCREATE(CViewerDemoView)

// Attributes
public:
    CViewerDemoDoc* GetDocument() const;

// Operations
public:

// Overrides
public:
    virtual void OnDraw(CDC* pDC);  // overridden to draw this view
    virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:

// Implementation
public:
    virtual ~CViewerDemoView();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
    DECLARE_MESSAGE_MAP()
};

#ifndef _DEBUG  // debug version in ViewerDemoView.cpp
inline CViewerDemoDoc* CViewerDemoView::GetDocument() const
   { return reinterpret_cast<CViewerDemoDoc*>(m_pDocument); }
#endif

