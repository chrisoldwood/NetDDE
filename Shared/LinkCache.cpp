/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		LINKCACHE.CPP
** COMPONENT:	The Application
** DESCRIPTION:	CLinkCache class definition.
**
*******************************************************************************
*/

#include "NetDDEShared.hpp"

#ifdef _DEBUG
// For memory leak detection.
#define new DBGCRT_NEW
#endif

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
	// Expect around 1000 links.
	m_oLinks.Reserve(1000);
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

CLinkValue* CLinkCache::Create(const CDDEConv* pConv, const CDDELink* pLink, const char* pszDefValue)
{
	ASSERT(Find(pConv, pLink) == NULL);

	// Allocate link value.
	CString     strKey = FormatKey(pConv, pLink);
	CLinkValue* pValue = new CLinkValue(strKey);

	// Add to collection.
	m_oLinks.Add(strKey, pValue);

	// Store inital value, if supplied.
	if (pszDefValue != NULL)
		pValue->m_oLastValue.FromString(pszDefValue);

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
	CLinkValue* pValue = NULL;

	// Fetch link data from cache.
	m_oLinks.Find(FormatKey(pConv, pLink), pValue);

	return pValue;
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
	CStrArray astrLinks;

	// Format the cache entry prefix for the conversation.
	CString strPrefix = CString::Fmt("%s|%s!", pConv->Service(), pConv->Topic());
	int     nLength   = strPrefix.Length();

	CString		strLink;
	CLinkValue* pLinkValue = NULL;
	CLinksIter	oIter(m_oLinks);

	// Find all links for the conversation...
	while (oIter.Next(strLink, pLinkValue))
	{
		if (strnicmp(strLink, strPrefix, nLength) == 0)
		{
			// Delete value, but remember key.
			astrLinks.Add(strLink);
			delete pLinkValue;
		}
	}

	// Purge all matching links...
	for (int i = 0; i < astrLinks.Size(); ++i)
		m_oLinks.Remove(astrLinks[i]);
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
	CString		strLink;
	CLinkValue* pLinkValue = NULL;
	CLinksIter	oIter(m_oLinks);

	while (oIter.Next(strLink, pLinkValue))
		delete pLinkValue;

	m_oLinks.RemoveAll();
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
	return CString::Fmt("%s|%s!%s%u", pConv->Service(), pConv->Topic(), pLink->Item(), pLink->Format());
}
