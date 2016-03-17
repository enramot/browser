#include "stdafx.h"
#include "catsets.h"
#include "browserdoc.h"
#include "mainfrm.h"
#include "Browser.h"
#include "SqlView.h"
#include "DbView.h"
#include "TableView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CContentView, CListView)

CContentView::CContentView() {
	m_pDoc = 0;
	m_bSortAscending = false;
}

CContentView::~CContentView() {}

BEGIN_MESSAGE_MAP(CContentView, CListView)
	//{{AFX_MSG_MAP(CContentView)
	ON_COMMAND(ID_FILE_PRINTF, Export)
	ON_COMMAND(ID_POPUP_ADJUSTWIDTH, AdjustColumnWidths)
	ON_COMMAND(ID_LISTCOPY, OnListCopy)
	ON_WM_CREATE()
	ON_COMMAND(ID_SELECTALL, OnSelectAll)
	ON_NOTIFY_REFLECT(LVN_COLUMNCLICK, OnColumnClick)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CContentView::OnDraw(CDC*) {}

#ifdef _DEBUG
void CContentView::AssertValid() const {
	CListView::AssertValid();
}

void CContentView::Dump(CDumpContext& dc) const {
	CListView::Dump(dc);
}
#endif

void CContentView::ResetContent() {
	GetListCtrl().DeleteAllItems();
	int nColCount = GetColumnCount();

	for(int nIndex = nColCount - 1; nIndex >= 0; nIndex--) {
		GetListCtrl().DeleteColumn(nIndex);
	}
}

void CContentView::Populate(CDynamicBulkSet* prs) {
	BeginWaitCursor();
	CListCtrl& theCtrl = GetListCtrl();
	CODBCFieldInfo info;
	CString strData;
	LPSTR rgData;
	long* rgLength;
	WORD nStatus;

	ResetContent();
	CreateColumns(prs);

	if(prs->IsEOF() || prs->IsBOF()) {
		return;
	}

	for(int n = 0; n < m_pDoc->m_nRcount; n++) {
		prs->MoveNext();
		if(prs->IsEOF() || prs->IsBOF()) {
			return;
		}
	}

	theCtrl.DeleteAllItems();
	int nFields = prs->GetODBCFieldCount();
	int nRowsFetched = prs->GetRowsFetched();

	m_pDoc->m_bNext = nRowsFetched == m_pDoc->m_nRowSize;

	for(int nField = 0; nField < nFields; nField++) {
		rgData = (LPSTR) prs->m_ppvData[nField];
		rgLength = (long*) prs->m_ppvLengths[nField];

		for(int nRow = 0; nRow < nRowsFetched; nRow++) {
			nStatus = prs->GetRowStatus((unsigned short) (nRow + 1));
			if(rgLength[nRow] == SQL_NULL_DATA) {
				strData = _T("<NULL>");
			}
			else {
				strData = &rgData[nRow * MAX_TEXT_LEN];
			}
			if(!nField) {
				theCtrl.InsertItem(nRow, strData);
			}
			else {
				theCtrl.SetItemText(nRow, nField, strData);
			}
		}
	}

	AdjustColumnWidths();
	EndWaitCursor();
}

void CContentView::CreateColumns(CDynamicBulkSet* prs) {
	CODBCFieldInfo info;
	CString sFieldName;
	CListCtrl& theCtrl = GetListCtrl();
	int nColumns = prs->GetODBCFieldCount();

	for(short nNum = 0; nNum < nColumns; nNum++) {
		prs->GetODBCFieldInfo(nNum, info);
		sFieldName = info.m_strName;
		sFieldName.TrimRight();
		theCtrl.InsertColumn(nNum, info.m_strName, LVCFMT_LEFT, theCtrl.GetStringWidth(info.m_strName) + 12);
	}
}

BOOL CContentView::PreCreateWindow(CREATESTRUCT& cs) {
	cs.style &= ~LVS_TYPEMASK;
	cs.style &= ~LVS_SINGLESEL;
	cs.style &= ~(LVS_SORTASCENDING | LVS_SORTDESCENDING);
	cs.style |= LVS_REPORT | LVS_SHOWSELALWAYS;

	return CListView::PreCreateWindow(cs);
}

