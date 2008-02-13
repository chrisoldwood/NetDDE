/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		LINKVALUE.HPP
** COMPONENT:	The Application
** DESCRIPTION:	The CLinkValue class declaration.
**
*******************************************************************************
*/

// Check for previous inclusion
#ifndef LINKVALUE_HPP
#define LINKVALUE_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include <WCL/DateTime.hpp>
#include <WCL/Buffer.hpp>

/******************************************************************************
** 
** The class used to store an links last value.
**
*******************************************************************************
*/

class CLinkValue
{
public:
	//
	// Constructors/Destructor.
	//
	CLinkValue(const tchar* pszLink);
	~CLinkValue();
	
	//
	// Members.
	//
	CString		m_strLink;			// Link cache key.
	CDateTime	m_tLastUpdate;		// Time of last update.
	CBuffer		m_oLastValue;		// Last update value.
};

/******************************************************************************
**
** Implementation of inline functions.
**
*******************************************************************************
*/

inline CLinkValue::CLinkValue(const tchar* pszLink)
	: m_strLink(pszLink)
	, m_tLastUpdate(CDateTime::Min())
{
	
}

inline CLinkValue::~CLinkValue()
{

}

#endif // LINKVALUE_HPP
