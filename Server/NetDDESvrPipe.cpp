/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		NETDDESVRPIPE.CPP
** COMPONENT:	The Application
** DESCRIPTION:	CNetDDESvrPipe class definition.
**
*******************************************************************************
*/

#include "AppHeaders.hpp"

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

CNetDDESvrPipe::CNetDDESvrPipe()
	: CNetDDEPipe(this)
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

CNetDDESvrPipe::~CNetDDESvrPipe()
{
	Close();

	// Cleanup.
	m_aoLinks.RemoveAll();
	m_aoConvs.RemoveAll();
}

/******************************************************************************
** Method:		AddConversation()
**
** Description:	Adds the conversation to the list used by this connection.
**
** Parameters:	pConv	The conversation.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDESvrPipe::AddConversation(CDDECltConv* pConv)
{
	ASSERT(pConv != NULL);

	m_aoConvs.Add(pConv);
}

/******************************************************************************
** Method:		RemoveConversation()
**
** Description:	Removes the conversation from the list used by this connection.
**				NB: It also removes all links associated with the conversation.
**
** Parameters:	pConv	The conversation.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDESvrPipe::RemoveConversation(CDDECltConv* pConv)
{
	ASSERT(pConv != NULL);
	ASSERT(UsesConversation(pConv));

	RemoveAllConvLinks(pConv);

	// Remove the conversation.
	m_aoConvs.Remove(m_aoConvs.Find(pConv));
}

/******************************************************************************
** Method:		UsesConversation()
**
** Description:	Queries if this connection uses the conversation.
**
** Parameters:	pConv	The conversation.
**
** Returns:		true or false.
**
*******************************************************************************
*/

bool CNetDDESvrPipe::UsesConversation(CDDECltConv* pConv)
{
	ASSERT(pConv != NULL);

	return (m_aoConvs.Find(pConv) != -1);
}

/******************************************************************************
** Method:		GetConversations()
**
** Description:	Gets the list of conversations used by the connection.
**
** Parameters:	aoConvs		The array to return the list.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDESvrPipe::GetConversations(CConvs& aoConvs)
{
	for (int i = 0; i < m_aoConvs.Size(); ++i)
		aoConvs.Add(m_aoConvs[i]);
}

/******************************************************************************
** Method:		AddLink()
**
** Description:	Adds the link to the list used by this connection.
**
** Parameters:	pLink	The link.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDESvrPipe::AddLink(CDDELink* pLink)
{
	ASSERT(pLink != NULL);

	m_aoLinks.Add(pLink);
}

/******************************************************************************
** Method:		RemoveLink()
**
** Description:	Removes the link from the list used by this connection.
**
** Parameters:	pLink	The link.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDESvrPipe::RemoveLink(CDDELink* pLink)
{
	ASSERT(pLink != NULL);
	ASSERT(UsesLink(pLink));

	m_aoLinks.Remove(m_aoLinks.Find(pLink));
}

/******************************************************************************
** Method:		RemoveAllConvLinks()
**
** Description:	Removes all links from the list used by the conversation.
**
** Parameters:	pConv	The conversation.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDESvrPipe::RemoveAllConvLinks(CDDECltConv* pConv)
{
	ASSERT(pConv != NULL);
	ASSERT(UsesConversation(pConv));

	// Remove all links associated with conversation.
	for (int i = m_aoLinks.Size()-1; i >= 0; --i)
	{
		CDDELink* pLink = m_aoLinks[i];

		if (pLink->Conversation() == pConv)
			m_aoLinks.Remove(i);
	}
}

/******************************************************************************
** Method:		UsesLink()
**
** Description:	Queries if this connection uses the link.
**
** Parameters:	pLink	The link.
**
** Returns:		true or false.
**
*******************************************************************************
*/

bool CNetDDESvrPipe::UsesLink(CDDELink* pLink)
{
	ASSERT(pLink != NULL);

	return (m_aoLinks.Find(pLink) != -1);
}

/******************************************************************************
** Method:		GetLinks()
**
** Description:	Gets the list of links used by the connection.
**
** Parameters:	aoLinks		The array to return the list.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDESvrPipe::GetLinks(CLinks& aoLinks)
{
	for (int i = 0; i < m_aoLinks.Size(); ++i)
		aoLinks.Add(m_aoLinks[i]);
}
