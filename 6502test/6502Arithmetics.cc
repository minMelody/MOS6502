// Test 6502 add and substract instructions

#include "6502Includes.hpp"

namespace MOS6502
{
	namespace UnitTests
	{
		struct Test6502ADCSBC : public testing::Test
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

		// ======== ADD =====

		TEST_F(Test6502ADCSBC, ADC)
		{
			std::vector<uint8_t> opcodes =
			{
				0x69, 2,
				0x65, 3,
				0x75, 4,
				0x6D, 4,
				0x7D, 4 + 1,
				0x79, 4 + 1,
				0x61, 6,
				0x71, 5 + 1
			};
			cpu.setFlag(cpu.D, 0);

			for (int i = 0; i < opcodes.size() - 1; i += 2)
			{
				uint8_t a = std::rand() & 0xFF;
				uint8_t b = std::rand() & 0xFF;
				uint16_t expected = a + b;
				cpu.A = a;
				cpu.given(b, 0);

				cpu.setFlag(cpu.C, 0);

				//printf("%02X: 0x22+%02X=%02X\n", opcodes[i], a, expected);
				cpu.testInstruction(opcodes[i]);

				EXPECT_EQ(cpu.cycles + cpu.getIns().extraCycle, opcodes[i + 1]);
				EXPECT_EQ(cpu.A, (expected & 0x00FF));
			}
		}
		TEST_F(Test6502ADCSBC, ADCWithZero)
		{
			cpu.A = 0x05;
			cpu.given(0xFB, 0);

			cpu.testInstruction(0x6D);

			EXPECT_EQ(cpu.cycles + cpu.getIns().extraCycle, 4);
			EXPECT_EQ(cpu.A, 0x00);
			EXPECT_FALSE(cpu.getFlag(Test6502::N));
			EXPECT_TRUE(cpu.getFlag(Test6502::Z));
			EXPECT_TRUE(cpu.getFlag(Test6502::C));
			EXPECT_FALSE(cpu.getFlag(Test6502::V));
		}
		TEST_F(Test6502ADCSBC, ADCWithOverflow)
		{
			cpu.A = 0x78;
			cpu.given(0x10, 0);

			cpu.testInstruction(0x6D);

			EXPECT_EQ(cpu.cycles + cpu.getIns().extraCycle, 4);
			EXPECT_EQ(cpu.A, 0x88);
			EXPECT_TRUE(cpu.getFlag(Test6502::N));
			EXPECT_FALSE(cpu.getFlag(Test6502::Z));
			EXPECT_FALSE(cpu.getFlag(Test6502::C));
			EXPECT_TRUE(cpu.getFlag(Test6502::V));
		}
		TEST_F(Test6502ADCSBC, ADCDecimal)
		{
			cpu.setFlag(cpu.D, 1);
			cpu.A = 0x09;
			cpu.given(0x09, 0);

			cpu.testInstruction(0x65);

			EXPECT_EQ(cpu.cycles + cpu.getIns().extraCycle, 3);
			EXPECT_EQ(cpu.A, 0x18);

			cpu.setFlag(cpu.D, 1);
			for (int i = 0; i < 100; i++)
			{
				uint8_t a = ((std::rand() % 10) << 4) | (std::rand() % 10);
				uint8_t b = ((std::rand() % 10) << 4) | (std::rand() % 10);
				cpu.A = a;
				cpu.given(b, 0);
				uint8_t expected = (a >> 4) * 10 + (a & 0x0F);
				expected += (b >> 4) * 10 + (b & 0x0F);

				cpu.setFlag(cpu.C, 0);
				cpu.testInstruction(0x65);
				//printf("%02X+%02X=%d => $%02X %c\n", a, b, expected, cpu.A, cpu.getFlag(cpu.C) ? 'C' : ' ');

				EXPECT_EQ(cpu.cycles + cpu.getIns().extraCycle, 3);

				uint8_t result = (cpu.A >> 4) * 10 + (cpu.A & 0x0F);
				EXPECT_EQ(result, expected % 100);
				EXPECT_EQ(cpu.getFlag(cpu.C), expected > 99);
			}
		}

		// ======== Substract =====

