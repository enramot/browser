#pragma once

class CBrowserDoc: public CDocument{
private:
	void FormatTime(long time, CString &sRet);
protected: // create from serialization only
	CBrowserDoc();
	DECLARE_DYNCREATE(CBrowserDoc)

	// Attributes
public:
	BOOL m_bAdjustColumnWidth;
	BOOL m_bOpenInExternalProgram;
	BOOL m_bQualifiedTableNames;
	bool m_bEnableExecute;
	bool m_bShowDefinition;
	bool m_bShowContent;
	bool m_bNext;
	int m_nRowSize;
	int m_nRcount;
	int m_nQueryTimeOut;
	int m_nCursors;
	int m_nViews;
	int m_nSynonyms;
	int	m_nSystemTables;
	CString m_sPassword;
	CString m_sUsername;
	CString m_sDsn;
	CString m_sDefaultDsn;
	CString m_sExternalProgramName;
	CString m_sExternalProgramPath;
	CString m_sQuotechar;
	CString m_sFieldSeparator;
	CString m_strTableName;
	CString m_strConnect;
	CDatabase* m_pDatabase;
	char lpszType[64];
protected:
	CString m_sSqlStatement;
	// Operations
private:
	CString getValueForKey(CString str, CString key);
public:
	CString GetFieldSeparator();
	CString GetDsn();
	char* rot13(char *str);
	void ExecuteSQL(CString sSql);
	void GetData();
	void FetchTableDefinitions();
	void SetTableTypes();
	BOOL OpenODBCConnection(CString ODBCName);
	virtual ~CBrowserDoc();

	// Overrides
		// ClassWizard generated virtual function overrides
		//{{AFX_VIRTUAL(CBrowserDoc)
public:
	virtual BOOL OnNewDocument();
	virtual BOOL OnOpenDocument(LPCTSTR lpszPathName);
	virtual void OnCloseDocument();
protected:
	virtual BOOL SaveModified();
	//}}AFX_VIRTUAL

// Implementation
public:
#ifdef _DEBUG
	virtual void AssertValid() const;
	virtual void Dump(CDumpContext& dc) const;
#endif

protected:

	// Generated message map functions
protected:
	//{{AFX_MSG(CBrowserDoc)
	afx_msg void OnPrevious();
	afx_msg void OnNext();
	afx_msg void OnUpdatePrevious(CCmdUI* pCmdUI);
	afx_msg void OnUpdateNext(CCmdUI* pCmdUI);
	afx_msg void OnOptions();
	afx_msg void OnDataview();
	afx_msg void OnDefinition();
	afx_msg void OnFileNewOdbc();
	afx_msg void OnRecordRequery();
	afx_msg void OnUpdateExecute(CCmdUI* pCmdUI);
	afx_msg void OnUpdateAdjustwidth(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDataview(CCmdUI* pCmdUI);
	afx_msg void OnUpdateDefinition(CCmdUI* pCmdUI);
	afx_msg void OnUpdateFilePrintf(CCmdUI* pCmdUI);
	afx_msg void OnUpdateRecordRequery(CCmdUI* pCmdUI);
	//}}AFX_MSG
	DECLARE_MESSAGE_MAP()
};
