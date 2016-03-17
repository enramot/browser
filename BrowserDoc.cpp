#include "stdafx.h"
#include "catsets.h"
#include "Browser.h"
#include "DbView.h"
#include "BrowserDoc.h"
#include "SqlView.h"
#include "mainfrm.h"
#include "TableView.h"
#include "BrowserSettings.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CBrowserDoc, CDocument)

BEGIN_MESSAGE_MAP(CBrowserDoc, CDocument)
	ON_COMMAND(ID_PREVIOUS, OnPrevious)
	ON_COMMAND(ID_NEXT, OnNext)
	ON_UPDATE_COMMAND_UI(ID_PREVIOUS, OnUpdatePrevious)
	ON_UPDATE_COMMAND_UI(ID_NEXT, OnUpdateNext)
	ON_COMMAND(ID_OPTIONS, OnOptions)
	ON_COMMAND(ID_DATAVIEW, OnDataview)
	ON_COMMAND(ID_DEFINITION, OnDefinition)
	ON_COMMAND(ID_RECORD_REQUERY, OnRecordRequery)
	ON_COMMAND(ID_FILE_OPEN_ODBC, OnFileNewOdbc)
	ON_UPDATE_COMMAND_UI(IDC_EXECUTE, OnUpdateExecute)
	ON_UPDATE_COMMAND_UI(ID_ADJUSTWIDTH, OnUpdateAdjustwidth)
	ON_UPDATE_COMMAND_UI(ID_DATAVIEW, OnUpdateDataview)
	ON_UPDATE_COMMAND_UI(ID_DEFINITION, OnUpdateDefinition)
	ON_UPDATE_COMMAND_UI(ID_FILE_PRINTF, OnUpdateFilePrintf)
	ON_UPDATE_COMMAND_UI(ID_RECORD_REQUERY, OnUpdateRecordRequery)
END_MESSAGE_MAP()

CBrowserDoc::CBrowserDoc() {
	CWinApp* pApp = AfxGetApp();
	m_pDatabase = 0;
	m_bShowContent = true;
	m_bShowDefinition = false;
	m_bEnableExecute = false;
	m_sDsn = "";
	m_sPassword = "";
	m_sUsername = "";
	m_sQuotechar = "";
	m_sDefaultDsn = rot13(pApp->GetProfileString(_T("Settings"), _T("OdbcName"), _T("None")).GetBuffer());
	m_sExternalProgramPath = pApp->GetProfileString(_T("Settings"), _T("ExternaProgramPath"), _T("None"));
	m_sExternalProgramName = pApp->GetProfileString(_T("Settings"), _T("ExternaProgramName"), _T("Program"));
	m_nSynonyms = pApp->GetProfileInt(_T("Settings"), _T("Alias"), 0);
	m_nViews = pApp->GetProfileInt(_T("Settings"), _T("View"), 0);
	m_nCursors = pApp->GetProfileInt(_T("Settings"), _T("UseODBCCursors"), 0);
	m_nSystemTables = pApp->GetProfileInt(_T("Settings"), _T("SystemTables"), 0);
	m_nRowSize = pApp->GetProfileInt(_T("Settings"), _T("RecordSize"), 500);
	m_nQueryTimeOut = pApp->GetProfileInt(_T("Settings"), _T("QueryTimeout"), 15);
	m_bOpenInExternalProgram = pApp->GetProfileInt(_T("Settings"), _T("Printing"), 0);
	m_bAdjustColumnWidth = pApp->GetProfileInt(_T("Settings"), _T("AdjustColumnWidth"), 1);
	m_bQualifiedTableNames = pApp->GetProfileInt(_T("Settings"), _T("QualifiedTableNames"), 1);
	m_sFieldSeparator = pApp->GetProfileString(_T("Settings"), _T("FieldSeparator"), _T(""));
	m_nRcount = 0;

	SetTableTypes();
}

CBrowserDoc::~CBrowserDoc() {
	delete m_pDatabase;
}

BOOL CBrowserDoc::OpenODBCConnection(CString ODBCName) {
	m_sDefaultDsn = ODBCName;
	OnNewDocument();
	return TRUE;
}

