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
	CLinkValue(const char* pszLink);
	~CLinkValue();
	
	//
	// Members.
	//
	CString	m_strLink;
	CBuffer	m_oBuffer;
};

/******************************************************************************
**
** Implementation of inline functions.
**
*******************************************************************************
*/

inline CLinkValue::CLinkValue(const char* pszLink)
	: m_strLink(pszLink)
{
	
}

inline CLinkValue::~CLinkValue()
{

}

#endif // LINKVALUE_HPP
