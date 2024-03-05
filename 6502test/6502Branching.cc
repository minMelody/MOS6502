// Test 6502 branching

#include "6502Includes.hpp"

namespace MOS6502
{
	namespace UnitTests
	{
		struct Test6502Branch : public testing::Test
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

		TEST_F(Test6502Branch, BranchOnFlagClear)
		{
			std::vector<uint8_t> opcodes =
			{
				0x10, cpu.N,
				0x50, cpu.V,
				0x90, cpu.C,
				0xD0, cpu.Z
			};

			for (int i = 0; i < opcodes.size() - 1; i += 2)
			{
				m6502::Statusflags flag = static_cast<m6502::Statusflags>(opcodes[i + 1]);
				cpu.setFlag(flag, 0);
				cpu.PC = 0x0001;
				mem.data[cpu.PC] = 0x04;
				uint16_t expected = cpu.PC + 5;

				cpu.given(0, cpu.fetchAddress(opcodes[i]));
				//printf("%02X: %02X\n", opcodes[i], opcodes[i + 1]);
				cpu.testInstruction(opcodes[i]);

				EXPECT_EQ(cpu.cycles, 3);
				EXPECT_EQ(cpu.PC, expected);
			}
		}
		TEST_F(Test6502Branch, BranchOnFlagSet)
		{
			std::vector<uint8_t> opcodes =
			{
				0x30, cpu.N,
				0x70, cpu.V,
				0xB0, cpu.C,
				0xF0, cpu.Z
			};

			for (int i = 0; i < opcodes.size() - 1; i += 2)
			{
				m6502::Statusflags flag = static_cast<m6502::Statusflags>(opcodes[i + 1]);
				cpu.PC = 0x0001;
				cpu.setFlag(flag, 1);
				mem.data[cpu.PC] = 0x04;
				uint16_t expected = cpu.PC + 5;

				cpu.given(0, cpu.fetchAddress(opcodes[i]));
				//printf("%02X: %02X\n", opcodes[i], opcodes[i + 1]);
				cpu.testInstruction(opcodes[i]);

				EXPECT_EQ(cpu.cycles, 3);
				EXPECT_EQ(cpu.PC, expected);
			}
		}
		TEST_F(Test6502Branch, BranchNegativeOffsetCrossPageBoundary)
		{
			cpu.PC = 0xF000;
			cpu.setFlag(cpu.N, 1);
			mem.data[cpu.PC] = 0xFC; // -4
			uint16_t expected = cpu.PC - 3;

			cpu.given(0, cpu.fetchAddress(0x30));
			cpu.testInstruction(0x30);

			EXPECT_EQ(cpu.cycles, 4);
			EXPECT_EQ(cpu.PC, expected);
		}
		TEST_F(Test6502Branch, DontBranchOnOverflowClear)
		{
			mem.data[cpu.PC] = 0x04;
			cpu.setFlag(cpu.V, 0);
			uint16_t expected = cpu.PC + 1;

			cpu.given(0, cpu.fetchAddress(0x70));
			cpu.testInstruction(0x70);

			EXPECT_EQ(cpu.cycles, 2);
			EXPECT_EQ(cpu.PC, expected);
		}

		TEST_F(Test6502Branch, Jump)
		{
			mem.data[cpu.PC] = 0x00;
			mem.data[cpu.PC + 1] = 0x80;

			cpu.given(0, cpu.fetchAddress(0x4C));
			cpu.testInstruction(0x4C);

			EXPECT_EQ(cpu.cycles, 3);
			EXPECT_EQ(cpu.PC, 0x8000);
		}

		TEST_F(Test6502Branch, JumpToSubroutine)
		{
			mem.data[cpu.PC] = 0x00;
			mem.data[cpu.PC + 1] = 0x80;
			uint16_t prevState = cpu.PC + 1;

			cpu.given(0, cpu.fetchAddress(0x20));
			cpu.testInstruction(0x20);

			EXPECT_EQ(cpu.cycles, 6);
			EXPECT_EQ(cpu.PC, 0x8000);
			EXPECT_EQ(mem.data[0x0100 | cpu.SP + 1], prevState & 0x00FF);
			EXPECT_EQ(mem.data[0x0100 | cpu.SP + 2], (prevState >> 8) & 0x00FF);
		}
		TEST_F(Test6502Branch, ReturnFromSubroutine)
		{
			cpu.stackPush(0x70);
			cpu.stackPush(0x01);
			cpu.testInstruction(0x60);

			EXPECT_EQ(cpu.cycles, 6);
			EXPECT_EQ(cpu.PC, 0x7002);
		}
	}
}