// ViewerDemoDoc.h : interface of the CViewerDemoDoc class
//


#pragma once


class CViewerDemoDoc : public CDocument
{
protected: // create from serialization only
    CViewerDemoDoc();
    DECLARE_DYNCREATE(CViewerDemoDoc)

// Attributes
public:

// Operations
public:

// Overrides
public:
    virtual BOOL OnNewDocument();
    virtual void Serialize(CArchive& ar);

// Implementation
public:
    virtual ~CViewerDemoDoc();
#ifdef _DEBUG
    virtual void AssertValid() const;
    virtual void Dump(CDumpContext& dc) const;
#endif

protected:

// Generated message map functions
protected:
    DECLARE_MESSAGE_MAP()
};


