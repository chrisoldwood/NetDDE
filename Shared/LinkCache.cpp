/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		LINKCACHE.CPP
** COMPONENT:	The Application
** DESCRIPTION:	CLinkCache class definition.
**
*******************************************************************************
*/

#include "Common.hpp"
#include "LinkCache.hpp"
#include "LinkValue.hpp"
#include <WCL/StrArray.hpp>
#include <NCL/DDEConv.hpp>
#include <NCL/DDELink.hpp>
#include <Core/Algorithm.hpp>

/******************************************************************************
** Method:		Constructor.
**
** Description:	.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

CLinkCache::CLinkCache()
{
}

/******************************************************************************
** Method:		Destructor.
**
** Description:	.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

CLinkCache::~CLinkCache()
{
	Purge();
}

/******************************************************************************
** Method:		Create()
**
** Description:	Create an entry for the links value.
**
** Parameters:	pConv			The conversation.
**				pLink			The link.
**				pszDefValue		The default link value.
**
** Returns:		The value.
**
*******************************************************************************
*/

CLinkValue* CLinkCache::Create(const CDDEConv* pConv, const CDDELink* pLink, const tchar* pszDefValue)
{
	ASSERT(Find(pConv, pLink) == NULL);

	// Allocate link value.
	CString     strKey = FormatKey(pConv, pLink);
	CLinkValue* pValue = new CLinkValue(strKey);

	// Add to collection.
	m_oLinks[strKey] = pValue;

	// Store inital value, if supplied.
	if (pszDefValue != NULL)
		pValue->m_oLastValue.FromString(pszDefValue, ANSI_TEXT, true);

	return pValue;
}

/******************************************************************************
** Method:		Find()
**
** Description:	Find the value for a link.
**
** Parameters:	pConv	The conversation.
**				pLink	The link.
**
** Returns:		The value or NULL.
**
*******************************************************************************
*/

CLinkValue* CLinkCache::Find(const CDDEConv* pConv, const CDDELink* pLink) const
{
	CLinkValue* defaultValue = nullptr;

	return Core::findOrDefault(m_oLinks, FormatKey(pConv, pLink), defaultValue);
}

/******************************************************************************
** Method:		Purge()
**
** Description:	Clear the entries relating to the conversation from the cache.
**
** Parameters:	pConv	The conversation.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CLinkCache::Purge(const CDDEConv* pConv)
{
	typedef CLinksMap::iterator LinksIter;

	CStrArray astrLinks;

	// Format the cache entry prefix for the conversation.
	CString strPrefix = CString::Fmt(TXT("%s|%s!"), pConv->Service(), pConv->Topic());
	size_t  nLength   = strPrefix.Length();

	// Find all links for the conversation...
	for (LinksIter it = m_oLinks.begin(); it != m_oLinks.end(); ++it)
	{
		const CString& strLink = it->first;

		if (tstrnicmp(strLink, strPrefix, nLength) == 0)
		{
			// Delete value, but remember key.
			astrLinks.Add(strLink);
			delete it->second;
		}
	}

	// Purge all matching links...
	for (size_t i = 0; i < astrLinks.Size(); ++i)
		m_oLinks.erase(astrLinks[i]);
}

/******************************************************************************
** Method:		Purge()
**
** Description:	Clear the entire cache.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CLinkCache::Purge()
{
	typedef CLinksMap::iterator LinksIter;

	for (LinksIter it = m_oLinks.begin(); it != m_oLinks.end(); ++it)
		delete it->second;

	m_oLinks.clear();
}

/******************************************************************************
** Method:		FormatKey()
**
** Description:	Format the map key for the link.
**
** Parameters:	pConv	The conversation.
**				pLink	The link.
**
** Returns:		The key.
**
*******************************************************************************
*/

CString CLinkCache::FormatKey(const CDDEConv* pConv, const CDDELink* pLink)
{
	return CString::Fmt(TXT("%s|%s!%s%u"), pConv->Service(), pConv->Topic(), pLink->Item(), pLink->Format());
}