char* CBrowserDoc::rot13(char *str) {
	for(int i = 0; '\0' != str[i]; i++) {
		char c = *(str + i);
		if(('a' <= c && 'n' > c) || ('A' <= c && 'N' > c)) {
			*(str + i) += 13;
		}
		else if(('n' <= c && 'z' >= c) || ('N' <= c && 'Z' >= c)) {
			*(str + i) -= 13;
		}
	}
	return str;
}

BOOL CBrowserDoc::OnNewDocument() {
	m_bEnableExecute = false;
	if(!CDocument::OnNewDocument()) {
		return FALSE;
	}

	try {
		m_bShowContent = true;
		m_bShowDefinition = false;

		CString sConnectionString;
		if(m_sDefaultDsn.GetLength() > 0) {
			m_sUsername = rot13(AfxGetApp()->GetProfileString(m_sDefaultDsn, "Username").GetBuffer());
			m_sPassword = rot13(AfxGetApp()->GetProfileString(m_sDefaultDsn, "Password").GetBuffer());

			sConnectionString.Format("DSN=%s;UID=%s;PWD=%s", m_sDefaultDsn, m_sUsername, m_sPassword);
		}

		CDatabase* pDb = new CDatabase();

		if(pDb->OpenEx(sConnectionString)) {
			if(m_pDatabase && m_pDatabase->IsOpen()) {
				m_pDatabase->Close();
			}
			delete m_pDatabase;
			m_pDatabase = pDb;
			m_pDatabase->SetQueryTimeout(m_nQueryTimeOut);
		}
		else {
			delete pDb;
		}

		if(m_pDatabase) {
			m_bEnableExecute = false;

			UCHAR buffer[201];
			buffer[200] = '0';
			SWORD cbData;

			::SQLGetInfo(m_pDatabase->m_hdbc, SQL_IDENTIFIER_QUOTE_CHAR, (PTR) buffer, 200, &cbData);
			m_sQuotechar = buffer;

			::SQLGetInfo(m_pDatabase->m_hdbc, SQL_DATA_SOURCE_NAME, (PTR) buffer, 200, &cbData);
			m_sDsn = buffer;

			SetTitle(m_pDatabase->GetDatabaseName());

			::SQLGetInfo(m_pDatabase->m_hdbc, SQL_USER_NAME, (PTR) buffer, 200, &cbData);
			m_sUsername = buffer;

			CString cs = m_pDatabase->GetConnect();
			m_sUsername = getValueForKey(cs.Right(cs.GetLength() - 5), "UID");
			m_sPassword = getValueForKey(cs.Right(cs.GetLength() - 5), "PWD");

			AfxGetApp()->WriteProfileString(_T("Settings"), _T("OdbcName"), rot13(m_sDsn.GetBuffer()));
			AfxGetApp()->WriteProfileString(_T(m_sDsn), _T("Username"), rot13(m_sUsername.GetBuffer()));
			AfxGetApp()->WriteProfileString(_T(m_sDsn), _T("Password"), rot13(m_sPassword.GetBuffer()));
		}
	}

	catch(CDBException* e) {
		AfxMessageBox(e->m_strError, MB_ICONEXCLAMATION);
		e->Delete();
	}

	CMainFrame* pMainFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	if(pMainFrame) {
		CDbObjectsView* pDBO = pMainFrame->GetDbObjectsView();
		if(pDBO) {
			pDBO->GetDbObjects(m_pDatabase);
		}
	}

	return TRUE;
}

BOOL CBrowserDoc::OnOpenDocument(LPCTSTR lpszPathName) {
	if(!CDocument::OnOpenDocument(lpszPathName))
		return FALSE;

	m_bEnableExecute = false;

	try {
		CString sConnectionString;
		sConnectionString.Format("DSN=%s;UID=%s;PWD=%s", m_sDsn, m_sUsername, m_sPassword);
		CDatabase* pDb = new CDatabase();

		if(pDb->OpenEx(sConnectionString)) {
			if(m_pDatabase && m_pDatabase->IsOpen()) {
				m_pDatabase->Close();
			}

			delete m_pDatabase;
			m_pDatabase = pDb;
			m_pDatabase->SetQueryTimeout(m_nQueryTimeOut);
			SetTitle(m_pDatabase->GetDatabaseName());
		}

		UCHAR buffer[21];
		buffer[20] = '0';
		SWORD cbData;

		::SQLGetInfo(m_pDatabase->m_hdbc, SQL_IDENTIFIER_QUOTE_CHAR, (PTR) buffer, 200, &cbData);
		m_sQuotechar = buffer;
	}
	catch(CDBException* e) {
		AfxMessageBox(e->m_strError, MB_ICONEXCLAMATION);
		e->Delete();
		return FALSE;
	}

	CMainFrame* pMainFrame = (CMainFrame*) AfxGetApp()->m_pMainWnd;
	if(pMainFrame) {
		CDbObjectsView* pDBO = pMainFrame->GetDbObjectsView();
		if(pDBO) {
			pDBO->GetDbObjects(m_pDatabase);
		}
	}

	m_bEnableExecute = false;
	return TRUE;
}

