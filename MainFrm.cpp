#include "stdafx.h"
#include "catsets.h"
#include "Browser.h"
#include "browserdoc.h"
#include "TableView.h"
#include "DbView.h"
#include "SqlView.h"
#include "DSNCollection.h"
#include "MainFrm.h"

#define MONITOR_CENTER   0x0001        // center rect to monitor
#define MONITOR_CLIP     0x0000        // clip rect to monitor
#define MONITOR_WORKAREA 0x0002        // use monitor work area
#define MONITOR_AREA     0x0000        // use monitor entire area

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CMainFrame, CFrameWnd)

BEGIN_MESSAGE_MAP(CMainFrame, CFrameWnd)
	ON_WM_CONTEXTMENU()
	ON_WM_CREATE()
	ON_WM_SIZE()
	ON_COMMAND(ID_HORIZ_EXPAND, OnHorizExpand)
	ON_COMMAND(ID_FILE_PRINTF, Export)
	ON_WM_HELPINFO()
	ON_COMMAND(ID_ALWAYS_ON_TOP, AlwaysOnTop)
	ON_COMMAND(ID_NEW_BROWSER, OnNewBrowser)
	ON_COMMAND(ID_HELP_FINDER, CFrameWnd::OnHelpFinder)
	ON_COMMAND(ID_HELP, CFrameWnd::OnHelp)
	ON_COMMAND(ID_CONTEXT_HELP, CFrameWnd::OnContextHelp)
	ON_COMMAND(ID_DEFAULT_HELP, CFrameWnd::OnHelpFinder)
	ON_CBN_SELENDOK(AFX_IDW_TOOLBAR + 1, OnODBCDrivers)
	ON_CBN_DROPDOWN(AFX_IDW_TOOLBAR + 1, OnODBCDriversDropDown)

END_MESSAGE_MAP()

static UINT indicators[] =
{
	ID_SEPARATOR,
	ID_INDICATOR_MELDING,
	ID_INDICATOR_CAPS,
	ID_INDICATOR_NUM,
	ID_INDICATOR_SCRL
};

CMainFrame::CMainFrame() {
	m_bInitialized = false;
}

CMainFrame::~CMainFrame() {}

