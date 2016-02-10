/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		NETDDESVRSOCKET.CPP
** COMPONENT:	The Application
** DESCRIPTION:	CNetDDESvrSocket class definition.
**
*******************************************************************************
*/

#include "Common.hpp"
#include "NetDDESvrSocket.hpp"
#include "NetDDEConv.hpp"
#include <NCL/DDECltConv.hpp>
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

CNetDDESvrSocket::CNetDDESvrSocket()
	: CTCPCltSocket(ASYNC)
	, CNetDDESocket(this)
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

CNetDDESvrSocket::~CNetDDESvrSocket()
{
	Close();

	// Cleanup.
	Core::deleteAll(m_aoNetConvs);
}

/******************************************************************************
** Method:		Close()
**
** Description:	Close the socket.
**
** Parameters:	None.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDESvrSocket::Close()
{
	// Forward to base class.
	CTCPCltSocket::Close();
}

/******************************************************************************
** Method:		FindNetConv()
**
** Description:	Find the NetDDE conversation by its DDE conversation and ID.
**
** Parameters:	pConv	The DDE conversation.
**				nConvID	The conversation ID.
**
** Returns:		The NetDDE conversation or nullptr.
**
*******************************************************************************
*/

CNetDDEConv* CNetDDESvrSocket::FindNetConv(CDDECltConv* pConv, uint32 nConvID) const
{
	// For all conversations...
	for (size_t j = 0; j < m_aoNetConvs.size(); ++j)
	{
		CNetDDEConv* pNetConv = m_aoNetConvs[j];

		// Matches?
		if ((pNetConv->m_pSvrConv == pConv) && (pNetConv->m_nSvrConvID == nConvID))
			return pNetConv;
	}

	return nullptr;
}

/******************************************************************************
** Method:		IsLinkUsed()
**
** Description:	Checks if the link is referenced by any of the connections'
**				conversations.
**
** Parameters:	pLink	The DDE link.
**
** Returns:		true or false.
**
*******************************************************************************
*/

bool CNetDDESvrSocket::IsLinkUsed(CDDELink* pLink) const
{
	CDDEConv* pConv = pLink->Conversation();

	// For all conversations...
	for (size_t i = 0; i < m_aoNetConvs.size(); ++i)
	{
		CNetDDEConv* pNetConv = m_aoNetConvs[i];

		// Ignore, if different conversation.
		if (pNetConv->m_pSvrConv != pConv)
			continue;

		// Conversation references link?
		if (Core::exists(pNetConv->m_aoLinks, pLink))
			return true;
	}

	return false;
}