void CContentView::ShowColumns(CColumns* pColumn) {
	ResetContent();
	CListCtrl& theCtrl = GetListCtrl();
	CString	strDataSource = "Browsing: ";

	strDataSource += m_pDoc->GetDsn();
	strDataSource += _T(" - ");
	strDataSource += m_pDoc->m_strTableName;
	strDataSource += _T(" [Column Info]");
	m_pDoc->SetTitle(strDataSource);

	theCtrl.InsertColumn(0, _T("Name"), LVCFMT_LEFT, 120, -1);
	theCtrl.InsertColumn(1, _T("Type"), LVCFMT_CENTER, 100, 1);
	theCtrl.InsertColumn(2, _T("Length"), LVCFMT_CENTER, 80, 2);
	theCtrl.InsertColumn(3, _T("Precision"), LVCFMT_CENTER, 80, 3);
	theCtrl.InsertColumn(4, _T("Scale"), LVCFMT_CENTER, 50, 4);
	theCtrl.InsertColumn(5, _T("Radix"), LVCFMT_CENTER, 50, 5);
	theCtrl.InsertColumn(6, _T("Nullable"), LVCFMT_CENTER, 50, 6);
	theCtrl.InsertColumn(7, _T("Remarks"), LVCFMT_CENTER, 80, 7);
	int	item = 0;
	while(!pColumn->IsEOF()) {
		CString	strValue;
		theCtrl.InsertItem(item, pColumn->m_sColumnName);
		int nColumn = 1;
		theCtrl.SetItem(item, nColumn++, LVIF_TEXT, pColumn->m_sTypeName, 0, 0, 0, 0);
		strValue.Format(_T("%d"), pColumn->m_nLength);
		theCtrl.SetItem(item, nColumn++, LVIF_TEXT, strValue, 0, 0, 0, 0);
		strValue.Format(_T("%d"), pColumn->m_nPrecision);
		theCtrl.SetItem(item, nColumn++, LVIF_TEXT, strValue, 0, 0, 0, 0);

		if(!pColumn->IsFieldNull(&(pColumn->m_nScale))) {
			strValue.Format(_T("%d"), pColumn->m_nScale);
			theCtrl.SetItem(item, nColumn++, LVIF_TEXT, strValue, 0, 0, 0, 0);
		}
		else {
			theCtrl.SetItem(item, nColumn++, LVIF_TEXT, _T("<na>"), 0, 0, 0, 0);
		}
		if(!pColumn->IsFieldNull(&(pColumn->m_nRadix))) {
			strValue.Format(_T("%d"), pColumn->m_nRadix);
			theCtrl.SetItem(item, nColumn++, LVIF_TEXT, strValue, 0, 0, 0, 0);
		}
		else {
			theCtrl.SetItem(item, nColumn++, LVIF_TEXT, _T("<na>"), 0, 0, 0, 0);
		}
		if(pColumn->m_fNullable == SQL_NO_NULLS) {
			theCtrl.SetItem(item, nColumn++, LVIF_TEXT, _T("No"), 0, 0, 0, 0);
		}
		else if(pColumn->m_fNullable == SQL_NULLABLE) {
			theCtrl.SetItem(item, nColumn++, LVIF_TEXT, _T("Yes"), 0, 0, 0, 0);
		}
		else {
			theCtrl.SetItem(item, nColumn++, LVIF_TEXT, _T("Unknown"), 0, 0, 0, 0);
		}
		theCtrl.SetItem(item, nColumn++, LVIF_TEXT, pColumn->m_sRemarks, 0, 0, 0, 0);
		item++;
		pColumn->MoveNext();
	}
	AdjustColumnWidths();
}

void CContentView::OnInitialUpdate() {
	m_pDoc = (CBrowserDoc*) GetDocument();
	CListView::OnInitialUpdate();
	DWORD_PTR dwStyle;
	dwStyle = GetListCtrl().SendMessage(LVM_GETEXTENDEDLISTVIEWSTYLE, 0, 0);
	dwStyle |= LVS_EX_FULLROWSELECT | LVS_EX_HEADERDRAGDROP;
	GetListCtrl().SendMessage(LVM_SETEXTENDEDLISTVIEWSTYLE, 0, dwStyle);
}

void CContentView::OnSelectAll() {
	CListCtrl& theCtrl = GetListCtrl();
	int nIndex = theCtrl.GetItemCount();
	SetRedraw(false);
	for(int i = 0; i < nIndex; i++) {
		theCtrl.SetItemState(i, LVIS_SELECTED | LVIS_FOCUSED, LVIS_SELECTED | LVIS_FOCUSED);
	}
	SetRedraw(true);
}

