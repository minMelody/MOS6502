#pragma once

#include <cstdint>
#include <vector>

namespace MOS6502
{
	// Forward declaration of the bus
	struct Bus;

	class m6502
	{
	public:
		m6502(Bus* bus);
		~m6502();

		// CPU internal registers
		uint16_t PC;	// Program Counter
		uint8_t  IR;	// Instruction Register
		uint8_t  A ;	// Accumulator Register
		uint8_t  X ;	// Index Register X
		uint8_t  Y ;	// Index Register Y
		uint8_t  SP;	// Stack Pointer

		// Processor Status Register
		uint8_t SR;
		enum Statusflags
		{
			C = 0b00000001,	// Carry Bit Flag
			Z = 0b00000010,	// Zero Flag
			I = 0b00000100,	// Interrrupt Disable Flag
			D = 0b00001000,	// Decimal Mode Flag
			B = 0b00010000,	// Break Flag
			_ = 0b00100000,	// Unused Flag
			V = 0b01000000,	// Overflow Flag
			N = 0b10000000	// Negative Flag
		};
		bool getFlag(Statusflags f);
		void setFlag(Statusflags f, bool v);
	public:
		void reset();
		// Indicates an instruction has been completed when true
		// allows single instruction execution
		bool sync();
		bool irqPending, nmiPending;
		
		void step();

	protected:
		void irq();	// Handle Interrupt Request
		void nmi();	// handle Non-Maskable Interrupt

		Bus* bus = nullptr;

		uint16_t
			ACC(bool ec), IMM(bool ec), IMP(bool ec),
			ABS(bool ec), ABX(bool ec), ABY(bool ec), IND(bool ec), REL(bool ec),
			ZIX(bool ec), ZIY(bool ec), ZP (bool ec), ZPX(bool ec), ZPY(bool ec);

		void
			ADC(), AND(), ASL(),
			BCC(), BCS(), BEQ(), BIT(), BMI(), BNE(), BPL(), BRK(), BVC(), BVS(),
			CLC(), CLD(), CLI(), CLV(), CMP(), CPX(), CPY(),
			DEC(), DEX(), DEY(),
			EOR(),
			INC(), INX(), INY(),
			JMP(), JSR(),
			LDA(), LDX(), LDY(), LSR(),
			NOP(),
			ORA(),
			PHA(), PHP(), PLA(), PLP(),
			ROL(), ROR(), RTI(), RTS(),
			SBC(), SEC(), SED(), SEI(), STA(), STX(), STY(),
			TAX(), TAY(), TSX(), TXA(), TXS(), TYA(),
			___();

		struct Instruction
		{
			void (m6502::*execute)() = nullptr; // Execute the instruction
			uint16_t(m6502::*fetch)(bool) = nullptr; // Fetch the operand from the instruction's addressing mode
			uint8_t cycles = 0;
			bool extraCycle = false; // Some instructions require one extra cycle when crossing a page boundary
		};
		std::vector<Instruction> opsLUT;

		uint8_t cycles = 0;
		uint8_t opd;		 // The instruction's operand
		uint16_t fetchedAdr; // Address of the operand

		uint8_t stackPop();
		void stackPush(uint8_t byte);
		void BranchIf(Statusflags f, bool condition);
	};
}