////////////////////////////////////////////////////////////////////////////////
//! \file   PacketCodec.hpp
//! \brief  Functions for encoding and decoding packets.
//! \author Chris Oldwood

// Check for previous inclusion
#ifndef APP_PACKETCODEC_HPP
#define APP_PACKETCODEC_HPP

#if _MSC_VER > 1000
#pragma once
#endif

#include "NetDDEPacket.hpp"

namespace NetDDE
{

////////////////////////////////////////////////////////////////////////////////
//! Encode a client disconnect packet.

NetDDEPacketPtr EncodeClientDisconnectPacket(const CString& serviceName,
                                             const CString& computerName);

////////////////////////////////////////////////////////////////////////////////
//! Decode a client disconnect packet.

void DecodeClientDisconnectPacket(CNetDDEPacket& packet,
                                  CString& serviceName,
                                  CString& computerName);

//namespace NetDDE
}

#endif // APP_PACKETCODEC_HPP
