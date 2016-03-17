// BrowserSettings.cpp : implementation file
//

#include "stdafx.h"
#include "Browser.h"
#include "BrowserSettings.h"

// CBrowserSettings dialog

IMPLEMENT_DYNAMIC(CBrowserSettings, CDialog)

CBrowserSettings::CBrowserSettings(CWnd* pParent /*=NULL*/)
	: CDialog(CBrowserSettings::IDD, pParent)
	, m_nRowsetSize(0)
	, m_bODBCCursor(FALSE)
	, m_bAliases(FALSE)
	, m_bSystemtables(FALSE)
	, m_bViews(FALSE)
	, m_bExportToExternalProgram(FALSE)
	, m_sColumnSeparator(_T(""))
	, m_bAdjustColumnwidths(FALSE)
	, m_sDefaultDb(_T(""))
	, m_nQueryTimeout(0)
	, m_bQualifiedTableNames(FALSE) {
#ifndef _WIN32_WCE
	EnableActiveAccessibility();
#endif
}

CBrowserSettings::~CBrowserSettings() {}

void CBrowserSettings::DoDataExchange(CDataExchange* pDX) {
	CDialog::DoDataExchange(pDX);
	DDX_Text(pDX, IDC_ROWSETSIZE, m_nRowsetSize);
	DDX_Check(pDX, IDC_ODBCCURSOR, m_bODBCCursor);
	DDX_Check(pDX, IDC_ALIASES, m_bAliases);
	DDX_Check(pDX, IDC_SYSTEMTABLES, m_bSystemtables);
	DDX_Check(pDX, IDC_VIEWS, m_bViews);
	DDX_Check(pDX, IDC_PRINTTOPROGRAM, m_bExportToExternalProgram);
	DDX_Text(pDX, IDC_EDIT5, m_sColumnSeparator);
	DDX_Check(pDX, IDC_COLUMNWIDTH, m_bAdjustColumnwidths);
	DDX_Text(pDX, IDC_QUERYTIMEOUT, m_nQueryTimeout);
	DDX_Check(pDX, IDC_QUALIFIED_TABLE_NAMES, m_bQualifiedTableNames);
}

BEGIN_MESSAGE_MAP(CBrowserSettings, CDialog)
	ON_BN_CLICKED(IDC_BROWSERBUTTON, &CBrowserSettings::OnBnClickedBrowserbutton)
	ON_BN_CLICKED(IDOK, &CBrowserSettings::OnBnClickedOk)
END_MESSAGE_MAP()

// CBrowserSettings message handlers

void CBrowserSettings::OnBnClickedBrowserbutton() {
	UpdateData(true);
	CFileDialog dlg(TRUE, _T("EXE"), _T("*.EXE"), OFN_HIDEREADONLY | OFN_OVERWRITEPROMPT, _T("Executable (*.EXE)|*.EXE|"));
	if(dlg.DoModal() == IDOK) {
		m_sPathname = dlg.GetPathName();
		m_sFilename = dlg.GetFileName();
		GetDlgItem(IDC_BROWSERBUTTON)->SetWindowText(m_sFilename.Left(m_sFilename.Find(".")));
		UpdateData(false);
	}
}

void CBrowserSettings::OnBnClickedOk() {
	OnOK();
}
