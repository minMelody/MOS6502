// Test 6502 register instructions

#include "6502Includes.hpp"

namespace MOS6502
{
	namespace UnitTests
	{
		struct Test6502Registers : public testing::Test
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

		// ===== Compare Registers =====

		TEST_F(Test6502Registers, CMP)
		{
			std::vector<uint8_t> opcodes =
			{
				0xC9, 2,
				0xC5, 3,
				0xD5, 4,
				0xCD, 4,
				0xDD, 4 + 1,
				0xD9, 4 + 1,
				0xC1, 6,
				0xD1, 5 + 1
			};

			for (int i = 0; i < opcodes.size() - 1; i += 2)
			{
				uint8_t a = std::rand() & 0xFF;
				uint8_t b = std::rand() & 0xFF;
				cpu.A = a;
				cpu.given(b, cpu.PC);

				cpu.testInstruction(opcodes[i]);

				EXPECT_EQ(cpu.cycles + cpu.getIns().extraCycle, opcodes[i + 1]);
				EXPECT_EQ(cpu.getFlag(cpu.C), a >= b);
				EXPECT_EQ(cpu.getFlag(cpu.Z), a == b);
				EXPECT_EQ(cpu.getFlag(cpu.N), static_cast<int8_t>(a - b) < 0);
			}
		}
		TEST_F(Test6502Registers, CPY)
		{
			std::vector<uint8_t> opcodes =
			{
				0xC0, 2,
				0xC4, 3,
				0xCC, 4
			};

			for (int i = 0; i < opcodes.size() - 1; i += 2)
			{
				uint8_t y = std::rand() & 0xFF;
				uint8_t b = std::rand() & 0xFF;
				cpu.Y = y;
				cpu.given(b, cpu.PC);

				cpu.testInstruction(opcodes[i]);

				EXPECT_EQ(cpu.cycles + cpu.getIns().extraCycle, opcodes[i + 1]);
				EXPECT_EQ(cpu.getFlag(cpu.C), y >= b);
				EXPECT_EQ(cpu.getFlag(cpu.Z), y == b);
				EXPECT_EQ(cpu.getFlag(cpu.N), static_cast<int8_t>(y - b) < 0);
				EXPECT_EQ(cpu.cycles + cpu.getIns().extraCycle, opcodes[i + 1]);
			}
		}
		TEST_F(Test6502Registers, CPX)
		{
			std::vector<uint8_t> opcodes =
			{
				0xE0, 2,
				0xE4, 3,
				0xEC, 4
			};

			for (int i = 0; i < opcodes.size() - 1; i += 2)
			{
				uint8_t x = std::rand() & 0xFF;
				uint8_t b = std::rand() & 0xFF;
				cpu.X = x;
				cpu.given(b, cpu.PC);

				cpu.testInstruction(opcodes[i]);

				EXPECT_EQ(cpu.cycles + cpu.getIns().extraCycle, opcodes[i + 1]);
				EXPECT_EQ(cpu.getFlag(cpu.C), x >= b);
				EXPECT_EQ(cpu.getFlag(cpu.Z), x == b);
				EXPECT_EQ(cpu.getFlag(cpu.N), static_cast<int8_t>(x - b) < 0);
				EXPECT_EQ(cpu.cycles + cpu.getIns().extraCycle, opcodes[i + 1]);
			}
		}

		// ===== Increment and Decrement =====

		TEST_F(Test6502Registers, INC)
		{
			std::vector<uint8_t> opcodes =
			{
				0xE6, 5,
				0xF6, 6,
				0xEE, 6,
				0xFE, 7
			};

			for (int i = 0; i < opcodes.size() - 1; i += 2)
			{
				uint8_t testValue = std::rand() & 0xFF;
				cpu.given(testValue, 0x7000);

				cpu.testInstruction(opcodes[i]);

				EXPECT_EQ(cpu.cycles + cpu.getIns().extraCycle, opcodes[i + 1]);
				EXPECT_EQ(mem.data[0x7000], testValue + 1);
			}
		}
		TEST_F(Test6502Registers, INX)
		{
			cpu.X = 0x7f;

			cpu.testInstruction(0xE8);

			EXPECT_EQ(cpu.cycles + cpu.getIns().extraCycle, 2);
			EXPECT_EQ(cpu.X, 0x80);
			EXPECT_FALSE(cpu.getFlag(cpu.Z));
			EXPECT_TRUE(cpu.getFlag(cpu.N));
		}
		TEST_F(Test6502Registers, INY)
		{
			cpu.Y = 0x7f;

			cpu.testInstruction(0xC8);

			EXPECT_EQ(cpu.cycles + cpu.getIns().extraCycle, 2);
			EXPECT_EQ(cpu.Y, 0x80);
			EXPECT_FALSE(cpu.getFlag(cpu.Z));
			EXPECT_TRUE(cpu.getFlag(cpu.N));
		}

