#pragma once
class CSortItemText
{
public:
	CSortItemText();
	CSortItemText(const DWORD& dwData, const CString& sTxt);
	virtual  ~CSortItemText();

	CString m_strTxt;
	DWORD m_dwData;
};

//////////////////////////////////////////////////////////////////////
// CSortItemInt
	
class CSortItemInt
{
public:
	CSortItemInt();
	CSortItemInt(const DWORD& dwData, const CString& sTxt);
	virtual ~CSortItemInt();

	CString m_strTxt;
	int m_nInt;
	DWORD m_dwData;
};

//////////////////////////////////////////////////////////////////////
// CSortColumn

class CSortColumn
{
public:
	CSortColumn();
	CSortColumn(CListCtrl* pWnd, const int& nCol, const bool& bIsNumeric);
	virtual ~CSortColumn();
	void Sort(const bool& bAsc);

	CListCtrl* m_pWnd;
	int m_nCol;
	bool m_bIsNumeric;

	static int CALLBACK CompareAscText(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	static int CALLBACK CompareDesText(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	
	static int CALLBACK CompareAscInt(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
	static int CALLBACK CompareDesInt(LPARAM lParam1, LPARAM lParam2, LPARAM lParamSort);
};