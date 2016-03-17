#include "stdafx.h"
#include "catsets.h"

CColumns::CColumns(CDatabase* pDatabase): CRecordset(pDatabase) {
	m_sTableQualifier = _T("");
	m_sTableOwner = _T("");
	m_sTableName = _T("");
	m_sColumnName = _T("");
	m_nDataType = 0;
	m_sTypeName = _T("");
	m_nPrecision = 0;
	m_nLength = 0;
	m_nScale = 0;
	m_nRadix = 0;
	m_fNullable = 0;
	m_sRemarks = _T("");
	m_nFields = 12;
}

bool CColumns::Open(
	LPCSTR pszTableQualifier,
	LPCSTR pszTableOwner,
	LPCSTR pszTableName,
	LPCSTR pszColumnName,
	UINT nOpenType) {
	RETCODE	nRetCode;
	UWORD bFunctionExists;

	AFX_SQL_SYNC(::SQLGetFunctions(m_pDatabase->m_hdbc, SQL_API_SQLCOLUMNS, &bFunctionExists));
	if(!Check(nRetCode) || !bFunctionExists) {
		if(!bFunctionExists)
			TRACE(_T("SQLColumns not supported\n"));
		return false;
	}

	SetState(nOpenType, NULL, readOnly);
	if(!AllocHstmt())
		return false;

	try {
		OnSetOptions(m_hstmt);
		AllocStatusArrays();

		AFX_ODBC_CALL(::SQLColumns(
			m_hstmt,
			(UCHAR FAR*)pszTableQualifier,
			(SWORD) SQL_NTS,
			(UCHAR FAR*)pszTableOwner,
			(SWORD) SQL_NTS,
			(UCHAR FAR*)pszTableName,
			(SWORD) SQL_NTS,
			(UCHAR FAR*)pszColumnName,
			(SWORD) SQL_NTS));

		if(!Check(nRetCode))
			ThrowDBException(nRetCode, m_hstmt);
		AllocAndCacheFieldInfo();
		AllocRowset();
		MoveNext();
		m_bBOF = m_bEOF;
	}

	catch(CDBException &e) {
		AfxMessageBox(e.m_strError, MB_ICONEXCLAMATION);
		Close();
		throw;
	}
	return true;
}

void CColumns::DoFieldExchange(CFieldExchange* pFX) {
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Text(pFX, _T("TABLE_QUALIFIER"), m_sTableQualifier);
	RFX_Text(pFX, _T("TABLE_OWNER"), m_sTableOwner);
	RFX_Text(pFX, _T("TABLE_NAME"), m_sTableName);
	RFX_Text(pFX, _T("COLUMN_NAME"), m_sColumnName);
	RFX_Int(pFX, _T("DATA_TYPE"), m_nDataType);
	RFX_Text(pFX, _T("TYPE_NAME"), m_sTypeName);
	RFX_Long(pFX, _T("PRECISION"), m_nPrecision);
	RFX_Long(pFX, _T("LENGTH"), m_nLength);
	RFX_Int(pFX, _T("SCALE"), m_nScale);
	RFX_Int(pFX, _T("RADIX"), m_nRadix);
	RFX_Int(pFX, _T("NULLABLE"), m_fNullable);
	RFX_Text(pFX, _T("REMARKS"), m_sRemarks);
}

CTables::CTables(CDatabase* pDatabase): CRecordset(pDatabase) {
	m_sTableQualifier = _T("");
	m_sTableOwner = _T("");
	m_sTableName = _T("");
	m_sTableType = _T("");
	m_sRemarks = _T("");
	m_nFields = 5;
}

bool CTables::Open(
	LPCSTR pszTableQualifier,
	LPCSTR pszTableOwner,
	LPCSTR pszTableName,
	LPCSTR pszTableType,
	UINT nOpenType) {
	RETCODE	nRetCode;
	UWORD bFunctionExists;

	AFX_SQL_SYNC(::SQLGetFunctions(m_pDatabase->m_hdbc, SQL_API_SQLTABLES, &bFunctionExists));

	if(!Check(nRetCode) || !bFunctionExists) {
		if(!bFunctionExists)
			TRACE(_T("SQLTables not supported\n"));
		return false;
	}
	SetState(nOpenType, NULL, readOnly);
	if(!AllocHstmt())
		return false;

	try {
		OnSetOptions(m_hstmt);
		AllocStatusArrays();
		AFX_ODBC_CALL(::SQLTables(
			m_hstmt,
			(UCHAR FAR*)pszTableQualifier,
			(SWORD) SQL_NTS,
			(UCHAR FAR*)pszTableOwner,
			(SWORD) SQL_NTS,
			(UCHAR FAR*)pszTableName,
			(SWORD) SQL_NTS,
			(UCHAR FAR*)pszTableType,
			(SWORD) SQL_NTS));

		if(!Check(nRetCode))
			ThrowDBException(nRetCode, m_hstmt);

		AllocAndCacheFieldInfo();
		AllocRowset();
		MoveNext();
		m_bBOF = m_bEOF;
	}
	catch(CDBException &e) {
		AfxMessageBox(e.m_strError, MB_ICONEXCLAMATION);
		Close();
		throw;
	}
	return true;
}