		TEST_F(Test6502Registers, DEC)
		{
			std::vector<uint8_t> opcodes =
			{
				0xC6, 5,
				0xD6, 6,
				0xCE, 6,
				0xDE, 7
			};

			for (int i = 0; i < opcodes.size() - 1; i += 2)
			{
				uint8_t testValue = std::rand() & 0xFF;
				cpu.given(testValue, 0x7000);

				cpu.testInstruction(opcodes[i]);

				EXPECT_EQ(cpu.cycles + cpu.getIns().extraCycle, opcodes[i + 1]);
				EXPECT_EQ(mem.data[0x7000], testValue - 1);
			}
		}
		TEST_F(Test6502Registers, DEX)
		{
			cpu.X = 0x81;

			cpu.testInstruction(0xCA);

			EXPECT_EQ(cpu.cycles + cpu.getIns().extraCycle, 2);
			EXPECT_EQ(cpu.X, 0x80);
			EXPECT_FALSE(cpu.getFlag(cpu.Z));
			EXPECT_TRUE(cpu.getFlag(cpu.N));
		}
		TEST_F(Test6502Registers, DEY)
		{
			cpu.Y = 0x01;

			cpu.testInstruction(0x88);

			EXPECT_EQ(cpu.cycles + cpu.getIns().extraCycle, 2);
			EXPECT_EQ(cpu.Y, 0x00);
			EXPECT_TRUE(cpu.getFlag(cpu.Z));
			EXPECT_FALSE(cpu.getFlag(cpu.N));
		}

		// ===== Transfer Registers =====
		TEST_F(Test6502Registers, TAX)
		{
			cpu.A = 0xF5;

			cpu.testInstruction(0xAA);
			EXPECT_EQ(cpu.cycles + cpu.getIns().extraCycle, 2);
			EXPECT_EQ(cpu.X, cpu.A);
		}
		TEST_F(Test6502Registers, TAY)
		{
			cpu.A = 0xF5;

			cpu.testInstruction(0xA8);
			EXPECT_EQ(cpu.cycles + cpu.getIns().extraCycle, 2);
			EXPECT_EQ(cpu.Y, cpu.A);
		}
		TEST_F(Test6502Registers, TXA)
		{
			cpu.X = 0xB2;

			cpu.testInstruction(0x8A);

			EXPECT_EQ(cpu.cycles + cpu.getIns().extraCycle, 2);
			EXPECT_EQ(cpu.A, cpu.X);
		}
		TEST_F(Test6502Registers, TYA)
		{
			cpu.Y = 0x9A;

			cpu.testInstruction(0x98);

			EXPECT_EQ(cpu.cycles + cpu.getIns().extraCycle, 2);
			EXPECT_EQ(cpu.A, cpu.Y);
		}
		TEST_F(Test6502Registers, TXS)
		{
			cpu.SP = 0x0B;

			cpu.testInstruction(0xBA);

			EXPECT_EQ(cpu.cycles + cpu.getIns().extraCycle, 2);
			EXPECT_EQ(cpu.X, cpu.SP);
		}
		TEST_F(Test6502Registers, TSX)
		{
			cpu.X = 0xB2;

			cpu.testInstruction(0x9A);

			EXPECT_EQ(cpu.cycles + cpu.getIns().extraCycle, 2);
			EXPECT_EQ(cpu.SP, cpu.X);
		}

		// ===== Load Registers =====

		TEST_F(Test6502Registers, LDA)
		{
			std::vector<uint8_t> opcodes =
			{
				0xA9, 2,
				0xA5, 3,
				0xB5, 4,
				0xAD, 4,
				0xBD, 4 + 1,
				0xB9, 4 + 1,
				0xA1, 6,
				0xB1, 5 + 1
			};

			for (int i = 0; i < opcodes.size() - 1; i += 2)
			{
				uint8_t expected = std::rand() & 0xFF;
				cpu.given(expected, cpu.PC);

				//printf("%02X\n", opcodes[i]);
				cpu.testInstruction(opcodes[i]);

				EXPECT_EQ(cpu.cycles + cpu.getIns().extraCycle, opcodes[i + 1]);
				EXPECT_EQ(cpu.A, expected);
			}
		}
		TEST_F(Test6502Registers, LDX)
		{
			std::vector<uint8_t> opcodes =
			{
				0xA2, 2,
				0xA6, 3,
				0xB6, 4,
				0xAE, 4,
				0xBE, 4 + 1
			};

			for (int i = 0; i < opcodes.size() - 1; i += 2)
			{
				uint8_t expected = std::rand() & 0xFF;
				cpu.given(expected, cpu.PC);

				cpu.testInstruction(opcodes[i]);

				EXPECT_EQ(cpu.cycles + cpu.getIns().extraCycle, opcodes[i + 1]);
				EXPECT_EQ(cpu.X, expected);
			}
		}
		TEST_F(Test6502Registers, LDY)
		{
			std::vector<uint8_t> opcodes =
			{
				0xA0, 2,
				0xA4, 3,
				0xB4, 4,
				0xAC, 4,
				0xBC, 4 + 1
			};

			for (int i = 0; i < opcodes.size() - 1; i += 2)
			{
				uint8_t expected = std::rand() & 0xFF;
				cpu.given(expected, cpu.PC);

				cpu.testInstruction(opcodes[i]);

				EXPECT_EQ(cpu.cycles + cpu.getIns().extraCycle, opcodes[i + 1]);
				EXPECT_EQ(cpu.Y, expected);
			}
		}

