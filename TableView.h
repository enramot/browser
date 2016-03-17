#pragma once

class CSortClass
{
public:
	CSortClass(CListCtrl* _pWnd, const int _iCol);
	virtual ~CSortClass();
	int m_iCol;
	CListCtrl* m_pWnd;
	void Sort(const bool bAscending);
	static int CALLBACK CompareFunction(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
public:
	class CSortItem
	{
	public:
		virtual  ~CSortItem();
		CSortItem(const DWORD dw, const CString& txt);
		CString m_txt;
		DWORD m_dw;
	};
};

class CContentView : public CListView
{
private:
	CBrowserDoc* m_pDoc;
protected:
	CContentView();
	DECLARE_DYNCREATE(CContentView)
public:

	bool m_bSortAscending;
	void Export();
	int	GetColumnCount() const;
	void AdjustColumnWidths();
	void ShowColumns(CColumns* pColumn);
	//	void SetRecordset(CDynamicBulkSet* prs);
	void ResetContent();
	void Populate(CDynamicBulkSet* prs);
private:
	void CreateColumns(CDynamicBulkSet* prs);
private:
	// Overrides
	// ClassWizard generated virtual function overrides
	//{{AFX_VIRTUAL(CContentView)
public:
	virtual void OnInitialUpdate();
protected:
	virtual void OnDraw(CDC* pDC);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	//}}AFX_VIRTUAL

	// Implementation
protected:
	virtual ~CContentView();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

	// Generated message map functions
protected:
	//{{AFX_MSG(CContentView)
	afx_msg void OnListCopy();
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSelectAll();
	afx_msg void OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult);
	//}}AFX_MSG

	DECLARE_MESSAGE_MAP()
};
