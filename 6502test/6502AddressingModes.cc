// Test 6502 addressing modes

#include "pch.h"
#include "Bus.h"
#include "6502.h"
#include "FakeMemory.hpp"

namespace MOS6502
{
	namespace UnitTests
	{
		// Expose protected members and methods to the test environment
		struct Test6502 : m6502
		{
			using m6502::m6502;

			using m6502::cycles;
			uint16_t testACC() { return ACC(false); }
			uint16_t testIMM() { return IMM(false); }
			uint16_t testIMP() { this->IR = bus->read(PC); return IMP(false); }
			uint16_t testREL() { return REL(false); }
			uint16_t testABS() { return ABS(false); }
			uint16_t testZP () { return ZP (false); }
			uint16_t testIND() { return IND(false); }
			uint16_t testZPX() { return ZPX(false); }
			uint16_t testZPY() { return ZPY(false); }
			uint16_t testZIX() { return ZIX(false); }
			uint16_t testABX(bool ec) { return ABX(ec); }
			uint16_t testABY(bool ec) { return ABY(ec); }
			uint16_t testZIY(bool ec) { return ZIY(ec); }
		};

		struct Test6502AdrModes : public testing::Test
		{
			Bus bus;
			FakeMemory mem{ 0x10000 };
			Test6502 cpu{ &bus };

			virtual void SetUp()
			{
				bus.connect(&mem, 0x0000);
				mem.data[0xFFFC] = 0x00;
				mem.data[0xFFFD] = 0x00;
				cpu.reset();
			}
			virtual void TearDown()
			{
			}
		};

		TEST_F(Test6502AdrModes, NonIndexedNonMemoryModes)
		{
			cpu.PC = 0x0000;
			mem.data[0x0000] = 0xea;
			mem.data[0x0001] = 0xad;
			cpu.A = 0xef;

			EXPECT_EQ(cpu.testIMP(), 0xea);
			cpu.PC++;
			EXPECT_EQ(cpu.testACC(), cpu.A);
			EXPECT_EQ(cpu.testIMM(), 0xad);
		}

		TEST_F(Test6502AdrModes, AbsoluteAndZeroPage)
		{
			mem.data[0x0000] = 0xad;
			mem.data[0x0001] = 0x50;

			EXPECT_EQ(cpu.testABS(), 0x50ad);
			cpu.reset();
			EXPECT_EQ(cpu.testZP(), 0x00ad);
		}

		TEST_F(Test6502AdrModes, RelativePositiveOffset)
		{
			mem.data[0x0000] = 0x05;
			Test6502 prevState = cpu;

			EXPECT_EQ(cpu.testREL(), prevState.PC + 6);
		}

		TEST_F(Test6502AdrModes, RelativeNegativeOffset)
		{
			cpu.PC = 0xeaea;
			mem.data[0xeaea] = 0x85;
			Test6502 prevState = cpu;

			EXPECT_EQ(static_cast<int8_t>(0x85), -123); // test cast
			EXPECT_EQ(cpu.testREL(), prevState.PC - 122);
		}

		TEST_F(Test6502AdrModes, Indirect)
		{
			mem.data[0x0000] = 0x00;
			mem.data[0x0001] = 0x31;
			mem.data[0x3100] = 0x50;
			mem.data[0x3101] = 0x99;

			EXPECT_EQ(cpu.testIND(), 0x9950);
		}

		TEST_F(Test6502AdrModes, IndirectCrossBoundary)
		{
			mem.data[0x0000] = 0xFF;
			mem.data[0x0001] = 0x30;
			mem.data[0x3000] = 0x40;
			mem.data[0x30FF] = 0x99;
			mem.data[0x3100] = 0x50;

			EXPECT_EQ(cpu.testIND(), 0x4099);
		}

