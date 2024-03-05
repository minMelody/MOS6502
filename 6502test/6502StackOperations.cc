// Test 6502 stack operations

#include "6502Includes.hpp"

namespace MOS6502
{
	namespace UnitTests
	{
		struct Test6502Stack : public testing::Test
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

		TEST_F(Test6502Stack, PHP)
		{
			Test6502 prevState = cpu;

			cpu.testInstruction(0x08);

			EXPECT_EQ(cpu.cycles, 3);
			EXPECT_EQ(cpu.SR, prevState.SR);
			EXPECT_EQ(cpu.SP, prevState.SP - 1);
		}
		TEST_F(Test6502Stack, PLP)
		{
			uint8_t prevState = cpu.SR;
			Test6502 cpuCopy = cpu;
			cpuCopy.setFlag(cpu.C, 1);
			cpuCopy.setFlag(cpu.N, 0);
			cpuCopy.setFlag(cpu.Z, 1);
			cpuCopy.setFlag(cpu.V, 1);
			cpu.stackPush(cpuCopy.SR);

			cpu.testInstruction(0x28);

			EXPECT_EQ(cpu.cycles, 4);
			EXPECT_NE(cpu.SR, prevState);
			EXPECT_EQ(cpu.SR, cpuCopy.SR);
		}
		TEST_F(Test6502Stack, PHA)
		{
			cpu.A = 0xea;
			cpu.testInstruction(0x48);

			EXPECT_EQ(cpu.cycles, 3);
			EXPECT_EQ(mem.data[0x0100 | cpu.SP + 1], cpu.A);
		}
		TEST_F(Test6502Stack, PLA)
		{
			cpu.stackPush(0xab);
			cpu.testInstruction(0x68);

			EXPECT_EQ(cpu.cycles, 4);
			EXPECT_EQ(cpu.A, 0xab);
		}
	}
}