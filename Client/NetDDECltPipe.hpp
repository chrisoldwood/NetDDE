/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		NETDDECLTPIPE.HPP
** COMPONENT:	The Application
** DESCRIPTION:	The CNetDDECltPipe class declaration.
**
*******************************************************************************
*/

// Check for previous inclusion
#ifndef NETDDECLTPIPE_HPP
#define NETDDECLTPIPE_HPP

/******************************************************************************
** 
** The client end of a NetDDE pipe.
**
*******************************************************************************
*/

class CNetDDECltPipe : public CClientPipe, public CNetDDEPipe
{
public:
	//
	// Constructors/Destructor.
	//
	CNetDDECltPipe();
	~CNetDDECltPipe();
	
	//
	// Methods.
	//
	void           WaitForPacket(CNetDDEPacket& oPacket, uint nType);
	CNetDDEPacket* ReadNotifyPacket();

protected:
	// Template shorthands.
	typedef TPtrArray<CNetDDEPacket> CPackets;

	//
	// Members.
	//
	CPackets	m_aoPackets;	// The received packet queue.
};

/******************************************************************************
**
** Implementation of inline functions.
**
*******************************************************************************
*/

#endif // NETDDECLTPIPE_HPP