		// ===== Store Registers =====

		TEST_F(Test6502Registers, STA)
		{
			std::vector<uint8_t> opcodes =
			{
				0x85, 3,
				0x95, 4,
				0x8D, 4,
				0x9D, 5,
				0x99, 5,
				0x81, 6,
				0x91, 6
			};
			cpu.given(0, 0x0085);

			for (int i = 0; i < opcodes.size() - 1; i += 2)
			{
				cpu.A = std::rand() & 0xFF;

				//printf("%02X: %02X\n", opcodes[i], cpu.A);
				cpu.testInstruction(opcodes[i]);

				EXPECT_EQ(cpu.cycles + cpu.getIns().extraCycle, opcodes[i + 1]);
				EXPECT_EQ(mem.data[0x0085], cpu.A);
			}
		}
		TEST_F(Test6502Registers, STX)
		{
			std::vector<uint8_t> opcodes =
			{
				0x86, 3,
				0x96, 4,
				0x8E, 4
			};
			cpu.given(0, 0x0086);

			for (int i = 0; i < opcodes.size() - 1; i += 2)
			{
				cpu.X = std::rand() & 0xFF;

				//printf("%02X: %02X\n", opcodes[i], cpu.X);
				cpu.testInstruction(opcodes[i]);

				EXPECT_EQ(cpu.cycles + cpu.getIns().extraCycle, opcodes[i + 1]);
				EXPECT_EQ(mem.data[0x0086], cpu.X);
			}
		}
		TEST_F(Test6502Registers, STY)
		{
			std::vector<uint8_t> opcodes =
			{
				0x84, 3,
				0x94, 4,
				0x8C, 4
			};
			cpu.given(0, 0x0087);

			for (int i = 0; i < opcodes.size() - 1; i += 2)
			{
				cpu.Y = std::rand() & 0xFF;

				//printf("%02X: %02X\n", opcodes[i], cpu.Y);
				cpu.testInstruction(opcodes[i]);

				EXPECT_EQ(cpu.cycles + cpu.getIns().extraCycle, opcodes[i + 1]);
				EXPECT_EQ(mem.data[0x0087], cpu.Y);
			}
		}

		// ===== Status Register =====

		TEST_F(Test6502Registers, SetAndClearCarryFlag)
		{
			uint8_t expected = cpu.SR | cpu.C;
			cpu.testInstruction(0x38);
			EXPECT_EQ(cpu.cycles, 2);
			EXPECT_EQ(cpu.SR, expected);

			expected = cpu.SR & (~cpu.C);
			cpu.testInstruction(0x18);
			EXPECT_EQ(cpu.cycles, 2);
			EXPECT_EQ(cpu.SR, expected);
		}
		TEST_F(Test6502Registers, SetAndClearInterruptFlag)
		{
			uint8_t expected = cpu.SR | cpu.I;
			cpu.testInstruction(0x78);
			EXPECT_EQ(cpu.cycles, 2);
			EXPECT_EQ(cpu.SR, expected);

			expected = cpu.SR & (~cpu.I);
			cpu.testInstruction(0x58);
			EXPECT_EQ(cpu.cycles, 2);
			EXPECT_EQ(cpu.SR, expected);
		}
		TEST_F(Test6502Registers, SetAndClearDecimalFlag)
		{
			uint8_t expected = cpu.SR | cpu.D;
			cpu.testInstruction(0xF8);
			EXPECT_EQ(cpu.cycles, 2);
			EXPECT_EQ(cpu.SR, expected);

			expected = cpu.SR & (~cpu.D);
			cpu.testInstruction(0xD8);
			EXPECT_EQ(cpu.cycles, 2);
			EXPECT_EQ(cpu.SR, expected);
		}
		TEST_F(Test6502Registers, ClearOverflowFlag)
		{
			cpu.setFlag(cpu.V, 1);
			uint8_t expected = cpu.SR & (~cpu.V);

			cpu.testInstruction(0xB8);
			EXPECT_EQ(cpu.cycles, 2);
			EXPECT_EQ(cpu.SR, expected);
		}
	}
}