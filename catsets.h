#pragma once

const int MAX_TEXT_LEN = 1024;

class CBulkRecordsetMod : public CRecordset
{
public:
	CBulkRecordsetMod(CDatabase* pDatabase = 0);
	DECLARE_DYNAMIC(CBulkRecordsetMod)

	virtual BOOL Open(UINT nOpenType = AFX_DB_USE_DEFAULT_TYPE, LPCTSTR lpszSQL = 0, DWORD dwOptions = useMultiRowFetch);
	virtual BOOL RowsetUpdate(WORD wRow, WORD wLockType = SQL_LOCK_NO_CHANGE);
	virtual BOOL RowsetAdd(WORD wRow, WORD wLockType = SQL_LOCK_NO_CHANGE);
	virtual BOOL RowsetDelete(WORD wRow, WORD wLockType = SQL_LOCK_NO_CHANGE);
	BOOL ValidateMod(WORD wRow, WORD wExpectedStatus);
};

class CDynamicBulkSet : public CBulkRecordsetMod
{
public:
	CDynamicBulkSet(CDatabase* pDatabase = 0);
	DECLARE_DYNAMIC(CDynamicBulkSet)

	virtual void Close();

	int m_nAllocatedFields;
	void** m_ppvData;
	void** m_ppvLengths;

	virtual void DoBulkFieldExchange(CFieldExchange* pFX);
	virtual void CheckRowsetError(RETCODE nRetCode);
};

class CColumns : public CRecordset
{
private:
	virtual CString GetDefaultConnect() { return ""; }
	virtual CString GetDefaultSQL() { return ""; }

public:
	CColumns(CDatabase* pDatabase);
	bool Open(LPCSTR pszTableQualifier = 0,
		LPCSTR pszTableOwner = 0,
		LPCSTR pszTableName = 0,
		LPCSTR pszColumnName = 0,
		UINT nOpenType = forwardOnly);

	CString	m_sTableQualifier;
	CString	m_sTableOwner;
	CString	m_sTableName;
	CString	m_sColumnName;
	CString	m_sTypeName;
	CString	m_sRemarks;
	long m_nPrecision;
	long m_nLength;
	int	m_nScale;
	int	m_nDataType;
	int	m_nRadix;
	int	m_fNullable;

	virtual void DoFieldExchange(CFieldExchange*);
};

class CTables : public CRecordset
{
private:
	virtual CString GetDefaultConnect() { return ""; }
	virtual CString GetDefaultSQL() { return ""; }

public:
	CTables(CDatabase* pDatabase);
	bool Open(LPCSTR pszTableQualifier = NULL,
		LPCSTR pszTableOwner = NULL,
		LPCSTR pszTableName = NULL,
		LPCSTR pszTableType = NULL,
		UINT   nOpenType = forwardOnly);

	CString	m_sTableQualifier;
	CString	m_sTableOwner;
	CString	m_sTableName;
	CString	m_sTableType;
	CString	m_sRemarks;

	virtual void DoFieldExchange(CFieldExchange*);
};

class CGetTypeInfo : public CRecordset
{
	virtual CString GetDefaultConnect() { return ""; }
	virtual CString GetDefaultSQL() { return ""; }

public:
	CGetTypeInfo(CDatabase* pDatabase);
	BOOL Open(short fSqlType = SQL_ALL_TYPES, UINT  nOpenType = forwardOnly);
	BOOL m_bCaseSensitive;
	BOOL m_bMoney;
	CString m_strTypeName;
	CString m_strLiteralPrefix;
	CString m_strLiteralSuffix;
	CString m_strCreateParams;
	CString m_strLocalTypeName;
	int m_fNullable;
	int m_fDataType;
	int m_fSearchable;
	int m_fUnsignedAttribute;
	int m_fAutoIncrement;
	int m_nMinimumScale;
	int m_nMaximumScale;
	long m_nPrecision;
	virtual void DoFieldExchange(CFieldExchange*);
};
