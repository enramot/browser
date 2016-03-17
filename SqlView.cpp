#include "stdafx.h"
#include "catsets.h"
#include "Browser.h"
#include "DbView.h"
#include "BrowserDoc.h"
#include "SqlView.h"
#include "TableView.h"
#include "MainFrm.h"

#ifdef _DEBUG
#define new DEBUG_NEW
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#endif

IMPLEMENT_DYNCREATE(CSqlEditorView, CEditView)

BEGIN_MESSAGE_MAP(CSqlEditorView, CEditView)
	//{{AFX_MSG_MAP(CSqlEditorView)
	ON_COMMAND(IDC_EXECUTE, OnExecute)
	//}}AFX_MSG_MAP
	// Standard printing commands
	ON_COMMAND(ID_LISTCOPY, &CSqlEditorView::OnListCopy)
END_MESSAGE_MAP()

/////////////////////////////////////////////////////////////////////////////
// CSqlEditorView construction/destruction

CSqlEditorView::CSqlEditorView() {
	bExecuted = false;
}

CSqlEditorView::~CSqlEditorView() {}

BOOL CSqlEditorView::PreCreateWindow(CREATESTRUCT& cs) {
	return CEditView::PreCreateWindow(cs);
}

/////////////////////////////////////////////////////////////////////////////
// CSqlEditorView diagnostics

#ifdef _DEBUG
void CSqlEditorView::AssertValid() const {
	CEditView::AssertValid();
}

void CSqlEditorView::Dump(CDumpContext& dc) const {
	CEditView::Dump(dc);
}

CBrowserDoc* CSqlEditorView::GetDocument() // non-debug version is inline
{
	ASSERT(m_pDocument->IsKindOf(RUNTIME_CLASS(CBrowserDoc)));
	return (CBrowserDoc*) m_pDocument;
}
#endif //_DEBUG

/////////////////////////////////////////////////////////////////////////////
// CSqlEditorView message handlers

BOOL CSqlEditorView::PreTranslateMessage(MSG* pMsg) {
	if(pMsg->wParam == VK_RETURN)
		if(GetKeyState(VK_CONTROL) < 0 && !bExecuted)
			OnExecute();
		else
			bExecuted = false;
	return CEditView::PreTranslateMessage(pMsg);
}

void CSqlEditorView::SetText(CString sText) {
	SetWindowText(sText);
}

void CSqlEditorView::OnExecute() {
	bExecuted = true;
	CString strChecked;
	GetWindowText(strChecked);
	strChecked.TrimLeft();
	strChecked.TrimRight();
	GetDocument()->ExecuteSQL(CheckString(strChecked));
}

CString CSqlEditorView::CheckString(CString innStr) {
	CString sCheckedString;
	unsigned int nLength = innStr.GetLength();
	UINT c;

	for(UINT unIndex = 0; unIndex < nLength; unIndex++) {
		c = innStr.GetAt(unIndex);
		switch(c) {
			case 8:
			case 9:
			case 10:
			case 13:
			case 32:
				sCheckedString += ' ';
				break;
			default:
				sCheckedString += (char) c;
		}
	}
	return sCheckedString;
}

void CSqlEditorView::OnListCopy() {
	GetEditCtrl().Copy();
}
