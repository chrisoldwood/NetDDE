/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		LINKCACHE.CPP
** COMPONENT:	The Application
** DESCRIPTION:	CLinkCache class definition.
**
*******************************************************************************
*/

#include "AppHeaders.hpp"

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
	Clear();
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
** Method:		Clear()
**
** Description:	Clear the cache.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CLinkCache::Clear()
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
	CString strKey;

	strKey.Format("%s%s%s%u", pConv->Service(), pConv->Topic(), pLink->Item(), pLink->Format());

	return strKey;
}
