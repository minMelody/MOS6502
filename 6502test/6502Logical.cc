// Test 6502 logic instructions

#include "6502Includes.hpp"

namespace MOS6502
{
	namespace UnitTests
	{
		struct Test6502Logical : public testing::Test
		{
			Bus bus;
			FakeMemory mem{ 0x10000 };
			Test6502 cpu{ &bus };
			virtual void SetUp()
			{
				bus.connect(&mem, 0x0000);

				cpu.reset();
				cpu.cycles = 0;
			}
			virtual void TearDown()
			{
			}
		};

		TEST_F(Test6502Logical, ORA)
		{
			std::vector<uint8_t> opcodes =
			{
				0x09, 2,
				0x05, 3,
				0x15, 4,
				0x0D, 4,
				0x1D, 4 + 1,
				0x19, 4 + 1,
				0x01, 6,
				0x11, 5 + 1
			};
			cpu.given(0xBE, 0);

			for (int i = 0; i < opcodes.size() - 1; i += 2)
			{
				cpu.A = 0x88;

				//printf("%02X \n", opcodes[i]);
				cpu.testInstruction(opcodes[i]);

				EXPECT_EQ(cpu.cycles + cpu.getIns().extraCycle, opcodes[i + 1]);
				EXPECT_EQ(cpu.A, 0xBE);
			}
		}

		TEST_F(Test6502Logical, BITtestZeroPage)
		{
			cpu.A = 0xC0;
			cpu.given(0xF0, 0x0004);

			cpu.testInstruction(0x24);

			EXPECT_EQ(cpu.cycles + cpu.getIns().extraCycle, 3);
			EXPECT_FALSE(cpu.getFlag(cpu.Z));
			EXPECT_TRUE(cpu.getFlag(cpu.N));
			EXPECT_TRUE(cpu.getFlag(cpu.V));
		}
		TEST_F(Test6502Logical, BITAbsolute)
		{
			cpu.A = 0xF0;
			cpu.given(0x0F, 0xFF00);

			cpu.testInstruction(0x2C);

			EXPECT_EQ(cpu.cycles + cpu.getIns().extraCycle, 4);
			EXPECT_TRUE(cpu.getFlag(cpu.Z));
			EXPECT_FALSE(cpu.getFlag(cpu.N));
			EXPECT_FALSE(cpu.getFlag(cpu.V));
		}

		TEST_F(Test6502Logical, AND)
		{
			std::vector<uint8_t> opcodes =
			{
				0x29, 2,
				0x25, 3,
				0x35, 4,
				0x2D, 4,
				0x3D, 4 + 1,
				0x39, 4 + 1,
				0x21, 6,
				0x31, 5 + 1
			};
			cpu.given(0x0F, 0);

			for (int i = 0; i < opcodes.size() - 1; i += 2)
			{
				cpu.A = 0xF0;

				//printf("%02X \n", opcodes[i]);
				cpu.testInstruction(opcodes[i]);

				EXPECT_EQ(cpu.cycles + cpu.getIns().extraCycle, opcodes[i + 1]);
				EXPECT_EQ(cpu.A, 0x00);
			}
		}

		TEST_F(Test6502Logical, EOR)
		{
			std::vector<uint8_t> opcodes =
			{
				0x49, 2,
				0x45, 3,
				0x55, 4,
				0x4D, 4,
				0x5D, 4 + 1,
				0x59, 4 + 1,
				0x41, 6,
				0x51, 5 + 1
			};
			cpu.given(0x0F, 0);

			for (int i = 0; i < opcodes.size() - 1; i += 2)
			{
				cpu.A = 0xFF;

				//printf("%02X \n", opcodes[i]);
				cpu.testInstruction(opcodes[i]);

				EXPECT_EQ(cpu.cycles + cpu.getIns().extraCycle, opcodes[i + 1]);
				EXPECT_EQ(cpu.A, 0xF0);
			}
		}
	}
}