/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		LINKCACHE.HPP
** COMPONENT:	The Application
** DESCRIPTION:	The CLinkCache class declaration.
**
*******************************************************************************
*/

// Check for previous inclusion
#ifndef LINKCACHE_HPP
#define LINKCACHE_HPP

/******************************************************************************
** 
** The cache of link values.
**
*******************************************************************************
*/

class CLinkCache
{
public:
	//
	// Constructors/Destructor.
	//
	CLinkCache();
	~CLinkCache();
	
	//
	// Methods.
	//
	CLinkValue* Create(const CDDEConv* pConv, const CDDELink* pLink, const char* pszDefValue = NULL);
	CLinkValue* Find(const CDDEConv* pConv, const CDDELink* pLink) const;

	void Clear();

protected:
	// Template shorthands.
	typedef TMap<CString, CLinkValue*> CLinksMap;
	typedef TMapIter<CString, CLinkValue*> CLinksIter;

	//
	// Members.
	//
	CLinksMap	m_oLinks;

	//
	// Internal methods.
	//
	static CString FormatKey(const CDDEConv* pConv, const CDDELink* pLink);
};

/******************************************************************************
**
** Implementation of inline functions.
**
*******************************************************************************
*/

#endif // LINKCACHE_HPP
