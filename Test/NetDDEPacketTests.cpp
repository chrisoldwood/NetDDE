////////////////////////////////////////////////////////////////////////////////
//! \file   NetDDEPacketTests.cpp
//! \brief  The unit tests for the CNetDDEPacket class.
//! \author Chris Oldwood

#include "Common.hpp"
#include <Core/UnitTest.hpp>
#include "NetDDEPacket.hpp"

namespace Tests
{

void SetNextPktId(uint id)
{
	CNetDDEPacket::s_nNextPktID = id;
}

}

TEST_SET(CNetDDEPacket)
{

TEST_CASE("A notification packet only needs to contain the packet type")
{
	const uint type = CNetDDEPacket::NETDDE_SERVER_DISCONNECT;
	CNetDDEPacket packet(type);

	TEST_TRUE(packet.DataType() == type);
	TEST_TRUE(packet.DataSize() == 0);
	TEST_TRUE(packet.PacketID() == CNetDDEPacket::ASYNC_PACKET_ID);
}
TEST_CASE_END

TEST_CASE("A sync packet can contain just the packet type and sequence number")
{
	const uint type = CNetDDEPacket::DDE_EXECUTE;
	CNetDDEPacket packet(type);

	TEST_TRUE(packet.DataType() == type);
	TEST_TRUE(packet.DataSize() == 0);
	TEST_TRUE(packet.PacketID() != CNetDDEPacket::ASYNC_PACKET_ID);
}
TEST_CASE_END

TEST_CASE("The packet sequence number changes with each new packet")
{
	CNetDDEPacket packet1(CNetDDEPacket::DDE_EXECUTE);
	CNetDDEPacket packet2(CNetDDEPacket::DDE_EXECUTE);

	TEST_TRUE(packet1.PacketID() != packet2.PacketID());
}
TEST_CASE_END

TEST_CASE("The packet sequence number wraps, skipping ASYNC_PACKET_ID and 0")
{
	Tests::SetNextPktId(CNetDDEPacket::ASYNC_PACKET_ID - 2);
	CNetDDEPacket packet1(CNetDDEPacket::DDE_EXECUTE);
	CNetDDEPacket packet2(CNetDDEPacket::DDE_EXECUTE);
	CNetDDEPacket packet3(CNetDDEPacket::DDE_EXECUTE);

	TEST_TRUE( (packet1.PacketID() != packet2.PacketID()) && (packet2.PacketID() != packet3.PacketID()) );
	TEST_TRUE( (packet1.PacketID() != CNetDDEPacket::ASYNC_PACKET_ID) && (packet1.PacketID() != 0) );
	TEST_TRUE( (packet2.PacketID() != CNetDDEPacket::ASYNC_PACKET_ID) && (packet2.PacketID() != 0) );
	TEST_TRUE( (packet3.PacketID() != CNetDDEPacket::ASYNC_PACKET_ID) && (packet3.PacketID() != 0) );
}
TEST_CASE_END

TEST_CASE("A packet can be copied")
{
	const uint packetID = 0x12341234; 
	const uint value = 0x42424242;
	CBuffer buffer(&value, sizeof(value));

	{
	CNetDDEPacket original(CNetDDEPacket::DDE_EXECUTE, packetID, buffer);

	CNetDDEPacket copy(original);

	TEST_TRUE(copy.DataType() == original.DataType());
	TEST_TRUE(copy.PacketID() == original.PacketID());
	TEST_TRUE(copy.DataSize() == original.DataSize());
	TEST_TRUE(memcmp(copy.DataBuffer(), original.DataBuffer(), sizeof(value)) == 0);
	}

	{
	CNetDDEPacket original(CNetDDEPacket::DDE_EXECUTE, packetID, buffer);

	CNetDDEPacket copy; copy = original;

	TEST_TRUE(copy.DataType() == original.DataType());
	TEST_TRUE(copy.PacketID() == original.PacketID());
	TEST_TRUE(copy.DataSize() == original.DataSize());
	TEST_TRUE(memcmp(copy.DataBuffer(), original.DataBuffer(), sizeof(value)) == 0);
	}
}
TEST_CASE_END

}
TEST_SET_END
