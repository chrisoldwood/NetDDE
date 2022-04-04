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
//! Encode a create conversation reply packet.

NetDDEPacketPtr EncodeCreateConversationReplyPacket(uint packetID,
                                                    bool result,
                                                    HCONV conversationHandle,
                                                    uint32 conversationID);

////////////////////////////////////////////////////////////////////////////////
//! Encode a destroy conversation packet.

NetDDEPacketPtr EncodeDestroyConversationPacket(HCONV conversationHandle,
                                                uint32 conversationID);

////////////////////////////////////////////////////////////////////////////////
//! Encode a conversation disconnect packet.

NetDDEPacketPtr EncodeConversationDisconnectPacket(HCONV conversationHandle);

////////////////////////////////////////////////////////////////////////////////
//! Encode an item request packet.

NetDDEPacketPtr EncodeRequestItemPacket(HCONV conversationHandle,
                                        uint32 conversationID,
                                        const tchar* item,
                                        uint format);

////////////////////////////////////////////////////////////////////////////////
//! Encode an item request reply packet.

NetDDEPacketPtr EncodeRequestItemReplyPacket(uint packetID,
                                             bool result,
                                             const CBuffer& data);

////////////////////////////////////////////////////////////////////////////////
//! Encode a start advise packet.

NetDDEPacketPtr EncodeStartAdvisePacket(HCONV conversationHandle,
                                        uint32 conversationID,
                                        const tchar* item,
                                        uint format,
                                        bool asynchronous,
                                        bool requestInitialValue);

////////////////////////////////////////////////////////////////////////////////
//! Encode a start advise reply packet.

NetDDEPacketPtr EncodeStartAdviseReplyPacket(uint packetID,
                                             bool result);

////////////////////////////////////////////////////////////////////////////////
//! Encode an advise start failed packet.

NetDDEPacketPtr EncodeAdviseStartFailedPacket(HCONV conversationHandle,
                                              const CString& item,
                                              uint format);

////////////////////////////////////////////////////////////////////////////////
//! Encode an advise packet.

NetDDEPacketPtr EncodeAdvisePacket(HCONV conversationHandle,
                                   const CString& item,
                                   uint format,
                                   const CBuffer& data);

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
//! Encode an execute command reply packet.

NetDDEPacketPtr EncodeExecuteCommandReplyPacket(uint packetID,
                                                bool result);

////////////////////////////////////////////////////////////////////////////////
//! Encode an item poke packet.

NetDDEPacketPtr EncodePokeItemPacket(HCONV conversationHandle,
                                     uint32 conversationID,
                                     const tchar* item,
                                     uint format,
                                     const CBuffer& data);

////////////////////////////////////////////////////////////////////////////////
//! Encode an item poke reply packet.

NetDDEPacketPtr EncodePokeItemReplyPacket(uint packetID,
                                          bool result);

////////////////////////////////////////////////////////////////////////////////
//! Encode a client connect packet.

NetDDEPacketPtr EncodeClientConnectPacket(const CString& serviceName);

////////////////////////////////////////////////////////////////////////////////
//! Encode a client connect reply packet.

NetDDEPacketPtr EncodeClientConnectReplyPacket(uint packetID,
                                               bool result);

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