int CMainFrame::OnCreate(LPCREATESTRUCT lpCreateStruct) {
	CImageList img;
	CString str;
	int idx = 0;

	if(CFrameWnd::OnCreate(lpCreateStruct) == -1) {
		return -1;
	}

	if(!m_wndReBar.Create(this)) {
		return -1;
	}

	if(!m_wndToolBar.CreateEx(this)) {
		return -1;
	}

	m_wndToolBar.GetToolBarCtrl().SetButtonWidth(50, 150);
	m_wndToolBar.GetToolBarCtrl().SetExtendedStyle(TBSTYLE_EX_DRAWDDARROWS);

	img.Create(IDB_HOTTOOLBAR, 22, 0, RGB(255, 0, 255));
	m_wndToolBar.GetToolBarCtrl().SetHotImageList(&img);
	img.Detach();

	img.Create(IDB_COLDTOOLBAR, 22, 0, RGB(255, 0, 255));
	m_wndToolBar.GetToolBarCtrl().SetImageList(&img);
	img.Detach();

	m_wndToolBar.ModifyStyle(0, TBSTYLE_FLAT | TBSTYLE_TRANSPARENT);
	m_wndToolBar.SetButtons(0, 12);

	m_wndToolBar.SetButtonInfo(idx, ID_NEW_BROWSER, TBSTYLE_BUTTON, 0);
	m_wndToolBar.SetButtonText(idx++, "New");
//	m_wndToolBar.SetButtonInfo(idx, ID_FILE_OPEN, TBSTYLE_BUTTON, 1);
//	m_wndToolBar.SetButtonText(idx++, "Open");
	m_wndToolBar.SetButtonInfo(idx, ID_FILE_PRINTF, TBSTYLE_BUTTON, 2);
	m_wndToolBar.SetButtonText(idx++, "Print");
	m_wndToolBar.SetButtonInfo(idx, ID_HORIZ_EXPAND, TBSTYLE_BUTTON, 3);
	m_wndToolBar.SetButtonText(idx++, "Expand");
//	m_wndToolBar.SetButtonInfo(idx, ID_HOME, TBSTYLE_BUTTON, 4);
//	m_wndToolBar.SetButtonText(idx++, "Home");
	m_wndToolBar.SetButtonInfo(idx, ID_DATAVIEW, TBSTYLE_BUTTON, 6);
	m_wndToolBar.SetButtonText(idx++, "Data");
	m_wndToolBar.SetButtonInfo(idx, ID_DEFINITION, TBSTYLE_BUTTON, 7);
	m_wndToolBar.SetButtonText(idx++, "Defs");
	m_wndToolBar.SetButtonInfo(idx, ID_NEXT, TBSTYLE_BUTTON, 9);
	m_wndToolBar.SetButtonText(idx++, "Next");
	m_wndToolBar.SetButtonInfo(idx, ID_PREVIOUS, TBSTYLE_BUTTON, 8);
	m_wndToolBar.SetButtonText(idx++, "Prev");
	m_wndToolBar.SetButtonInfo(idx, ID_RECORD_REQUERY, TBSTYLE_BUTTON, 10);
	m_wndToolBar.SetButtonText(idx++, "Refresh");
	m_wndToolBar.SetButtonInfo(idx, IDC_EXECUTE, TBSTYLE_BUTTON, 11);
	m_wndToolBar.SetButtonText(idx++, "Exec");
	m_wndToolBar.SetButtonInfo(idx, ID_OPTIONS, TBSTYLE_BUTTON, 12);
	m_wndToolBar.SetButtonText(idx++, "Options");
	m_wndToolBar.SetButtonInfo(idx, ID_APP_ABOUT, TBSTYLE_BUTTON, 13);
	m_wndToolBar.SetButtonText(idx++, "About");
	m_wndToolBar.SetButtonInfo(idx, ID_ALWAYS_ON_TOP, TBBS_CHECKBOX, 14);
	m_wndToolBar.SetButtonText(idx++, "On Top");

	CRect rectToolBar;
	m_wndToolBar.GetItemRect(0, &rectToolBar);
	m_wndToolBar.SetSizes(rectToolBar.Size(), CSize(30, 20));

	if(!m_wndODBCDrivers.Create(CBS_DROPDOWN | WS_CHILD, CRect(0, 0, 200, 120), this, AFX_IDW_TOOLBAR + 1)) {
		return -1;
	}

	m_wndReBar.AddBar(&m_wndToolBar);
	m_wndReBar.AddBar(&m_wndODBCDrivers, "ODBC:", NULL, RBBS_NOGRIPPER/*|RBBS_BREAK*/);

	REBARBANDINFO rbbi;

	rbbi.cbSize = sizeof(rbbi);
	rbbi.fMask = RBBIM_CHILDSIZE | RBBIM_IDEALSIZE | RBBIM_SIZE;
	rbbi.cxMinChild = rectToolBar.Width();
	rbbi.cyMinChild = rectToolBar.Height();
	rbbi.cx = rbbi.cxIdeal = rectToolBar.Width() * 15;
	m_wndReBar.GetReBarCtrl().SetBandInfo(0, &rbbi);
	rbbi.cxMinChild = 0;

	CRect rectODBC;

	rbbi.fMask = RBBIM_CHILDSIZE | RBBIM_IDEALSIZE;
	m_wndODBCDrivers.GetEditCtrl()->GetWindowRect(&rectODBC);
	rbbi.cyMinChild = rectODBC.Height() + 10;
	rbbi.cxIdeal = 200;
	m_wndReBar.GetReBarCtrl().SetBandInfo(2, &rbbi);
	m_wndToolBar.SetBarStyle(m_wndToolBar.GetBarStyle() | CBRS_TOOLTIPS | CBRS_FLYBY | CBRS_SIZE_FIXED);

	if(!m_wndStatusBar.Create(this) || !m_wndStatusBar.SetIndicators(indicators, sizeof(indicators) / sizeof(UINT))) {
		return -1;
	}

	m_wndStatusBar.SetPaneStyle(1, SBPS_STRETCH);

	WriteMessage("");
	return 0;
}