		TEST_F(Test6502AdrModes, AbsoluteIndexedNoCrossingPageA)
		{
			cpu.cycles = 0;
			mem.data[0x0000] = 0x00;
			mem.data[0x0001] = 0x10;
			cpu.X = 0x09;
			cpu.Y = 0x06;
			// ABX
			EXPECT_EQ(cpu.testABX(false), 0x1009);
			EXPECT_EQ(cpu.cycles, 0);
			// ABY
			cpu.cycles = 0;
			cpu.PC = 0;
			EXPECT_EQ(cpu.testABY(false), 0x1006);
			EXPECT_EQ(cpu.cycles, 0);
		}
		TEST_F(Test6502AdrModes, AbsoluteIndexedNoCrossingPageB)
		{
			cpu.cycles = 0;
			mem.data[0x0000] = 0x00;
			mem.data[0x0001] = 0x10;
			cpu.X = 0x09;
			cpu.Y = 0x06;
			// ABX
			EXPECT_EQ(cpu.testABX(true), 0x1009);
			EXPECT_EQ(cpu.cycles, 0);
			// ABY
			cpu.cycles = 0;
			cpu.PC = 0;
			EXPECT_EQ(cpu.testABY(true), 0x1006);
			EXPECT_EQ(cpu.cycles, 0);
		}
		TEST_F(Test6502AdrModes, AbsoluteIndexedCrossingPageNoExtraCycle)
		{
			cpu.cycles = 0;
			mem.data[0x0000] = 0xFA;
			mem.data[0x0001] = 0x10;
			cpu.X = 0x09;
			cpu.Y = 0x06;
			// ABX
			EXPECT_EQ(cpu.testABX(false), 0x1103);
			EXPECT_EQ(cpu.cycles, 0);
			// ABY
			cpu.cycles = 0;
			cpu.PC = 0;
			EXPECT_EQ(cpu.testABY(false), 0x1100);
			EXPECT_EQ(cpu.cycles, 0);
		}
		TEST_F(Test6502AdrModes, AbsoluteIndexedCrossingPageWithExtraCycle)
		{
			cpu.cycles = 0;
			mem.data[0x0000] = 0xFA;
			mem.data[0x0001] = 0x10;
			cpu.X = 0x09;
			cpu.Y = 0x06;
			// ABX
			EXPECT_EQ(cpu.testABX(true), 0x1103);
			EXPECT_EQ(cpu.cycles, 1);
			// ABY
			cpu.cycles = 0;
			cpu.PC = 0;
			EXPECT_EQ(cpu.testABY(true), 0x1100);
			EXPECT_EQ(cpu.cycles, 1);
		}

		TEST_F(Test6502AdrModes, ZeropageIndexedX)
		{
			mem.data[0x0000] = 0xC0;
			cpu.X = 0x60;

			EXPECT_EQ(cpu.testZPX(), 0x20);
		}
		TEST_F(Test6502AdrModes, ZeropageIndexedY)
		{
			mem.data[0x0000] = 0xC0;
			cpu.Y = 0x09;

			EXPECT_EQ(cpu.testZPY(), 0xC9);
		}

		TEST_F(Test6502AdrModes, IndexedIndirectX)
		{
			mem.data[0x0000] = 0x20;
			mem.data[0x0024] = 0x74;
			mem.data[0x0025] = 0x20;
			cpu.X = 0x04;

			EXPECT_EQ(cpu.testZIX(), 0x2074);
		}
		TEST_F(Test6502AdrModes, IndexedIndirectXwrapAround)
		{
			mem.data[0x0000] = 0xFD;
			mem.data[0x0002] = 0x08;
			mem.data[0x0003] = 0x25;
			cpu.X = 0x05;

			EXPECT_EQ(cpu.testZIX(), 0x2508);
		}

		TEST_F(Test6502AdrModes, IndirectIndexedYnoExtraCycle)
		{
			cpu.cycles = 0;
			mem.data[0x0000] = 0x86;
			mem.data[0x0086] = 0x28;
			mem.data[0x0087] = 0x40;
			cpu.Y = 0x10;

			EXPECT_EQ(cpu.testZIY(true), 0x4038);
			EXPECT_EQ(cpu.cycles, 0);
		}
		TEST_F(Test6502AdrModes, IndirectIndexedYwithExtraCycle)
		{
			cpu.cycles = 0;
			mem.data[0x0000] = 0x86;
			mem.data[0x0086] = 0xF5;
			mem.data[0x0087] = 0x3F;
			cpu.Y = 0x10;

			EXPECT_EQ(cpu.testZIY(true), 0x4005);
			EXPECT_EQ(cpu.cycles, 1);
		}
	}
}