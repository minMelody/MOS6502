// Tests 6502 reset and interrupts

#include "6502Includes.hpp"

namespace MOS6502
{
	namespace UnitTests
	{
		struct Test6502CoreFs : public testing::Test
		{
			Bus bus;
			FakeMemory mem{ 0x10000 };
			Test6502 cpu{ &bus };
			virtual void SetUp()
			{
				bus.connect(&mem, 0x0000);
				mem.data[0xFFFA] = 0x01;
				mem.data[0xFFFB] = 0x02;
				mem.data[0xFFFC] = 0x03;
				mem.data[0xFFFD] = 0x04;
				mem.data[0xFFFE] = 0x05;
				mem.data[0xFFFF] = 0x06;

				cpu.reset();
				cpu.cycles = 0;
			}
			virtual void TearDown()
			{
			}
			void FLAG_UNCHANGED(Test6502* prevState, Test6502::Statusflags f)
			{
				EXPECT_EQ(cpu.getFlag(f), prevState->getFlag(f));
			}
		};
		TEST_F(Test6502CoreFs, ResetCPU)
		{
			cpu.reset();

			EXPECT_EQ(cpu.cycles, 8);
			EXPECT_EQ(cpu.PC, 0x0403);
			EXPECT_EQ(cpu.A, 0);
			EXPECT_EQ(cpu.X, 0);
			EXPECT_EQ(cpu.Y, 0);
			EXPECT_EQ(cpu.IR, 0x00);
			EXPECT_EQ(cpu.SP, 0xFD);
			EXPECT_EQ(cpu.SR, 0b00100000);

			EXPECT_FALSE(cpu.irqPending);
			EXPECT_FALSE(cpu.nmiPending);
		}
		TEST_F(Test6502CoreFs, InterruptRequest)
		{
			cpu.setFlag(cpu.I, 0);
			Test6502 prevState = cpu;

			cpu.irq();

			EXPECT_EQ(cpu.cycles, 7);
			EXPECT_EQ(cpu.SP, prevState.SP - 3);
			EXPECT_EQ(cpu.PC, 0x0605);
			// Status register is mostly unchanged except B and I
			EXPECT_FALSE(cpu.getFlag(cpu.B));
			EXPECT_TRUE(cpu.getFlag(cpu.I));

			FLAG_UNCHANGED(&prevState, cpu.C);
			FLAG_UNCHANGED(&prevState, cpu.Z);
			FLAG_UNCHANGED(&prevState, cpu.D);
			FLAG_UNCHANGED(&prevState, cpu._);
			FLAG_UNCHANGED(&prevState, cpu.V);
			FLAG_UNCHANGED(&prevState, cpu.N);
			// Other registers stay unchanged
			EXPECT_EQ(cpu.A, prevState.A);
			EXPECT_EQ(cpu.X, prevState.X);
			EXPECT_EQ(cpu.Y, prevState.Y);
		}
		TEST_F(Test6502CoreFs, InterruptRequestDisabled)
		{
			cpu.setFlag(cpu.I, 1);
			Test6502 prevState = cpu;

			cpu.irq();
			// Nothing changed
			EXPECT_EQ(cpu.cycles, 0);
			EXPECT_EQ(cpu.SP, prevState.SP);
			EXPECT_EQ(cpu.PC, prevState.PC);
			EXPECT_EQ(cpu.SR, prevState.SR);
			EXPECT_EQ(cpu.A, prevState.A);
			EXPECT_EQ(cpu.X, prevState.X);
			EXPECT_EQ(cpu.Y, prevState.Y);
		}
		TEST_F(Test6502CoreFs, NonMaskableInterrupt)
		{
			Test6502 prevState = cpu;

			cpu.nmi();

			EXPECT_EQ(cpu.cycles, 7);
			EXPECT_EQ(cpu.SP, prevState.SP - 3);
			EXPECT_EQ(cpu.PC, 0x0201);
			// Status register is mostly unchanged except B and I
			EXPECT_FALSE(cpu.getFlag(cpu.B));
			EXPECT_TRUE(cpu.getFlag(cpu.I));
			FLAG_UNCHANGED(&prevState, cpu.C);
			FLAG_UNCHANGED(&prevState, cpu.Z);
			FLAG_UNCHANGED(&prevState, cpu.D);
			FLAG_UNCHANGED(&prevState, cpu._);
			FLAG_UNCHANGED(&prevState, cpu.V);
			FLAG_UNCHANGED(&prevState, cpu.N);
			// Other registers stay unchanged
			EXPECT_EQ(cpu.A, prevState.A);
			EXPECT_EQ(cpu.X, prevState.X);
			EXPECT_EQ(cpu.Y, prevState.Y);
		}

		TEST_F(Test6502CoreFs, BreakInstruction)
		{
			Test6502 prevState = cpu;

			cpu.testInstruction(0x00);

			EXPECT_EQ(cpu.cycles, 7);
			EXPECT_EQ(cpu.SP, prevState.SP - 3);
			EXPECT_EQ(cpu.PC, 0x0605);
			EXPECT_EQ(cpu.SR, prevState.SR | cpu.I);
			EXPECT_EQ(mem.data[0x0100 | (cpu.SP + 1)], prevState.SR | cpu.B);
			EXPECT_EQ(cpu.A, prevState.A);
			EXPECT_EQ(cpu.X, prevState.X);
			EXPECT_EQ(cpu.Y, prevState.Y);
		}

		TEST_F(Test6502CoreFs, ReturnFromInterrupt)
		{
			cpu.testInstruction(0x00);	// BRK instruction first
			Test6502 prevState = cpu;

			cpu.testInstruction(0x40);

			EXPECT_EQ(cpu.cycles, 6);
			EXPECT_EQ(cpu.SP, prevState.SP + 3);
			EXPECT_NE(cpu.SR, prevState.SR);
			EXPECT_NE(cpu.PC, prevState.PC);
		}

		TEST_F(Test6502CoreFs, NOP)
		{
			Test6502 prevState = cpu;

			cpu.testInstruction(0xea);

			EXPECT_EQ(cpu.cycles, 2);
			EXPECT_EQ(cpu.SR, prevState.SR);
			EXPECT_EQ(cpu.PC, prevState.PC);
			EXPECT_EQ(cpu.A, prevState.A);
			EXPECT_EQ(cpu.X, prevState.X);
			EXPECT_EQ(cpu.Y, prevState.Y);
			EXPECT_EQ(cpu.SP, prevState.SP);
		}
	}
}