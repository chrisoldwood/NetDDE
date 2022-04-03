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
//! Encode a create conversation packet.

NetDDEPacketPtr EncodeCreateConversationPacket(const CString& serviceName,
                                               const tchar* topic);

////////////////////////////////////////////////////////////////////////////////
//! Encode a destroy conversation packet.

NetDDEPacketPtr EncodeDestroyConversationPacket(HCONV conversationHandle,
                                                uint32 conversationID);

////////////////////////////////////////////////////////////////////////////////
//! Encode an item request packet.

NetDDEPacketPtr EncodeRequestItemPacket(HCONV conversationHandle,
                                        uint32 conversationID,
                                        const tchar* item,
                                        uint format);

////////////////////////////////////////////////////////////////////////////////
//! Encode a start advise packet.

NetDDEPacketPtr EncodeStartAdvisePacket(HCONV conversationHandle,
                                        uint32 conversationID,
                                        const tchar* item,
                                        uint format,
                                        bool asynchronous,
                                        bool requestInitialValue);

////////////////////////////////////////////////////////////////////////////////
//! Encode a stop advise packet.

NetDDEPacketPtr EncodeStopAdvisePacket(HCONV conversationHandle,
                                       uint32 conversationID,
                                       const CString& item,
                                       uint format);

////////////////////////////////////////////////////////////////////////////////
//! Encode an execute command packet.

NetDDEPacketPtr EncodeExecuteCommandPacket(HCONV conversationHandle,
                                           uint32 conversationID,
                                           const CString& command);

////////////////////////////////////////////////////////////////////////////////
//! Encode an item poke packet.

NetDDEPacketPtr EncodePokeItemPacket(HCONV conversationHandle,
                                     uint32 conversationID,
                                     const tchar* item,
                                     uint format,
                                     const CBuffer& data);

////////////////////////////////////////////////////////////////////////////////
//! Encode a client connect packet.

NetDDEPacketPtr EncodeClientConnectPacket(const CString& serviceName);

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