void CMainFrame::OnUpdateFrameTitle(BOOL) {
	AfxSetWindowText(m_hWnd, GetDocTitle());
}

BOOL CMainFrame::OnCreateClient(LPCREATESTRUCT, CCreateContext* pContext) {
	m_wndSplitterVert.CreateStatic(this, 1, 2);
	m_wndSplitterVert.CreateView(0, 0, RUNTIME_CLASS(CDbObjectsView), CSize(0, 0), pContext);
	m_wndSplitterHoriz.CreateStatic(&m_wndSplitterVert, 2, 1, WS_CHILD | WS_VISIBLE | WS_BORDER, m_wndSplitterVert.IdFromRowCol(0, 1));
	m_wndSplitterHoriz.CreateView(0, 0, RUNTIME_CLASS(CSqlEditorView), CSize(0, 50), pContext);
	m_wndSplitterHoriz.CreateView(1, 0, RUNTIME_CLASS(CContentView), CSize(0, 0), pContext);
	m_wndSplitterVert.SetColumnInfo(0, 175, 0);
	m_wndSplitterVert.RecalcLayout();
	return TRUE;
}

CString CMainFrame::GetDocTitle() {
	CString sTitle = GetActiveDocument()->GetTitle();
	return sTitle.IsEmpty() ? "Waiting" : sTitle;
}

BOOL CMainFrame::PreCreateWindow(CREATESTRUCT& cs) {
	cs.style |= FWS_PREFIXTITLE;
	return CFrameWnd::PreCreateWindow(cs);
}

#ifdef _DEBUG
void CMainFrame::AssertValid() const {
	CFrameWnd::AssertValid();
}

void CMainFrame::Dump(CDumpContext& dc) const {
	CFrameWnd::Dump(dc);
}
#endif

CSqlEditorView* CMainFrame::GetSqlEditorView() {
	return(CSqlEditorView*) m_wndSplitterHoriz.GetPane(0, 0);
}

CDbObjectsView* CMainFrame::GetDbObjectsView() {
	return(CDbObjectsView*) m_wndSplitterVert.GetPane(0, 0);
}

CContentView* CMainFrame::GetContentView() {
	return(CContentView*) m_wndSplitterHoriz.GetPane(1, 0);
}

void CMainFrame::OnSize(UINT nType, int cx, int cy) {
	CFrameWnd::OnSize(nType, cx, cy);
	if(m_bInitialized) {
		GetContentView()->Invalidate(true);
	}
	m_bInitialized = true;
}

void CMainFrame::OnHorizExpand() {
	HMONITOR hMonitor;
	MONITORINFO mi;
	RECT rMonitorWin;
	CRect rCurrWin;
	GetWindowRect(&rCurrWin);

	int width = rCurrWin.right - rCurrWin.left;
	int height = rCurrWin.bottom - rCurrWin.top;

	hMonitor = MonitorFromRect(&rCurrWin, MONITOR_DEFAULTTONEAREST);
	mi.cbSize = sizeof(mi);
	GetMonitorInfo(hMonitor, &mi);

	rMonitorWin = mi.rcWork;

	rCurrWin.left = max(rMonitorWin.left, min(rMonitorWin.right - width, rCurrWin.left));
	rCurrWin.top = max(rMonitorWin.top, min(rMonitorWin.bottom - height, rCurrWin.top));
	rCurrWin.right = rCurrWin.left + width;
	rCurrWin.bottom = rCurrWin.top + height;

	SetWindowPos(&CWnd::wndTop, rMonitorWin.left, rCurrWin.top, rMonitorWin.right - rMonitorWin.left, height, SWP_NOZORDER);
}

void CMainFrame::OnNewBrowser() {
	BeginWaitCursor();
	STARTUPINFO si;
	PROCESS_INFORMATION pi;
	RECT rc;

	GetWindowRect(&rc);
	ZeroMemory(&pi, sizeof(pi));
	ZeroMemory(&si, sizeof(si));
	si.cb = sizeof(si);

	si.dwX = rc.left + 10;
	si.dwY = rc.top + 10;
	si.dwXSize = rc.right - rc.left;
	si.dwYSize = rc.bottom - rc.top;
	si.wShowWindow = (WORD) CW_USEDEFAULT;
	si.dwFlags = STARTF_USESHOWWINDOW | STARTF_USESIZE | STARTF_USEPOSITION | STARTF_FORCEONFEEDBACK;
	CreateProcess(NULL, (LPSTR) AfxGetAppName(), NULL, NULL, FALSE, NORMAL_PRIORITY_CLASS, NULL, NULL, &si, &pi);
}

