/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		NETDDECLTPIPE.CPP
** COMPONENT:	The Application
** DESCRIPTION:	CNetDDECltPipe class definition.
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

CNetDDECltPipe::CNetDDECltPipe()
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

CNetDDECltPipe::~CNetDDECltPipe()
{
	Close();

	// Clenup packet queue.
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

void CNetDDECltPipe::ReadResponsePacket(CNetDDEPacket& oPacket, uint nType)
{
	// Until read.
	for(;;)
	{
		// Any packet received?
		if (RecvPacket(oPacket))
		{
			// The packet we're after?
			if (oPacket.DataType() == nType)
				return;

			ASSERT(oPacket.DataType() >= CNetDDEPacket::NETDDE_SERVER_DISCONNECT);

			// Append to notification queue.
			m_aoPackets.Add(new CNetDDEPacket(oPacket));
		}

		::Sleep(1);
	}
}

/******************************************************************************
** Method:		ReadNotifyPacket()
**
** Description:	Gets the next notification packet either from the packet queue
**				or by reading directly from the pipe.
**
** Parameters:	None.
**
** Returns:		A packet or NULL.
**
*******************************************************************************
*/

bool CNetDDECltPipe::ReadNotifyPacket(CNetDDEPacket& oPacket)
{
	// Packet queue not empty.
	if (m_aoPackets.Size())
	{
		oPacket = *m_aoPackets[0];

		m_aoPackets.Delete(0);

		return true;
	}
	// Check pipe.
	else if (Available() > 0)
	{
		// Try and read a packet.
		if (RecvPacket(oPacket))
			return true;
	}

	return false;
}
