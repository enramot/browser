#include "stdafx.h"
#include "catsets.h"
#include "BrowserDoc.h"
#include "mainfrm.h"
#include "Browser.h"
#include "TableView.h"
#include "SqlView.h"
#include "DbView.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CDbObjectsView, CTreeView)

CDbObjectsView::CDbObjectsView() {}

CDbObjectsView::~CDbObjectsView() {}

BEGIN_MESSAGE_MAP(CDbObjectsView, CTreeView)
	//{{AFX_MSG_MAP(CDbObjectsView)
	ON_NOTIFY_REFLECT(TVN_SELCHANGED, OnSelchanged)
	//}}AFX_MSG_MAP
END_MESSAGE_MAP()

void CDbObjectsView::OnDraw(CDC* /*pDC*/) {}

#ifdef _DEBUG
void CDbObjectsView::AssertValid() const {
	CTreeView::AssertValid();
}

void CDbObjectsView::Dump(CDumpContext& dc) const {
	CTreeView::Dump(dc);
}
#endif

BOOL CDbObjectsView::PreCreateWindow(CREATESTRUCT& cs) {
	cs.style |= WS_BORDER | TVS_HASLINES | TVS_HASBUTTONS | TVS_LINESATROOT | TVS_DISABLEDRAGDROP | TVS_SHOWSELALWAYS;
	return CTreeView::PreCreateWindow(cs);
}

void CDbObjectsView::OnInitialUpdate() {
	CTreeView::OnInitialUpdate();
	if(m_imageList == 0) {
		m_imageList.Create(IDB_TREEIMAGE, 16, 0, RGB(255, 0, 255));
		m_imageList.SetBkColor(GetSysColor(COLOR_WINDOW));
	}

	m_pDoc = (CBrowserDoc*) GetDocument();

	GetDbObjects(m_pDoc->m_pDatabase);
}

void CDbObjectsView::GetDbObjects(CDatabase* pDB) {
	CTreeCtrl& pTreeCtrl = GetTreeCtrl();
	pTreeCtrl.DeleteItem(pTreeCtrl.GetRootItem());

	((CMainFrame*) AfxGetApp()->m_pMainWnd)->GetContentView()->ResetContent();

	if(pDB) {
		CTables rsTables(pDB);
		try {
			rsTables.Open(NULL, NULL, NULL, m_pDoc->lpszType, CRecordset::forwardOnly);
		}

		catch(CDBException* e) {
			if(e->m_nRetCode != SQL_SUCCESS_WITH_INFO) {
				AfxMessageBox(e->m_strError, MB_ICONEXCLAMATION);
			}
			e->Delete();
		}

		CString strTableRef;
		CString sObjectName = pDB->GetDatabaseName();

		pTreeCtrl.SetImageList(&m_imageList, TVSIL_NORMAL);
		TV_INSERTSTRUCT tvinsert;

		tvinsert.hParent = NULL;
		tvinsert.hInsertAfter = TVI_LAST;
		tvinsert.item.mask = TVIF_IMAGE | TVIF_SELECTEDIMAGE | TVIF_TEXT;

		tvinsert.item.hItem = NULL;
		tvinsert.item.state = 0;
		tvinsert.item.stateMask = 0;
		tvinsert.item.cchTextMax = sObjectName.GetLength();
		tvinsert.item.pszText = sObjectName.GetBuffer();
		tvinsert.item.iImage = 0;
		tvinsert.item.iSelectedImage = 1;
		tvinsert.item.cChildren = 0;
		tvinsert.item.lParam = 0;

		HTREEITEM hTable = pTreeCtrl.InsertItem(&tvinsert);
		tvinsert.hParent = hTable;

		while(!rsTables.IsEOF()) {
			if(rsTables.m_sTableType.CompareNoCase("TABLE") == 0) {
				tvinsert.item.iImage = 2;
				tvinsert.item.iSelectedImage = 3;
			}
			else if(rsTables.m_sTableType.CompareNoCase("VIEW") == 0) {
				tvinsert.item.iImage = 10;
				tvinsert.item.iSelectedImage = 11;
			}

			else if(rsTables.m_sTableType.CompareNoCase("ALIAS") == 0) {
				tvinsert.item.iImage = 6;
				tvinsert.item.iSelectedImage = 7;
			}

			else if(rsTables.m_sTableType.CompareNoCase("SYNONYM") == 0) {
				tvinsert.item.iImage = 6;
				tvinsert.item.iSelectedImage = 7;
			}

			else if(rsTables.m_sTableType.CompareNoCase("SYSTEM TABLE") == 0) {
				tvinsert.item.iImage = 6;
				tvinsert.item.iSelectedImage = 7;
			}

			if(m_pDoc->m_bQualifiedTableNames)
				strTableRef.Format("%s.[%s]", rsTables.m_sTableOwner, rsTables.m_sTableName);
			else
				strTableRef.Format("[%s]", rsTables.m_sTableName);

			tvinsert.item.pszText = strTableRef.GetBuffer();
			pTreeCtrl.InsertItem(&tvinsert);
			rsTables.MoveNext();
		}
		pTreeCtrl.Expand(pTreeCtrl.GetRootItem(), TVE_EXPAND);
	}
}

void CDbObjectsView::OnSelchanged(NMHDR* pNMHDR, LRESULT* pResult) {
	NM_TREEVIEW* pNMTreeView = (NM_TREEVIEW*) pNMHDR;
	CTreeCtrl& pTreeCtrl = GetTreeCtrl();
	HTREEITEM hSelected = pNMTreeView->itemNew.hItem;
	if(hSelected != NULL) {
		char text[MAX_TEXT_LEN];
		TV_ITEM item;
		item.mask = TVIF_HANDLE | TVIF_TEXT | TVIF_SELECTEDIMAGE;
		item.hItem = hSelected;
		item.pszText = text;
		item.cchTextMax = MAX_TEXT_LEN + 1;
		pTreeCtrl.GetItem(&item);
		if(pTreeCtrl.GetParentItem(hSelected)) {
			m_pDoc->m_strTableName = item.pszText;
			m_pDoc->m_nRcount = 0;

			if(m_pDoc->m_strTableName.Find(' ', 0) >= 0) {
				m_pDoc->ExecuteSQL("select * from " + m_pDoc->m_sQuotechar + m_pDoc->m_strTableName + m_pDoc->m_sQuotechar);
			}
			else {
				m_pDoc->ExecuteSQL("select * from " + m_pDoc->m_strTableName);
			}
		}
	}
	*pResult = 0;
}
