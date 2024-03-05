#pragma once

#include "6502.h"

namespace MOS6502
{
	namespace UnitTests
	{
		// Expose protected members and methods to the test environment
		class Test6502 : public m6502
		{
		public:
			using m6502::m6502;	// using constructor

			using m6502::cycles;
			using m6502::stackPush;
			using m6502::stackPop;
			using m6502::irq;
			using m6502::nmi;

			Instruction getIns() { return opsLUT[IR]; }

			// Set operand and operand address for test
			void given(uint8_t value, uint16_t address)
			{
				opd = value;
				fetchedAdr = address;
			}

			uint16_t fetchAddress(uint8_t opcode) { return (this->*opsLUT[opcode].fetch)(opsLUT[IR].extraCycle); }

			// Execute an instruction by opcode
			void testInstruction(uint8_t opcode)
			{
				IR = opcode;
				cycles = opsLUT[IR].cycles;
				setFlag(_, 1); // unused flag is always set

				(this->*opsLUT[IR].execute)();
			}
		};
	}
}
