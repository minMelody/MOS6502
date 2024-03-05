// Test 6502 rotate and shift instructions

#include "6502Includes.hpp"

namespace MOS6502
{
	namespace UnitTests
	{
		struct Test6502Shift : public testing::Test
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

		TEST_F(Test6502Shift, ASL)
		{
			std::vector<uint8_t> opcodes =
			{
				0x0A, 2,
				0x06, 5,
				0x16, 6,
				0x0E, 6,
				0x1E, 7
			};
			cpu.given(0x80, 0x0015);
			cpu.A = 0x80;

			for (int i = 0; i < opcodes.size() - 1; i += 2)
			{
				//printf("%02X %d \n", opcodes[i], opcodes[i + 1]);
				cpu.testInstruction(opcodes[i]);

				EXPECT_EQ(cpu.cycles + cpu.getIns().extraCycle, opcodes[i + 1]);
				if (opcodes[i] == 0x0A) EXPECT_EQ(cpu.A, 0x00);
				else EXPECT_EQ(mem.data[0x0015], 0x00);
				EXPECT_TRUE(cpu.getFlag(Test6502::Z));
				EXPECT_FALSE(cpu.getFlag(Test6502::N));
				EXPECT_TRUE(cpu.getFlag(Test6502::C));
			}
		}

		TEST_F(Test6502Shift, LSR)
		{
			std::vector<uint8_t> opcodes =
			{
				0x4A, 2,
				0x46, 5,
				0x56, 6,
				0x4E, 6,
				0x5E, 7
			};
			cpu.given(0x01, 0x0015);
			cpu.A = 0x01;

			for (int i = 0; i < opcodes.size() - 1; i += 2)
			{
				//printf("%02X %d \n", opcodes[i], opcodes[i + 1]);
				cpu.testInstruction(opcodes[i]);

				EXPECT_EQ(cpu.cycles + cpu.getIns().extraCycle, opcodes[i + 1]);
				if (opcodes[i] == 0x4A) EXPECT_EQ(cpu.A, 0x00);
				else EXPECT_EQ(mem.data[0x0015], 0x00);
				EXPECT_TRUE(cpu.getFlag(Test6502::Z));
				EXPECT_FALSE(cpu.getFlag(Test6502::N));
				EXPECT_TRUE(cpu.getFlag(Test6502::C));
			}
		}

		TEST_F(Test6502Shift, ROL)
		{
			std::vector<uint8_t> opcodes =
			{
				0x2A, 2,
				0x26, 5,
				0x36, 6,
				0x2E, 6,
				0x3E, 7
			};
			cpu.given(0x88, 0x0015);
			cpu.A = 0x88;
			cpu.setFlag(cpu.C, 1);

			for (int i = 0; i < opcodes.size() - 1; i += 2)
			{
				//printf("%02X %d \n", opcodes[i], opcodes[i + 1]);
				cpu.testInstruction(opcodes[i]);

				EXPECT_EQ(cpu.cycles + cpu.getIns().extraCycle, opcodes[i + 1]);
				if (opcodes[i] == 0x2A) EXPECT_EQ(cpu.A, 0x11);
				else EXPECT_EQ(mem.data[0x0015], 0x11);
				EXPECT_FALSE(cpu.getFlag(Test6502::Z));
				EXPECT_FALSE(cpu.getFlag(Test6502::N));
				EXPECT_TRUE(cpu.getFlag(Test6502::C));
			}
		}

		TEST_F(Test6502Shift, ROR)
		{
			std::vector<uint8_t> opcodes =
			{
				0x6A, 2,
				0x66, 5,
				0x76, 6,
				0x6E, 6,
				0x7E, 7
			};
			cpu.given(0x88, 0x0015);
			cpu.A = 0x88;

			for (int i = 0; i < opcodes.size() - 1; i += 2)
			{
				//printf("%02X %d \n", opcodes[i], opcodes[i+1]);
				cpu.testInstruction(opcodes[i]);

				EXPECT_EQ(cpu.cycles + cpu.getIns().extraCycle, opcodes[i + 1]);
				if (opcodes[i] == 0x6A) EXPECT_EQ(cpu.A, 0x44);
				else EXPECT_EQ(mem.data[0x0015], 0x44);
				EXPECT_FALSE(cpu.getFlag(Test6502::Z));
				EXPECT_FALSE(cpu.getFlag(Test6502::N));
				EXPECT_FALSE(cpu.getFlag(Test6502::C));
			}
		}
	}
}