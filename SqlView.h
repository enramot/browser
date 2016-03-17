#pragma once

class CBrowserDoc;

class CSqlEditorView : public CEditView
{
protected: // create from serialization only
	CSqlEditorView();
	DECLARE_DYNCREATE(CSqlEditorView)

	// Attributes
public:
	CBrowserDoc* GetDocument();
	CString CheckString(CString innStr);
	void SetText(CString sText);
private:
	bool bExecuted;
	// Operations
public:

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CSqlEditorView)
public:
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
protected:
	//}}AFX_VIRTUAL

// Implementation
public:
	virtual ~CSqlEditorView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// Generated message map functions
protected:
	//{{AFX_MSG(CSqlEditorView)
	afx_msg void OnExecute();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnListCopy();
};

#ifndef _DEBUG  // debug version in BrowserView.cpp
inline CBrowserDoc* CSqlEditorView::GetDocument()
{
	return (CBrowserDoc*)m_pDocument;
}
#endif
