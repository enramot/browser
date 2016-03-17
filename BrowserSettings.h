#pragma once

// CBrowserSettings dialog

class CBrowserSettings : public CDialog
{
	DECLARE_DYNAMIC(CBrowserSettings)

public:
	CBrowserSettings(CWnd* pParent = NULL);   // standard constructor
	virtual ~CBrowserSettings();

	// Dialog Data
	enum { IDD = IDD_SETTINGS };

protected:
	virtual void DoDataExchange(CDataExchange* pDX);    // DDX/DDV support

	DECLARE_MESSAGE_MAP()
public:
	afx_msg void OnBnClickedBrowserbutton();
	afx_msg void OnBnClickedOk();
	int m_nRowsetSize;
	BOOL m_bODBCCursor;
	BOOL m_bAliases;
	BOOL m_bSystemtables;
	BOOL m_bViews;
	BOOL m_bExportToExternalProgram;
	CString m_sColumnSeparator;
	BOOL m_bAdjustColumnwidths;
	CString m_sDefaultDb;
	CString m_sFilename;
	CString m_sPathname;

public:
	int m_nQueryTimeout;
	BOOL m_bQualifiedTableNames;
};