#ifdef _DEBUG
void CBrowserDoc::AssertValid() const {
	CDocument::AssertValid();
}

void CBrowserDoc::Dump(CDumpContext& dc) const {
	CDocument::Dump(dc);
}
#endif

void CBrowserDoc::SetTableTypes() {
	size_t length = sizeof "'TABLE'";
	strcpy_s(lpszType, length, "'TABLE'");

	if(m_nViews) {
		length += sizeof ",'VIEW'";
		strcat_s(lpszType, length, ",'VIEW'");
	}

	if(m_nSystemTables) {
		length += sizeof ",'SYSTEM TABLE'";
		strcat_s(lpszType, length, ",'SYSTEM TABLE'");
	}

	if(m_nSynonyms) {
		length += sizeof ",'ALIAS','SYNONYM'";
		strcat_s(lpszType, length, ",'ALIAS','SYNONYM'");
	}
}

CString CBrowserDoc::GetDsn() {
	if(!m_pDatabase->IsOpen()) {
		return _T("[No Data Source Selected]");
	}

	CString	sDSN = m_pDatabase->GetConnect();
	sDSN = sDSN.Right(sDSN.GetLength() - (sDSN.Find(_T("DSN=")) + 4));
	sDSN = sDSN.Left(sDSN.Find(_T(";")));
	return sDSN;
}

void CBrowserDoc::OnCloseDocument() {
	if(m_pDatabase && m_pDatabase->IsOpen()) {
		m_pDatabase->Close();
		delete m_pDatabase;
		m_pDatabase = 0;
	}

	CDocument::OnCloseDocument();
}

void CBrowserDoc::GetData() {
	CWaitCursor wait;

	CDynamicBulkSet rs(m_pDatabase);
	CString sSql = m_sSqlStatement;

	sSql.TrimLeft();
	sSql = sSql.Left(6);

	if(!sSql.CompareNoCase("select")) {
		rs.SetRowsetSize(m_nRowSize);
		try {
			clock_t start = clock();
			((CMainFrame*) AfxGetApp()->m_pMainWnd)->WriteMessage("Records fetched: 0.    Time used: 0ms");
			rs.Open(CRecordset::snapshot, m_sSqlStatement, CRecordset::readOnly | CRecordset::useMultiRowFetch | CRecordset::executeDirect);
			CContentView* pTableView = ((CMainFrame*) AfxGetApp()->m_pMainWnd)->GetContentView();
			pTableView->Populate(&rs);
			int nRowsFetched = rs.GetRowsFetched();
			rs.Close();

			clock_t finish = clock();

			CString sRet;
			CString sFormattedString;

			FormatTime((finish - start), sRet);

			sFormattedString.Format("Records fetched: %i.    Time used: %s ", nRowsFetched, sRet);
			((CMainFrame*) AfxGetApp()->m_pMainWnd)->WriteMessage(sFormattedString);
			m_bEnableExecute = true;
		}
		catch(CDBException* e) {
			if(e->m_nRetCode != SQL_SUCCESS_WITH_INFO) {
				AfxMessageBox(e->m_strError, MB_ICONEXCLAMATION);
			}
			e->Delete();
		}
	}
	else {
		try {
			((CMainFrame*) AfxGetApp()->m_pMainWnd)->WriteMessage("");
			m_pDatabase->ExecuteSQL(m_sSqlStatement);
			m_bEnableExecute = true;
		}
		catch(CDBException* e) {
			if(e->m_nRetCode != SQL_SUCCESS_WITH_INFO)
				AfxMessageBox(e->m_strError, MB_ICONEXCLAMATION);
			e->Delete();
		}
	}
}

