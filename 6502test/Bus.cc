#include "pch.h"
#include "Bus.h"
#include "RAM.hpp"
#include "ROM.hpp"

namespace MOS6502
{
	namespace UnitTests
	{
		// Tests bus connectivity, read and write functions.
		// Along with core IAddressable functions
		struct BusTest : public testing::Test
		{
			Bus bus;
			RAM ram{};
			ROM rom{};
			virtual void SetUp()
			{
				bus.connect(&ram, 0x0000);
				bus.connect(&rom, 0x8000);
			}
			virtual void TearDown()
			{
			}
		};

		TEST_F(BusTest, ConnectBus)
		{
			EXPECT_EQ(ram.start, 0x0000);
			EXPECT_EQ(ram.end, 0x5FFF);
			EXPECT_EQ(ram.size(), 0x6000);

			EXPECT_EQ(rom.start, 0x8000);
			EXPECT_EQ(rom.end, 0xFFFF);
			EXPECT_EQ(rom.size(), 0x8000);
		}

		TEST_F(BusTest, BusTryReadFromEmptyAddressSpace)
		{
			EXPECT_THROW(bus.read(0x7050), std::range_error);
		}

		TEST_F(BusTest, BusCanReadFromIAddressable)
		{
			// Read from RAM
			ram.write(0x0065, 0xff);
			EXPECT_EQ(bus.read(0x0065), 0xff);
			// Read from ROM
			EXPECT_EQ(rom.read(0x1180), 0xea);
			EXPECT_EQ(bus.read(0x9180), 0xea);
		}

		TEST_F(BusTest, BusCanWriteInRAM)
		{
			// Try to write through the bus
			bus.write(0x0065, 0xea);
			EXPECT_EQ(ram.read(0x0065), 0xea);
		}

		TEST_F(BusTest, ShouldNotBeAbleToWriteInROM)
		{
			EXPECT_THROW(rom.write(0xF000, 0x00), std::runtime_error);
		}
	}
}