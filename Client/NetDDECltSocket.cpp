/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		NETDDECLTSOCKET.CPP
** COMPONENT:	The Application
** DESCRIPTION:	CNetDDECltSocket class definition.
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

CNetDDECltSocket::CNetDDECltSocket(CNetDDEService* pService)
	: CTCPCltSocket(ASYNC)
	, CNetDDESocket(this)
	, m_pService(pService)
	, m_nTimeOut(App.m_nNetTimeOut)
{
	ASSERT(pService != NULL);
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

CNetDDECltSocket::~CNetDDECltSocket()
{
	Close();
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

void CNetDDECltSocket::Close()
{
	// Forward to base class.
	CTCPCltSocket::Close();

	// Cleanup packet queue.
	m_aoPackets.DeleteAll();
}

/******************************************************************************
** Method:		ReadResponsePacket()
**
** Description:	Read the response packet, waiting if required.
**				NB: All other packets types are appended to a queue.
**
** Parameters:	oPacket		The packet recieved.
**				nType		The type of packet expected.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDECltSocket::ReadResponsePacket(CNetDDEPacket& oPacket, uint nType)
{
	DWORD dwStartTime = ::GetTickCount();

	// Until timed-out...
	while ((::GetTickCount() - dwStartTime) < m_nTimeOut)
	{
		// Any response packets received?
		if (m_aoPackets.Size())
		{
			ASSERT(m_aoPackets.Size() == 1);

			// Find our response.
			for (int i = 0; i < m_aoPackets.Size(); ++i)
			{
				if (m_aoPackets[i]->DataType() == nType)
				{
					oPacket = *m_aoPackets[i];

					m_aoPackets.Delete(i);

					return;
				}
			}
		}

		// Handle any socket messages.
		CWinSock::ProcessSocketMsgs();

		::Sleep(1);
	}

	throw CSocketException(CSocketException::E_RECV_FAILED, WAIT_TIMEOUT);
}
