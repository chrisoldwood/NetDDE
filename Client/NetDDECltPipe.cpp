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
** Method:		WaitForPacket()
**
** Description:	Wait for a specific packet type.
**				NB: All other packets types are appended to a queue.
**
** Parameters:	oPacket		The packet recieved.
**				nType		The type of packet expected.
**
** Returns:		Nothing.
**
*******************************************************************************
*/

void CNetDDECltPipe::WaitForPacket(CNetDDEPacket& oPacket, uint nType)
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

			// Append to queue.
			m_aoPackets.Add(new CNetDDEPacket(oPacket));
		}

		::Sleep(0);
	}
}
