#include "6502Includes.hpp"

namespace MOS6502
{
	namespace UnitTests
	{
		struct Test6502Program : public testing::Test
		{
			Bus bus;
			FakeMemory mem{ 0x10000 };
			Test6502 cpu{ &bus };
			virtual void SetUp()
			{
				bus.connect(&mem, 0x0000);
			}
			virtual void TearDown()
			{
			}
		};

		TEST_F(Test6502Program, Multiply)
		{
			mem.data[0xFFFC] = 0x00;
			mem.data[0xFFFD] = 0x80;

			mem.data[0x0000] = 5;
			mem.data[0x0001] = 9;
			uint8_t expected = 45;

			mem.data[0x8000] = 0xa6; // LDX $00
			mem.data[0x8001] = 0x00;
			mem.data[0x8002] = 0x65; // ADC $01
			mem.data[0x8003] = 0x01;
			mem.data[0x8004] = 0xca; // DEX
			mem.data[0x8005] = 0xd0; // BNE
			mem.data[0x8006] = 0xfb; // -5
			mem.data[0x8007] = 0x00; // BRK
			mem.data[0x8008] = 0x00;

			cpu.reset();

			std::cout << "Zp:";
			for (uint16_t adr = 0x00; adr < 0x10; adr++)
			{
				printf(" %02X", mem.data[adr]);
			}
			std::cout << std::endl;

			while (cpu.getFlag(Test6502::I) == false)
			{
				cpu.step();
			}

			EXPECT_EQ(cpu.A, expected);
		}
	}
}