#pragma once

class CBrowserDoc;

class CDbObjectsView : public CTreeView
{
protected:
	CDbObjectsView();
	DECLARE_DYNCREATE(CDbObjectsView)
	CImageList m_imageList;

public:
	CString m_sTitle;
	void GetDbObjects(CDatabase* pDB);
	CRecordset* m_pSet;
	CBrowserDoc* m_pDoc;

public:

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CDbObjectsView)
public:
	virtual void OnInitialUpdate();
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
protected:
	virtual void OnDraw(CDC* pDC);      // overridden to draw this view
	//}}AFX_VIRTUAL

protected:
	virtual ~CDbObjectsView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:
	//{{AFX_MSG(CDbObjectsView)
	afx_msg void OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
