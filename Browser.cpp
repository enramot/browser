#include "stdafx.h"
#include "catsets.h"
#include "BrowserDoc.h"
#include "DbView.h"
#include "Browser.h"
#include "SqlView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

BEGIN_MESSAGE_MAP(CBrowserApp, CWinApp)
	ON_COMMAND(ID_APP_ABOUT, OnAppAbout)
	ON_COMMAND(ID_FILE_NEW, CWinApp::OnFileNew)
	ON_COMMAND(ID_FILE_OPEN, CWinApp::OnFileOpen)
	ON_COMMAND(ID_FILE_PRINT_SETUP, CWinApp::OnFilePrintSetup)
END_MESSAGE_MAP()

CBrowserApp theApp;

CBrowserApp::CBrowserApp() {}

BOOL CBrowserApp::InitInstance() {
	INITCOMMONCONTROLSEX InitCtrls;
	InitCtrls.dwSize = sizeof(InitCtrls);
	InitCtrls.dwICC = ICC_WIN95_CLASSES;
	InitCommonControlsEx(&InitCtrls);

	CWinApp::InitInstance();

	if(!AfxOleInit()) {
		AfxMessageBox(IDP_OLE_INIT_FAILED);
		return FALSE;
	}

	_tsetlocale(LC_ALL, "");

	SetRegistryKey(_T("Trovatn Systemutvikling"));
	LoadStdProfileSettings(0);

	CSingleDocTemplate* pDocTemplate = new CSingleDocTemplate(
		IDR_MAINFRAME,
		RUNTIME_CLASS(CBrowserDoc),
		RUNTIME_CLASS(CMainFrame),
		RUNTIME_CLASS(CSqlEditorView));
	AddDocTemplate(pDocTemplate);

	EnableShellOpen();
	RegisterShellFileTypes(TRUE);
	COleObjectFactory::UpdateRegistryAll();

	CCommandLineInfo cmdInfo;
	ParseCommandLine(cmdInfo);

	if(!ProcessShellCommand(cmdInfo)) {
		return FALSE;
	}

	m_pMainWnd->ShowWindow(SW_SHOW);
	m_pMainWnd->UpdateWindow();
	m_pMainWnd->DragAcceptFiles();

	return TRUE;
}

class CAboutDlg: public CDialog {
public:
	CAboutDlg();
	enum {
		IDD = IDD_ABOUTBOX
	};
};

CAboutDlg::CAboutDlg(): CDialog(CAboutDlg::IDD) {}

void CBrowserApp::OnAppAbout() {
	CAboutDlg aboutDlg;
	aboutDlg.DoModal();
}
