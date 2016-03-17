// CSortColumn.cpp: implementation of the CSortColumn class.
//

#include "stdafx.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[]=__FILE__;
#define new DEBUG_NEW
#endif

//////////////////////////////////////////////////////////////////////
// CSortColumn

CSortColumn::CSortColumn()
{
	ASSERT(FALSE); // use the other one
}

CSortColumn::CSortColumn(CListCtrl* pWnd, const int& nCol, const bool& bIsNumeric)
{
    m_nCol = nCol;
    m_pWnd = pWnd;
    m_bIsNumeric = bIsNumeric;
    
    ASSERT(m_pWnd);
    int nMax = m_pWnd->GetItemCount();
    DWORD_PTR dwData;
    CString sTxt;
    if(m_bIsNumeric)
    {
        for(int n = 0; n < nMax; n++)
        {
            dwData = m_pWnd->GetItemData(n);
            sTxt = m_pWnd->GetItemText(n, m_nCol);
            m_pWnd->SetItemData(n, (DWORD_PTR)new CSortItemInt(dwData, sTxt));
        }
    }
    else
    {
        for(int n = 0; n < nMax; n++)
        {
            dwData = m_pWnd->GetItemData(n);
            sTxt = m_pWnd->GetItemText(n, m_nCol);
            m_pWnd->SetItemData(n, (DWORD_PTR)new CSortItemText(dwData, sTxt));
        }
    }
}

CSortColumn::~CSortColumn()
{
    ASSERT(m_pWnd);
    int nMax = m_pWnd->GetItemCount();
	if(m_bIsNumeric)
    {
        CSortItemInt* pItem;
		for(int n = 0; n < nMax; n++)
        {
            pItem = (CSortItemInt*)m_pWnd->GetItemData(n);
            ASSERT(pItem);
            m_pWnd->SetItemData(n, pItem->m_dwData);
            delete pItem;
        }
    }
    else
    {
        CSortItemText* pItem;
        for(int n = 0; n < nMax; n++)
        {
            pItem = (CSortItemText*)m_pWnd->GetItemData(n);
            ASSERT(pItem);
            m_pWnd->SetItemData(n, pItem->m_dwData);
            delete pItem;
        }
    }
}

void CSortColumn::Sort(const bool& bAsc)
{
    if(m_bIsNumeric)
    {
        if(bAsc)
            m_pWnd->SortItems(CompareAscInt, 0L);
		else
            m_pWnd->SortItems(CompareDesInt, 0L);
    }
    else
    {
        if(bAsc)
            m_pWnd->SortItems(CompareAscText, 0L);
		else
            m_pWnd->SortItems(CompareDesText, 0L);
    }
}

int CALLBACK CSortColumn::CompareAscText(LPARAM lParam1, LPARAM lParam2, LPARAM /*lParamSort*/)
{
    CSortItemText* pItem1 = (CSortItemText*)lParam1;
    CSortItemText* pItem2 = (CSortItemText*)lParam2;
    ASSERT(pItem1 && pItem2);
    return pItem1->m_strTxt.CompareNoCase(pItem2->m_strTxt);
}

int CALLBACK CSortColumn::CompareDesText(LPARAM lParam1, LPARAM lParam2, LPARAM /*lParamSort*/)
{
    CSortItemText* pItem1 = (CSortItemText*)lParam1;
    CSortItemText* pItem2 = (CSortItemText*)lParam2;
    ASSERT(pItem1 && pItem2);
    return pItem2->m_strTxt.CompareNoCase(pItem1->m_strTxt);
}

int CALLBACK CSortColumn::CompareAscInt(LPARAM lParam1, LPARAM lParam2, LPARAM /*lParamSort*/)
{
    CSortItemInt* pItem1 = (CSortItemInt*)lParam1;
    CSortItemInt* pItem2 = (CSortItemInt*)lParam2;
    ASSERT(pItem1 && pItem2);
    if(pItem1->m_nInt == pItem2->m_nInt)
		return 0;
    return pItem1->m_nInt > pItem2->m_nInt ? 1 : -1;
}

int CALLBACK CSortColumn::CompareDesInt(LPARAM lParam1, LPARAM lParam2, LPARAM /*lParamSort*/)
{
    CSortItemInt* pItem1 = (CSortItemInt*)lParam1;
    CSortItemInt* pItem2 = (CSortItemInt*)lParam2;
    ASSERT(pItem1 && pItem2);
    if(pItem1->m_nInt == pItem2->m_nInt)
		return 0;
    return pItem1->m_nInt < pItem2->m_nInt ? 1 : -1;
}

//////////////////////////////////////////////////////////////////////
// CSortItemText

CSortItemText::CSortItemText()
{
	ASSERT(FALSE); // use the other one  
}

CSortItemText::CSortItemText(const DWORD& dwData, const CString& sTxt)
{
    m_dwData = dwData;
    m_strTxt = sTxt;
}

CSortItemText::~CSortItemText()
{
}

//////////////////////////////////////////////////////////////////////
// CSortItemInt

CSortItemInt::CSortItemInt()
{
	ASSERT(FALSE); // use the other one  
}

CSortItemInt::CSortItemInt(const DWORD& dwData, const CString& sTxt)
{
    m_dwData = dwData;
#ifdef _UNICODE
	USES_CONVERSION;
	m_nInt = atoi(W2CA((LPCTSTR)sTxt));
#else
	m_nInt = atoi((LPCTSTR)sTxt);
#endif
}

CSortItemInt::~CSortItemInt()
{
}