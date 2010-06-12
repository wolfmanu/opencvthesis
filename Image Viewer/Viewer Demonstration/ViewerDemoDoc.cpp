// ViewerDemoDoc.cpp : implementation of the CViewerDemoDoc class
//

#include "stdafx.h"
#include "ViewerDemo.h"

#include "ViewerDemoDoc.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#endif


// CViewerDemoDoc

IMPLEMENT_DYNCREATE(CViewerDemoDoc, CDocument)

BEGIN_MESSAGE_MAP(CViewerDemoDoc, CDocument)
END_MESSAGE_MAP()


// CViewerDemoDoc construction/destruction

CViewerDemoDoc::CViewerDemoDoc()
{
    // TODO: add one-time construction code here

}

CViewerDemoDoc::~CViewerDemoDoc()
{
}

BOOL CViewerDemoDoc::OnNewDocument()
{
    if (!CDocument::OnNewDocument())
        return FALSE;

    // TODO: add reinitialization code here
    // (SDI documents will reuse this document)

    return TRUE;
}




// CViewerDemoDoc serialization

void CViewerDemoDoc::Serialize(CArchive& ar)
{
    if (ar.IsStoring())
    {
        // TODO: add storing code here
    }
    else
    {
        // TODO: add loading code here
    }
}


// CViewerDemoDoc diagnostics

#ifdef _DEBUG
void CViewerDemoDoc::AssertValid() const
{
    CDocument::AssertValid();
}

void CViewerDemoDoc::Dump(CDumpContext& dc) const
{
    CDocument::Dump(dc);
}
#endif //_DEBUG


// CViewerDemoDoc commands
