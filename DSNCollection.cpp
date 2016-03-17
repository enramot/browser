// DSNCollection.cpp: implementation of the CDSNCollection class.
//
// Author:  Emil Gustafsson (e@ntier.se)
//          NTier Solutions (www.ntier.se)
// Created: 2000-06-14
//
// Description:
//			This class wraps a few calls to ODBC in order to collect
//			a set of DSNs.
//
//////////////////////////////////////////////////////////////////////

#include "stdafx.h"
#include "DSNCollection.h"

#ifdef _DEBUG
#undef THIS_FILE
static char THIS_FILE[] = __FILE__;
#define new DEBUG_NEW
#endif

/*static */const unsigned char CDSNCollection::s_SYSTEM = 0x01;
/*static */const unsigned char CDSNCollection::s_USER = 0x02;

//////////////////////////////////////////////////////////////////////
// Construction/Destruction
//////////////////////////////////////////////////////////////////////

CDSNCollection::CDSNCollection()
	: m_sErrorMsg("")
	, m_iSQLResult(SQL_SUCCESS)
	, m_hSQLEnvironment(SQL_NULL_HANDLE) {
	// Initialize an environment handle.
	m_iSQLResult = SQLAllocHandle(SQL_HANDLE_ENV,
								  SQL_NULL_HANDLE,
								  &m_hSQLEnvironment);
	if(m_iSQLResult != SQL_SUCCESS) {
		m_sErrorMsg = "SQLAllocHandle failed";
		return;
	}

	// Must set the ODBC version we wish to use...
	// Otherwise you will get an error.
	m_iSQLResult = SQLSetEnvAttr(m_hSQLEnvironment,
								 SQL_ATTR_ODBC_VERSION,
								 (SQLPOINTER) SQL_OV_ODBC3,
								 0); //this parameter is ignored when setting ODBC version.
	if(m_iSQLResult != SQL_SUCCESS) {
		m_sErrorMsg = "SQLSetEnvAttr failed to set ODBC version";
		return;
	}
}

CDSNCollection::~CDSNCollection() {
	// Must release the environment handle.
	m_iSQLResult = SQLFreeHandle(SQL_HANDLE_ENV, m_hSQLEnvironment);
}

SQLRETURN CDSNCollection::CallSQLDataSources(CString& sName, SQLUSMALLINT direction) {
	SQLSMALLINT dataLen1, dataLen2;
	SQLCHAR sTemp[DSNCOLLECTION_BUFLEN];
	SQLCHAR sDesc[DSNCOLLECTION_BUFLEN];

	// This call enumerates the DSNs.
	m_iSQLResult = SQLDataSources(m_hSQLEnvironment,
								  direction,
								  sTemp,
								  DSNCOLLECTION_BUFLEN,
								  &dataLen1,
								  sDesc,
								  DSNCOLLECTION_BUFLEN,
								  &dataLen2);

	switch(m_iSQLResult) {
		case SQL_SUCCESS:
		case SQL_SUCCESS_WITH_INFO:
			sName = sTemp;
			break;
		case SQL_NO_DATA:
			// This is not an error but rather information that there
			// are no more DSNs to enumerate.
			m_sErrorMsg = "No more DSNs to enumerate";
			break;
		case SQL_ERROR:
		{
			SQLCHAR sState[10];
			SQLINTEGER iNativeError;
			SQLCHAR sMsg[DSNCOLLECTION_BUFLEN];
			(void) SQLGetDiagRec(SQL_HANDLE_ENV,
								 m_hSQLEnvironment,
								 1,
								 sState,
								 &iNativeError,
								 sMsg,
								 DSNCOLLECTION_BUFLEN,
								 &dataLen2);
			m_sErrorMsg.Format("SQL_ERROR: %s:%s", sState, sMsg);
		}
		break;
		case SQL_INVALID_HANDLE:
			m_sErrorMsg = "Invalid Handle";
			break;
		default:
			m_sErrorMsg = "Unknown ERROR";
	}

	return m_iSQLResult;
}

CString CDSNCollection::GetFirstDSNName(unsigned char ucType /* = (s_SYSTEM | s_USER)*/) {
	CString sResult = "";
	if(ucType != s_SYSTEM &&
	   ucType != s_USER &&
	   ucType != (s_SYSTEM | s_USER)) {
		m_sErrorMsg = "Unknown DSN type";
		m_iSQLResult = SQL_ERROR;
	}
	else {
		SQLUSMALLINT direction = SQL_FETCH_FIRST;
		if(ucType == s_SYSTEM)
			direction = SQL_FETCH_FIRST_SYSTEM;
		else if(ucType == s_USER)
			direction = SQL_FETCH_FIRST_USER;
		if(SQL_SUCCESS != CallSQLDataSources(sResult, direction)) {
			//TODO: maybe do something here...
		}
	}

	return sResult;
}

CString CDSNCollection::GetNextDSNName() {
	CString sResult = "";
	if(SQL_SUCCESS != CallSQLDataSources(sResult, SQL_FETCH_NEXT)) {
		//TODO: maybe do something here...
	}

	return sResult;
}
