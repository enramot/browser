// DSNCollection.h: interface for the CDSNCollection class.
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

#ifndef DSNCOLLECTION_H
#define DSNCOLLECTION_H

#define DSNCOLLECTION_BUFLEN 128

#if _MSC_VER > 1000
#pragma once
#endif // _MSC_VER > 1000

#include <afxdb.h> //Needed for SQLxxx defines and definitions.

class CDSNCollection  
{
public:
	// Constants used.
	static const unsigned char s_SYSTEM;
	static const unsigned char s_USER;

	// Constructors & destructors.
	CDSNCollection();
	virtual ~CDSNCollection();

protected:
	SQLHENV m_hSQLEnvironment;
	CString m_sErrorMsg;
	SQLRETURN m_iSQLResult;

	// Helper function.
	SQLRETURN CallSQLDataSources(CString& sName, SQLUSMALLINT direction);

public:
	// Get the first DSN. ucType should be s_SYSTEM, 
	// s_USER or a combination of both (s_SYSTEM|s_USER)
	// which is default.
	// Returns empty strings upon failure.
	CString GetFirstDSNName(unsigned char ucType = (s_SYSTEM | s_USER));
	
	// Get the next DSN in the enumeration sequence.
	// Returns empty strings upon failure.
	CString GetNextDSNName();

	// Retrieve the las error message (a formated string)
	CString GetErrorMessage() { return m_sErrorMsg; }

	// Retrieve the last result code from a SQL operation.
	SQLRETURN GetLastSQLResult() { return m_iSQLResult; }
};

#endif