void CTables::DoFieldExchange(CFieldExchange* pFX) {
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Text(pFX, _T("TABLE_QUALIFIER"), m_sTableQualifier);
	RFX_Text(pFX, _T("TABLE_OWNER"), m_sTableOwner);
	RFX_Text(pFX, _T("TABLE_NAME"), m_sTableName);
	RFX_Text(pFX, _T("TABLE_TYPE"), m_sTableType);
	RFX_Text(pFX, _T("REMARKS"), m_sRemarks);
}

IMPLEMENT_DYNAMIC(CBulkRecordsetMod, CRecordset)

CBulkRecordsetMod::CBulkRecordsetMod(CDatabase* pdb): CRecordset(pdb) {
	m_nDefaultType = dynaset;
}

BOOL CBulkRecordsetMod::Open(UINT nOpenType, LPCTSTR lpszSQL, DWORD dwOptions) {
	ASSERT(dwOptions & useMultiRowFetch);
	return CRecordset::Open(nOpenType, lpszSQL, dwOptions);
}

BOOL CBulkRecordsetMod::RowsetUpdate(WORD wRow, WORD wLockType) {
	ASSERT(wRow >= 0 && wRow <= GetRowsetSize());

	RETCODE nRetCode;
	AFX_ODBC_CALL(::SQLSetPos(m_hstmt, wRow, SQL_UPDATE, wLockType));

	return ValidateMod(wRow, SQL_ROW_UPDATED);
}

BOOL CBulkRecordsetMod::RowsetAdd(WORD wRow, WORD wLockType) {
	ASSERT(wRow >= 0 && wRow <= GetRowsetSize() + 1);

	RETCODE nRetCode;
	AFX_ODBC_CALL(::SQLSetPos(m_hstmt, wRow, SQL_ADD, wLockType));

	return ValidateMod(wRow, SQL_ROW_ADDED);
}

BOOL CBulkRecordsetMod::RowsetDelete(WORD wRow, WORD wLockType) {
	ASSERT(wRow >= 0 && wRow <= GetRowsetSize());

	RETCODE nRetCode;
	AFX_ODBC_CALL(::SQLSetPos(m_hstmt, wRow, SQL_DELETE, wLockType));

	return ValidateMod(wRow, SQL_ROW_DELETED);
}

BOOL CBulkRecordsetMod::ValidateMod(WORD wRow, WORD wExpectedStatus) {
	BOOL bReturn = TRUE;

	if(wRow != 0)
		bReturn = GetRowStatus(wRow) == wExpectedStatus;
	else {
		for(WORD wNum = 1; wNum <= GetRowsetSize(); wNum++) {
			if(GetRowStatus(wNum) != wExpectedStatus)
				bReturn = FALSE;
		}
	}

	return bReturn;
}

IMPLEMENT_DYNAMIC(CDynamicBulkSet, CBulkRecordsetMod)

CDynamicBulkSet::CDynamicBulkSet(CDatabase* pdb)
	: CBulkRecordsetMod(pdb) {
	m_nDefaultType = dynaset;
	m_nAllocatedFields = 0;
	m_ppvData = 0;
	m_ppvLengths = 0;
}

void CDynamicBulkSet::Close() {
	CRecordset::Close();

	delete[] m_ppvData;
	delete[] m_ppvLengths;
}

void CDynamicBulkSet::DoBulkFieldExchange(CFieldExchange* pFX) {
	if(pFX->m_nOperation == CFieldExchange::AllocMultiRowBuffer && m_nAllocatedFields == 0) {
		m_nFields = m_nAllocatedFields = GetODBCFieldCount();
		m_ppvData = new void*[m_nFields];
		memset(m_ppvData, 0, sizeof(void*) * m_nFields);
		m_ppvLengths = new void*[m_nFields];
		memset(m_ppvLengths, 0, sizeof(void*) * m_nFields);
	}

	ASSERT(m_nAllocatedFields != 0);

	pFX->SetFieldType(CFieldExchange::outputColumn);
	for(int nNum = 0; nNum < GetODBCFieldCount(); nNum++) {
		RFX_Text_Bulk(pFX, (LPCTSTR) ("Dummy"), (LPSTR*) &m_ppvData[nNum], (LONG_PTR**) &m_ppvLengths[nNum], MAX_TEXT_LEN);
	}
}

