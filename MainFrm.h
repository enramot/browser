#pragma once

class CBrowserDoc;
class CDbObjectsView;
class CContentView;
class CSqlEditorView;

class CMainFrame : public CFrameWnd
{
protected: // create from serialization only
	CMainFrame();
	DECLARE_DYNCREATE(CMainFrame)
	void OnUpdateFrameTitle(BOOL bAddToTitle);
	bool m_bInitialized;
	// Attributes
public:
	CSplitterWnd m_wndSplitterHoriz;
	CSplitterWnd m_wndSplitterVert;
	CToolBar m_wndToolBar;
	CReBar m_wndReBar;
	CStatusBar m_wndStatusBar;
protected:
	CComboBoxEx m_wndODBCDrivers;
	//Operations
public:
	void WriteMessage(CString sMsg);
	CContentView* GetContentView();
	CSqlEditorView* GetSqlEditorView();
	CDbObjectsView* GetDbObjectsView();
	CString	GetDocTitle();
	CDatabase* GetDb();
	void AddToODBCCombo(CString sSQL);
	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CMainFrame)
public:
	virtual BOOL OnCreateClient(LPCREATESTRUCT lpcs, CCreateContext* pContext);
	virtual BOOL PreCreateWindow(CREATESTRUCT& cs);
	virtual BOOL PreTranslateMessage(MSG* pMsg);
	//}}AFX_VIRTUAL

// Implementation
public:
	void initDriverList();
	virtual ~CMainFrame();
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:  // control bar embedded members

// Generated message map functions
protected:
	//{{AFX_MSG(CMainFrame)
	afx_msg int OnCreate(LPCREATESTRUCT lpCreateStruct);
	afx_msg void OnSize(UINT nType, int cx, int cy);
	afx_msg void OnContextMenu(CWnd*, CPoint point);
	afx_msg void OnHorizExpand();
	afx_msg void OnNewBrowser();
	afx_msg void Export();
	afx_msg BOOL OnHelpInfo(HELPINFO* pHelpInfo);
	afx_msg void AlwaysOnTop();
	afx_msg void OnODBCDrivers();
	afx_msg void OnODBCDriversDropDown();
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