void CContentView::OnListCopy() {
	CString source;
	CListCtrl& theCtrl = GetListCtrl();
	CHeaderCtrl* pHeader = (CHeaderCtrl*) theCtrl.GetDlgItem(0);
	CWaitCursor wait;
	int nIndex = GetColumnCount();
	HDITEM hdi;
	char  lpBuffer[256];
	hdi.mask = HDI_TEXT;
	hdi.pszText = lpBuffer;
	hdi.cchTextMax = 256;
	CString sFieldSeparator = m_pDoc->GetFieldSeparator();

	for(int x = 0; x < nIndex; x++) {
		pHeader->GetItem(x, &hdi);
		source += hdi.pszText;
		source += sFieldSeparator;
	}
	source += "\r\n";

	int i = theCtrl.GetNextItem(-1, LVNI_ALL | LVNI_SELECTED);
	while(i != -1) {
		for(int n = 0; n < nIndex; n++) {
			source += theCtrl.GetItemText(i, n) + sFieldSeparator;
		}
		source += "\r\n";
		i = theCtrl.GetNextItem(i, LVNI_ALL | LVNI_SELECTED);
	}

	if(OpenClipboard()) {
		EmptyClipboard();
		HANDLE clipbuffer = GlobalAlloc(GMEM_DDESHARE, source.GetLength() + 1);
		char* buffer = (char*) GlobalLock(clipbuffer);
		strcpy(buffer, LPCSTR(source));
		GlobalUnlock(clipbuffer);
		SetClipboardData(CF_TEXT, clipbuffer);
		CloseClipboard();
	}
}

void CContentView::Export() {
	CListCtrl& theCtrl = GetListCtrl();
	int nCount = theCtrl.GetItemCount();
	CStdioFile outFile;
	CString strPathName;
	CString buf;
	CFileException e;
	CString sFieldSeparator = m_pDoc->GetFieldSeparator();

	char szFilter[] = "csv files (*.csv)|*.csv||All Files (*.*)|*.*|";
	CFileDialog dlg(FALSE, "csv", m_pDoc->m_strTableName, OFN_HIDEREADONLY, szFilter);

	if(dlg.DoModal() == IDOK) {
		CWaitCursor wait;
		strPathName = dlg.GetPathName();

		if(outFile.Open(strPathName, CFile::modeCreate | CFile::modeWrite | CFile::typeText)) {
			outFile.SeekToEnd();

			CHeaderCtrl* pHeader = (CHeaderCtrl*) theCtrl.GetDlgItem(0);
			HDITEM hdi;
			char  lpBuffer[256];
			hdi.mask = HDI_TEXT;
			hdi.pszText = lpBuffer;
			hdi.cchTextMax = 256;

			for(int x = 0; x < pHeader->GetItemCount(); x++) {
				pHeader->GetItem(x, &hdi);
				buf += hdi.pszText;
				buf += sFieldSeparator;
			}
			outFile.WriteString(buf);
			buf.Empty();
			outFile.WriteString("\n");
			for(int i = 0; i < nCount; i++) {
				for(int n = 0; n <= pHeader->GetItemCount(); n++) {
					buf += theCtrl.GetItemText(i, n) + sFieldSeparator;
				}
				buf += "\n";
				outFile.WriteString(buf);
				buf.Empty();
			}
			outFile.Close();

			static char szFileName[_MAX_PATH];
			szFileName[_MAX_PATH -1] = '\0';
			static char szTitleName[_MAX_FNAME + _MAX_EXT];

			CString tmp = " " + strPathName;
			LPTSTR szFile = tmp.GetBuffer(tmp.GetLength() + 1);

			GetShortPathName(szFileName, szFile, _MAX_PATH);
			if(m_pDoc->m_bOpenInExternalProgram) {
				STARTUPINFO siStartupInfo;
				PROCESS_INFORMATION piProcessInfo;

				memset(&siStartupInfo, 0, sizeof(siStartupInfo));
				memset(&piProcessInfo, 0, sizeof(piProcessInfo));

				siStartupInfo.cb = sizeof(siStartupInfo);

				if(CreateProcess(m_pDoc->m_sExternalProgramPath, szFile, NULL, NULL, FALSE, CREATE_NO_WINDOW | CREATE_DEFAULT_ERROR_MODE, NULL, NULL, &siStartupInfo, &piProcessInfo) == FALSE)
					WaitForSingleObject(piProcessInfo.hProcess, INFINITE);
				::CloseHandle(piProcessInfo.hThread);
				::CloseHandle(piProcessInfo.hProcess);
			}
		}
	}
}