void CBrowserDoc::FormatTime(long time, CString &sRet) {
	long ms = (time % 1000);
	long s = ((time / 1000) % 60);
	long m = ((time / 60000) % 60);
	long h = ((time / 3600000) % 24);

	if(h) {
		sRet.Format("%2dh : %2dm : %2ds : %dms", h, m, s, ms);
		return;
	}
	if(m) {
		sRet.Format("%2dm : %2ds : %dms", m, s, ms);
		return;
	}
	if(s) {
		sRet.Format("%2ds : %dms", s, ms);
		return;
	}

	sRet.Format("%dms", ms);
}

void CBrowserDoc::OnNext() {
	m_nRcount++;
	GetData();
}

void CBrowserDoc::OnPrevious() {
	if(--m_nRcount < 0) {
		m_nRcount = 0;
	}
	GetData();
}

void CBrowserDoc::OnOptions() {
	CBrowserSettings dlg;

	dlg.m_nRowsetSize = m_nRowSize;
	dlg.m_nQueryTimeout = m_nQueryTimeOut;
	dlg.m_bAliases = m_nSynonyms;
	dlg.m_bSystemtables = m_nSystemTables;
	dlg.m_bViews = m_nViews;
	dlg.m_bODBCCursor = m_nCursors;

	dlg.m_bAdjustColumnwidths = m_bAdjustColumnWidth;
	dlg.m_bQualifiedTableNames = m_bQualifiedTableNames;
	dlg.m_bExportToExternalProgram = m_bOpenInExternalProgram;
	dlg.m_sFilename = m_sExternalProgramName;
	dlg.m_sPathname = m_sExternalProgramPath;
	dlg.m_sColumnSeparator = m_sFieldSeparator;
	dlg.m_sDefaultDb = rot13(m_sDsn.GetBuffer());

	if(dlg.DoModal() == IDOK) {
		CWinApp* pApp = AfxGetApp();
		ASSERT(pApp != NULL);

		m_nRowSize = dlg.m_nRowsetSize;
		m_nQueryTimeOut = dlg.m_nQueryTimeout;
		m_nSynonyms = dlg.m_bAliases;
		m_nSystemTables = dlg.m_bSystemtables;
		m_nViews = dlg.m_bViews;
		m_nCursors = dlg.m_bODBCCursor;

		m_bAdjustColumnWidth = dlg.m_bAdjustColumnwidths;
		m_bQualifiedTableNames = dlg.m_bQualifiedTableNames;
		m_bOpenInExternalProgram = dlg.m_bExportToExternalProgram;
		m_sExternalProgramName = dlg.m_sFilename;
		m_sExternalProgramPath = dlg.m_sPathname;
		m_sFieldSeparator = dlg.m_sColumnSeparator;
		m_sDsn = dlg.m_sDefaultDb;

		pApp->WriteProfileString(_T("Settings"), _T("OdbcName"), rot13(m_sDsn.GetBuffer()));
		pApp->WriteProfileString(_T("Settings"), _T("ExternaProgramName"), m_sExternalProgramName);
		pApp->WriteProfileString(_T("Settings"), _T("ExternaProgramPath"), m_sExternalProgramPath);
		pApp->WriteProfileInt(_T("Settings"), _T("Alias"), m_nSynonyms);
		pApp->WriteProfileInt(_T("Settings"), _T("View"), m_nViews);
		pApp->WriteProfileInt(_T("Settings"), _T("SystemTables"), m_nSystemTables);
		pApp->WriteProfileInt(_T("Settings"), _T("UseODBCCursors"), m_nCursors);
		pApp->WriteProfileInt(_T("Settings"), _T("AdjustColumnWidth"), m_bAdjustColumnWidth);
		pApp->WriteProfileInt(_T("Settings"), _T("QualifiedTableNames"), m_bQualifiedTableNames);
		pApp->WriteProfileInt(_T("Settings"), _T("RecordSize"), m_nRowSize);
		pApp->WriteProfileInt(_T("Settings"), _T("QueryTimeout"), m_nQueryTimeOut);
		pApp->WriteProfileInt(_T("Settings"), _T("Printing"), m_bOpenInExternalProgram);
		pApp->WriteProfileString(_T("Settings"), _T("FieldSeparator"), m_sFieldSeparator);
		SetTableTypes();
	}
}