void CMainFrame::WriteMessage(CString sMsg) {
	m_wndStatusBar.SetPaneText(1, sMsg, TRUE);
}

BOOL CMainFrame::PreTranslateMessage(MSG* pMsg) {
	if((((pMsg->message == WM_KEYDOWN || pMsg->message == WM_SYSKEYDOWN) &&
	   (pMsg->wParam == VK_F10) && (GetKeyState(VK_SHIFT) & ~1)) != 0) ||
	   (pMsg->message == WM_CONTEXTMENU)) {
		CRect rect;
		GetClientRect(rect);
		ClientToScreen(rect);
		CPoint point = rect.TopLeft();
		point.Offset(5, 5);
		OnContextMenu(NULL, point);
		return TRUE;
	}
	return CFrameWnd::PreTranslateMessage(pMsg);
}

void CMainFrame::Export() {
	GetContentView()->Export();
}

BOOL CMainFrame::OnHelpInfo(HELPINFO*) {
	return TRUE;
}

void CMainFrame::OnContextMenu(CWnd*, CPoint point) {
	if(point.x == -1 && point.y == -1) {
		CRect rect;
		GetClientRect(rect);
		ClientToScreen(rect);
		point = rect.TopLeft();
		point.Offset(5, 5);
	}

	CMenu menu;
	VERIFY(menu.LoadMenu(CG_IDR_POPUP_MAIN_FRAME));
	CMenu* pPopup = menu.GetSubMenu(0);
	ASSERT(pPopup != NULL);
	CWnd* pWndPopupOwner = this;

	while(pWndPopupOwner->GetStyle() & WS_CHILD) {
		pWndPopupOwner = pWndPopupOwner->GetParent();
	}

	pPopup->TrackPopupMenu(TPM_LEFTALIGN | TPM_RIGHTBUTTON, point.x, point.y, pWndPopupOwner);
}

void CMainFrame::AlwaysOnTop() {
	if(GetExStyle() & WS_EX_TOPMOST) {
		SetWindowPos(&wndNoTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	}
	else {
		SetWindowPos(&wndTopMost, 0, 0, 0, 0, SWP_NOSIZE | SWP_NOMOVE);
	}
}

CDatabase* CMainFrame::GetDb() {
	return ((CBrowserDoc*) GetActiveDocument())->m_pDatabase;
}

void CMainFrame::AddToODBCCombo(CString sName) {
	if(!sName.IsEmpty()) {
		m_wndODBCDrivers.SetWindowText(sName);
		COMBOBOXEXITEM item;
		item.mask = CBEIF_TEXT;
		item.iItem = -1;
		item.pszText = (LPTSTR) (LPCTSTR) sName;
		m_wndODBCDrivers.InsertItem(&item);
	}
}

void CMainFrame::OnODBCDriversDropDown() {
	UpdateData(TRUE);
	m_wndODBCDrivers.ResetContent();

	CDSNCollection dsn;
	CString s = dsn.GetFirstDSNName(CDSNCollection::s_SYSTEM);
	if(dsn.GetLastSQLResult() == SQL_SUCCESS) {
		AddToODBCCombo(s);
	}

	for(; dsn.GetLastSQLResult() == SQL_SUCCESS;) {
		s = dsn.GetNextDSNName();

		if(dsn.GetLastSQLResult() == SQL_SUCCESS) {
			AddToODBCCombo(s);
		}
	}
}

void CMainFrame::OnODBCDrivers() {
	CString str;
	m_wndODBCDrivers.GetLBText(m_wndODBCDrivers.GetCurSel(), str);
	if(!str.IsEmpty()) {
		((CBrowserDoc*) GetActiveDocument())->OpenODBCConnection(str);
	}
}
