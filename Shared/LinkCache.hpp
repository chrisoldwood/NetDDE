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

#include <Legacy/TMap.hpp>
#include <Legacy/TMapIter.hpp>

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
	uint Size() const;

	CLinkValue* Create(const CDDEConv* pConv, const CDDELink* pLink, const char* pszDefValue = NULL);
	CLinkValue* Find(const CDDEConv* pConv, const CDDELink* pLink) const;

	void Purge(const CDDEConv* pConv);
	void Purge();

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

inline uint CLinkCache::Size() const
{
	return m_oLinks.Count();
}

#endif // LINKCACHE_HPP