		TEST_F(Test6502ADCSBC, SBC)
		{
			std::vector<uint8_t> opcodes =
			{
				0xE9, 2,
				0xE5, 3,
				0xF5, 4,
				0xED, 4,
				0xFD, 4+1,
				0xF9, 4+1,
				0xE1, 6,
				0xF1, 5+1
			};
			cpu.setFlag(cpu.D, 0);

			for (int i = 0; i < opcodes.size() - 1; i += 2)
			{
				uint8_t a = std::rand() & 0xFF;
				uint8_t b = std::rand() & 0xFF;
				uint16_t expected = a - b;
				cpu.A = a;
				cpu.given(b, 0);

				cpu.setFlag(cpu.C, 1);

				//printf("%02X: 0x22-%02X=%02X\n", opcodes[i], a, expected);
				cpu.testInstruction(opcodes[i]);

				EXPECT_EQ(cpu.cycles + cpu.getIns().extraCycle, opcodes[i + 1]);
				EXPECT_EQ(cpu.A, (expected & 0x00FF));
				EXPECT_EQ(cpu.getFlag(cpu.C), a >= b);	// Carry flag is cleared when a true borrow occurs
				EXPECT_EQ(cpu.getFlag(cpu.Z), cpu.A == 0);
				EXPECT_EQ(cpu.getFlag(cpu.N), cpu.A > 0x80);
			}
		}
		TEST_F(Test6502ADCSBC, SBCWithZero)
		{
			cpu.A = 0x05;
			cpu.given(0x05, 0);

			cpu.setFlag(cpu.C, 1);
			cpu.testInstruction(0xE9);

			EXPECT_EQ(cpu.cycles + cpu.getIns().extraCycle, 2);
			EXPECT_EQ(cpu.A, 0x00);
			EXPECT_FALSE(cpu.getFlag(Test6502::N));
			EXPECT_TRUE(cpu.getFlag(Test6502::Z));
			EXPECT_TRUE(cpu.getFlag(Test6502::C));
			EXPECT_FALSE(cpu.getFlag(Test6502::V));
		}
		TEST_F(Test6502ADCSBC, SBCWithOverflow)
		{
			cpu.A = 0xF6;
			cpu.given(0x78, 0);

			cpu.setFlag(cpu.C, 1);
			cpu.testInstruction(0xE9);

			EXPECT_EQ(cpu.cycles + cpu.getIns().extraCycle, 2);
			EXPECT_EQ(cpu.A, 0x7E);
			EXPECT_FALSE(cpu.getFlag(Test6502::N));
			EXPECT_FALSE(cpu.getFlag(Test6502::Z));
			EXPECT_TRUE(cpu.getFlag(Test6502::C));
			EXPECT_TRUE(cpu.getFlag(Test6502::V));
		}
		TEST_F(Test6502ADCSBC, SBCDecimal)
		{
			cpu.setFlag(cpu.D, 1);
			for (int i = 0; i < 100; i++)
			{
				uint8_t a = ((std::rand() % 10) << 4) | (std::rand() % 10);
				uint8_t b = ((std::rand() % 10) << 4) | (std::rand() % 10);
				if (b > a)
				{
					uint8_t temp = a;
					a = b;
					b = temp;
				}
				cpu.A = a;
				cpu.given(b, 0);
				uint8_t expected = (a >> 4) * 10 + (a & 0x0F);
				expected -= (b >> 4) * 10 + (b & 0x0F);

				cpu.setFlag(cpu.C, 1);
				cpu.testInstruction(0xE9);
				//printf("%02X-%02X=%d => $%02X\n", a, b, expected, cpu.A);

				EXPECT_EQ(cpu.cycles + cpu.getIns().extraCycle, 2);

				uint8_t result = (cpu.A >> 4) * 10 + (cpu.A & 0x0F);
				EXPECT_EQ(result, expected);
			}
		}
		TEST_F(Test6502ADCSBC, SBCDecimalWithBorrow)
		{
			cpu.A = 0x00;
			cpu.given(0x21, 0);

			cpu.setFlag(cpu.D, 1);
			cpu.setFlag(cpu.C, 1);
			cpu.testInstruction(0xE9);
			//printf("$%02X\n", cpu.A);

			EXPECT_FALSE(cpu.getFlag(cpu.C));
		}
	}
}