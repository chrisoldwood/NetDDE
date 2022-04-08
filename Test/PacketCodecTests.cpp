////////////////////////////////////////////////////////////////////////////////
//! \file   Pa.cpp
//! \brief  The unit tests for the packet encoding/decoding functions.
//! \author Chris Oldwood

#include "Common.hpp"
#include <Core/UnitTest.hpp>
#include "PacketCodec.hpp"
#include <WCL/MemStream.hpp>

using namespace NetDDE;

namespace
{

size_t calcStringDataSizeInBytes(const tchar* string)
{
	return Core::numBytes<tchar>(tstrlen(string)+1);
}

size_t calcStringFieldLengthInBytes(const tchar* string)
{
	return sizeof(uint32) + calcStringDataSizeInBytes(string);
}

size_t calcStringDataSizeInBytes(const CString& string)
{
	return Core::numBytes<tchar>(string.Capacity());
}

size_t calcStringFieldLengthInBytes(const CString& string)
{
	return sizeof(uint32) + calcStringDataSizeInBytes(string);
}

}

TEST_SET(PacketCodec)
{

TEST_CASE("The client disconnect packet can be encoded")
{
	const CString serviceName(TXT("ServiceName"));
	const CString computerName(TXT("ComputerName"));

	const NetDDEPacketPtr packet = EncodeClientDisconnectPacket(serviceName, computerName);

	TEST_TRUE(packet->DataType() == CNetDDEPacket::NETDDE_CLIENT_DISCONNECT);
	TEST_TRUE(packet->PacketID() != CNetDDEPacket::ASYNC_PACKET_ID);

	const size_t packetDataSize = calcStringFieldLengthInBytes(serviceName)
                                + calcStringFieldLengthInBytes(computerName);
	TEST_TRUE(packet->DataSize() == packetDataSize);

	CBuffer buffer(packet->Buffer());
	CMemStream stream(buffer);
	stream.Open();
	stream.Seek(sizeof(CNetDDEPacket::Header));
	
	uint32 capacity;
	CBuffer string(packet->Buffer().Size());

	stream >> capacity;
	TEST_TRUE(capacity == serviceName.Capacity());
	stream.Read(buffer.Buffer(), calcStringDataSizeInBytes(serviceName));
	TEST_TRUE(serviceName == reinterpret_cast<const tchar*>(buffer.Buffer()));	

	stream >> capacity;
	TEST_TRUE(capacity == computerName.Capacity());
	stream.Read(buffer.Buffer(), calcStringDataSizeInBytes(computerName));
	TEST_TRUE(computerName == reinterpret_cast<const tchar*>(buffer.Buffer()));	

	TEST_TRUE(stream.IsEOF());
	stream.Close();
}
TEST_CASE_END

TEST_CASE("The client disconnect packet can be decoded")
{
	const tchar* serviceNameSent = TXT("ServiceName");
	const tchar* computerNameSent = TXT("ComputerName");

	CBuffer buffer;
	CMemStream stream(buffer);
	stream.Create();

	uint32 capacity = tstrlen(serviceNameSent)+1;
	stream << capacity;
	stream.Write(serviceNameSent, Core::numBytes<tchar>(capacity));
	
	capacity = tstrlen(computerNameSent)+1;
	stream << capacity;
	stream.Write(computerNameSent, Core::numBytes<tchar>(capacity));

	stream.Close();
	NetDDEPacketPtr packet = NetDDEPacketPtr(new CNetDDEPacket(CNetDDEPacket::NETDDE_CLIENT_DISCONNECT, buffer));

	CString serviceNameReceived;
	CString computerNameReceived;

	DecodeClientDisconnectPacket(packet, serviceNameReceived, computerNameReceived);

	TEST_TRUE(serviceNameReceived == serviceNameSent);
	TEST_TRUE(computerNameReceived == computerNameSent);
}
TEST_CASE_END

TEST_CASE("A client disconnect packet can be round-tripped")
{
	const CString serviceNameSent(TXT("ServiceName"));
	const CString computerNameSent(TXT("ComputerName"));

	const NetDDEPacketPtr packet = EncodeClientDisconnectPacket(serviceNameSent, computerNameSent);

	CString serviceNameReceived;
	CString computerNameReceived;

	DecodeClientDisconnectPacket(packet, serviceNameReceived, computerNameReceived);

	TEST_TRUE(serviceNameReceived == serviceNameSent);
	TEST_TRUE(computerNameReceived == computerNameSent);
}
TEST_CASE_END

}
TEST_SET_END
