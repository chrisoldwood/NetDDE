/******************************************************************************
** (C) Chris Oldwood
**
** MODULE:		NETDDEPIPE.HPP
** COMPONENT:	The Application
** DESCRIPTION:	The CNetDDEPipe class declaration.
**
*******************************************************************************
*/

// Check for previous inclusion
#ifndef NETDDEPIPE_HPP
#define NETDDEPIPE_HPP

/******************************************************************************
** 
** The mixin class for NetDDE client/server pipes.
**
*******************************************************************************
*/

class CNetDDEPipe
{
public:
	//
	// Constructors/Destructor.
	//
	CNetDDEPipe(CNamedPipe* pPipe);
	virtual ~CNetDDEPipe();
	
	//
	// Methods.
	//
	void SendPacket(const CNetDDEPacket& oPacket);
	bool RecvPacket(CNetDDEPacket& oPacket);

protected:
	//
	// Members.
	//
	CNamedPipe&	m_oPipe;

	// Disallow copies for now.
	CNetDDEPipe(const CNetDDEPipe&);
	void operator=(const CNetDDEPipe&);
};

/******************************************************************************
**
** Implementation of inline functions.
**
*******************************************************************************
*/

inline CNetDDEPipe::CNetDDEPipe(CNamedPipe* pPipe)
	: m_oPipe(*pPipe)
{
}

inline CNetDDEPipe::~CNetDDEPipe()
{
}

inline void CNetDDEPipe::SendPacket(const CNetDDEPacket& oPacket)
{
	m_oPipe.Write(oPacket.Buffer());
}

inline bool CNetDDEPipe::RecvPacket(CNetDDEPacket& oPacket)
{
	uint nAvail = m_oPipe.Available();

	// Enough data to read packet header?
	if (nAvail >= sizeof(CNetDDEPacket::Header))
	{
		// Set buffer size to header only.
		oPacket.Buffer().Size(sizeof(CNetDDEPacket::Header));

		// Peek at packet header.
		uint nRead = m_oPipe.Peek(oPacket.Buffer(), oPacket.Buffer().Size());

		// Peeked entire header?
		if (nRead == sizeof(CNetDDEPacket::Header))
		{
			// Calculate full packet size.
			uint nPktSize = oPacket.DataSize() + sizeof(CNetDDEPacket::Header);

			// Entire packet buffered?
			if (nAvail >= nPktSize)
			{
				oPacket.Buffer().Size(nPktSize);

				// Read entire packet.
				m_oPipe.Read(oPacket.Buffer());
				return true;
			}
		}
	}

	return false;
}

#endif // NETDDEPIPE_HPP