void CContentView::AdjustColumnWidths() {
	if(m_pDoc->m_bAdjustColumnWidth) {
		SetRedraw(false);
		for(int Col = 0; Col <= GetColumnCount() - 1; Col++) {
			GetListCtrl().SetColumnWidth(Col, LVSCW_AUTOSIZE);
			int wc1 = GetListCtrl().GetColumnWidth(Col);
			GetListCtrl().SetColumnWidth(Col, LVSCW_AUTOSIZE_USEHEADER);
			int wc2 = GetListCtrl().GetColumnWidth(Col);
			int wc = max(wc1, wc2);
			wc = max(30, wc);
			GetListCtrl().SetColumnWidth(Col, wc);
		}
		SetRedraw(true);
	}
}

int CContentView::GetColumnCount() const {
	CHeaderCtrl* pHeader = (CHeaderCtrl*) GetDlgItem(0);
	return pHeader->GetItemCount();
}

void CContentView::OnColumnClick(NMHDR* pNMHDR, LRESULT* pResult) {
	NM_LISTVIEW* pNMListView = (NM_LISTVIEW*) pNMHDR;
	CListCtrl& theCtrl = GetListCtrl();
	CSortClass s(&(theCtrl), pNMListView->iSubItem);
	s.Sort(m_bSortAscending);
	m_bSortAscending = !m_bSortAscending;
	*pResult = 0;
}

int CContentView::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	if(CListView::OnCreate(lpCreateStruct) == -1) {
		return -1;
	}
	return 0;
}

CSortClass::CSortClass(CListCtrl* pWnd, const int iCol) {
	m_iCol = iCol;
	m_pWnd = pWnd;

	if(m_pWnd == 0) {
		return;
	}

	int max = m_pWnd->GetItemCount();
	DWORD_PTR dw;
	CString txt;
	for(int i = 0; i < max; i++) {
		dw = m_pWnd->GetItemData(i);
		txt = m_pWnd->GetItemText(i, iCol);
		m_pWnd->SetItemData(i, (DWORD_PTR) new CSortItem(dw, txt));
	}
}

CSortClass::~CSortClass() {
	if(m_pWnd == 0) {
		return;
	}

	int max = m_pWnd->GetItemCount();
	CSortItem* pItem;

	for(int i = 0; i < max; i++) {
		pItem = (CSortItem*) m_pWnd->GetItemData(i);
		m_pWnd->SetItemData(i, pItem->m_dw);
		delete pItem;
	}
}

void CSortClass::Sort(const bool bAscending) {
	m_pWnd->SortItems(CompareFunction, bAscending);
}

int CALLBACK CSortClass::CompareFunction(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort) {
	CSortItem* i1 = (CSortItem*) lParam1;
	CSortItem* i2 = (CSortItem*) lParam2;

	int ai = 0;
	int bi = 0;
	char ca = 0;
	char cb = 0;
	const char* a = lParamSort ? i1->m_txt : i2->m_txt;
	const char* b = lParamSort ? i2->m_txt : i1->m_txt;

	while(true) {
		ca = a[ai];
		cb = b[bi];

		while(isspace(ca) || ca == '0') {
			ca = a[++ai];
		}

		while(isspace(cb) || cb == '0') {
			cb = b[++bi];
		}

		if(isdigit(ca) && isdigit(cb)) {
			int bias = 0;
			while(true) {
				if(!isdigit(ca) && !isdigit(cb)) {
					break;
				}
				else if(!isdigit(ca)) {
					return -1;
				}
				else if(!isdigit(cb)) {
					return 1;
				}
				else if(ca < cb) {
					if(!bias) {
						bias = -1;
					}
				}
				else if(ca > cb) {
					if(!bias) {
						bias = 1;
					}
				}
				else if(!ca  &&  !cb) {
					return bias;
				}

				ca = a[++ai];
				cb = b[++bi];
			}
			if(bias) {
				return bias;
			}
		}

		if(!ca && !cb) {
			return strcmp(&ca, &cb);
		}

		ca = (char) toupper(ca);
		cb = (char) toupper(cb);

		if(ca < cb) {
			return -1;
		}
		else if(ca > cb) {
			return 1;
		}
		++ai;
		++bi;
	}
}

CSortClass::CSortItem::CSortItem(const DWORD dw, const CString& txt) {
	m_dw = dw;
	m_txt = txt;
}

CSortClass::CSortItem::~CSortItem() {}