void CBrowserDoc::OnFileNewOdbc() {
	if(OnNewDocument()) {
		((CMainFrame*) AfxGetApp()->m_pMainWnd)->GetDbObjectsView()->GetDbObjects(m_pDatabase);
	}
}

void CBrowserDoc::ExecuteSQL(CString sSQL) {
	m_sSqlStatement = sSQL;
	if(m_bShowContent)
		OnDataview();

	if(m_bShowDefinition)
		OnDefinition();
}

void CBrowserDoc::OnDataview() {
	m_bShowContent = true;
	m_bShowDefinition = false;

	GetData();
}

void CBrowserDoc::OnDefinition() {
	m_bShowDefinition = true;
	m_bShowContent = false;

	if(!m_strTableName.IsEmpty()) {
		FetchTableDefinitions();
	}
}

void CBrowserDoc::FetchTableDefinitions() {
	try {
		BeginWaitCursor();
		CColumns Columns(m_pDatabase);
		CString s = m_strTableName.Mid(m_strTableName.Find('[') + 1, m_strTableName.Find(']'));
		int start = m_strTableName.Find('[') + 1;
		Columns.Open(NULL, NULL, m_strTableName.Mid(start, m_strTableName.Find(']') - start), NULL, CRecordset::forwardOnly);
		CContentView* pTV = ((CMainFrame*) AfxGetApp()->m_pMainWnd)->GetContentView();
		pTV->ResetContent();
		pTV->ShowColumns(&Columns);
	}
	catch(CDBException* e) {
		if(e->m_nRetCode != SQL_SUCCESS_WITH_INFO) {
			AfxMessageBox(e->m_strError, MB_ICONEXCLAMATION);
		}
		e->Delete();
	}
}

void CBrowserDoc::OnRecordRequery() {
	BeginWaitCursor();
	GetData();
}

CString CBrowserDoc::GetFieldSeparator() {
	return m_sFieldSeparator.IsEmpty() ? ";" : m_sFieldSeparator;
}

CString CBrowserDoc::getValueForKey(CString str, CString key) {
	int i = 0;
	CString strNameValue;
	while(AfxExtractSubString(strNameValue, str, i, ';')) {
		i++;

		CString strName, strValue;
		if(!AfxExtractSubString(strName, strNameValue, 0, _T('='))) {
			OutputDebugString(_T("Error extracting name\r\n"));
			continue;
		}
		if(key.Compare(strName) != 0) {
			continue;
		}

		// Attempt to extract the value element from the pair
		if(!AfxExtractSubString(strValue, strNameValue, 1, _T('='))) {
			OutputDebugString(_T("Error extracting value element\r\n"));
			continue;
		}
		return strValue;
	}
	return "";
}

BOOL CBrowserDoc::SaveModified() {
	SetModifiedFlag(false);
	return CDocument::SaveModified();
}

void CBrowserDoc::OnUpdateNext(CCmdUI* pCmdUI) {
	pCmdUI->Enable(m_bNext);
}

void CBrowserDoc::OnUpdatePrevious(CCmdUI* pCmdUI) {
	pCmdUI->Enable(m_nRcount);
}

void CBrowserDoc::OnUpdateAdjustwidth(CCmdUI* pCmdUI) {
	pCmdUI->Enable(m_bEnableExecute);
}

void CBrowserDoc::OnUpdateRecordRequery(CCmdUI* pCmdUI) {
	pCmdUI->Enable(m_bEnableExecute);
}

void CBrowserDoc::OnUpdateExecute(CCmdUI* pCmdUI) {
	pCmdUI->Enable(m_bEnableExecute);
}

void CBrowserDoc::OnUpdateDataview(CCmdUI* pCmdUI) {
	pCmdUI->Enable(m_bEnableExecute && m_bShowDefinition);
}

void CBrowserDoc::OnUpdateDefinition(CCmdUI* pCmdUI) {
	pCmdUI->Enable(m_bEnableExecute && m_bShowContent);
}

void CBrowserDoc::OnUpdateFilePrintf(CCmdUI* pCmdUI) {
	pCmdUI->Enable(m_bEnableExecute);
}