void CDynamicBulkSet::CheckRowsetError(RETCODE nRetCode) {
	if(nRetCode == SQL_SUCCESS_WITH_INFO) {
		CDBException e(nRetCode);
		e.BuildErrorString(m_pDatabase, m_hstmt, FALSE);

		if(e.m_strStateNativeOrigin.Find(_T("State:01004")) >= 0) {
		}

		else if(e.m_strStateNativeOrigin.Find(_T("State:01S01")) >= 0) {
			e.Empty();
			TRACE0("Error: fetching row from server.\n");
			ThrowDBException(AFX_SQL_ERROR_ROW_FETCH);
		}
		else {
#ifdef _DEBUG
			if(afxTraceFlags & traceDatabase) {
				TRACE0("Warning: ODBC Success With Info, ");
				e.TraceErrorMessage(e.m_strError);
				e.TraceErrorMessage(e.m_strStateNativeOrigin);
			}
#endif // _DEBUG
		}
	}
	else if(!Check(nRetCode))
		ThrowDBException(nRetCode);
}

/////////////////////////////////////////////////////////////////////////////
// CGetTypeInfo

CGetTypeInfo::CGetTypeInfo(CDatabase* pDatabase)
	: CRecordset(pDatabase) {
	m_strTypeName = _T("");
	m_fDataType = 0;
	m_nPrecision = 0;
	m_strLiteralPrefix = _T("");
	m_strLiteralSuffix = _T("");
	m_strCreateParams = _T("");
	m_fNullable = 0;
	m_bCaseSensitive = 0;
	m_fSearchable = 0;
	m_fUnsignedAttribute = 0;
	m_bMoney = 0;
	m_fAutoIncrement = 0;
	m_strLocalTypeName = _T("");
	m_nMinimumScale = 0;
	m_nMaximumScale = 0;
	m_nFields = 15;
}

BOOL CGetTypeInfo::Open(short fSqlType, UINT nOpenType) {
	RETCODE nRetCode;
	UWORD   bFunctionExists;

	AFX_SQL_SYNC(::SQLGetFunctions(m_pDatabase->m_hdbc, SQL_API_SQLGETTYPEINFO, &bFunctionExists));
	if(!Check(nRetCode) || !bFunctionExists) {
		if(!bFunctionExists)
			TRACE(_T("SQLGetTypeInfo not supported\n"));
		return FALSE;
	}

	SetState(nOpenType, NULL, readOnly);
	if(!AllocHstmt())
		return FALSE;

	TRY
	{
		OnSetOptions(m_hstmt);
		AllocStatusArrays();

		AFX_SQL_ASYNC(this,::SQLGetTypeInfo(m_hstmt,fSqlType));
		if(!Check(nRetCode))
			ThrowDBException(nRetCode,m_hstmt);

		AllocAndCacheFieldInfo();
		AllocRowset();

		MoveNext();
		m_bBOF = m_bEOF;
	}
		CATCH_ALL(e) {
		Close();
		THROW_LAST();
	}
	END_CATCH_ALL

		return TRUE;
}

void CGetTypeInfo::DoFieldExchange(CFieldExchange* pFX) {
	pFX->SetFieldType(CFieldExchange::outputColumn);
	RFX_Text(pFX, _T("TYPE_NAME"), m_strTypeName);
	RFX_Int(pFX, _T("DATA_TYPE"), m_fDataType);
	RFX_Long(pFX, _T("PRECISION"), m_nPrecision);
	RFX_Text(pFX, _T("LITERAL_PREFIX"), m_strLiteralPrefix);
	RFX_Text(pFX, _T("LITERAL_SUFFIX"), m_strLiteralSuffix);
	RFX_Text(pFX, _T("CREATE_PARAMS"), m_strCreateParams);
	RFX_Int(pFX, _T("NULLABLE"), m_fNullable);
	RFX_Int(pFX, _T("CASE_SENSITIVE"), m_bCaseSensitive);
	RFX_Int(pFX, _T("SEARCHABLE"), m_fSearchable);
	RFX_Int(pFX, _T("UNSIGNED_ATTRIBUTE"), m_fUnsignedAttribute);
	RFX_Int(pFX, _T("MONEY"), m_bMoney);
	RFX_Int(pFX, _T("AUTO_INCREMENT"), m_fAutoIncrement);
	RFX_Text(pFX, _T("LOCAL_TYPE_NAME"), m_strLocalTypeName);
	RFX_Int(pFX, _T("MINIMUM_SCALE"), m_nMinimumScale);
	RFX_Int(pFX, _T("MAXIMUM_SCALE"), m_nMaximumScale);
}
