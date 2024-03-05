#include <stdexcept>
#include "6502.h"
#include "Bus.h"

#define LOG_INFO

MOS6502::m6502::m6502(Bus* bus)
{
	this->bus = bus;
	opsLUT =
	{	//              0                                1                                2                                3                                4                                5                                6                                7                                8                                9                                A                                B                                C                                D                                E                                F
/*0*/	{&m6502::BRK, &m6502::IMP, 7, 0},{&m6502::ORA, &m6502::ZIX, 6, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::ORA, &m6502::ZP , 3, 0},{&m6502::ASL, &m6502::ZP , 5, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::PHP, &m6502::IMP, 3, 0},{&m6502::ORA, &m6502::IMM, 2, 0},{&m6502::ASL, &m6502::ACC, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::ORA, &m6502::ABS, 4, 0},{&m6502::ASL, &m6502::ABS, 6, 0},{&m6502::___, &m6502::IMP, 2, 0},
/*1*/	{&m6502::BPL, &m6502::REL, 2, 0},{&m6502::ORA, &m6502::ZIY, 5, 1},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::ORA, &m6502::ZPX, 4, 0},{&m6502::ASL, &m6502::ZPX, 6, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::CLC, &m6502::IMP, 2, 0},{&m6502::ORA, &m6502::ABY, 4, 1},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::ORA, &m6502::ABX, 4, 1},{&m6502::ASL, &m6502::ABX, 7, 0},{&m6502::___, &m6502::IMP, 2, 0},
/*2*/	{&m6502::JSR, &m6502::ABS, 6, 0},{&m6502::AND, &m6502::ZIX, 6, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::BIT, &m6502::ZP , 3, 0},{&m6502::AND, &m6502::ZP , 3, 0},{&m6502::ROL, &m6502::ZP , 5, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::PLP, &m6502::IMP, 4, 0},{&m6502::AND, &m6502::IMM, 2, 0},{&m6502::ROL, &m6502::ACC, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::BIT, &m6502::ABS, 4, 0},{&m6502::AND, &m6502::ABS, 4, 0},{&m6502::ROL, &m6502::ABS, 6, 0},{&m6502::___, &m6502::IMP, 2, 0},
/*3*/	{&m6502::BMI, &m6502::REL, 2, 0},{&m6502::AND, &m6502::ZIY, 5, 1},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::AND, &m6502::ZPX, 4, 0},{&m6502::ROL, &m6502::ZPX, 6, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::SEC, &m6502::IMP, 2, 0},{&m6502::AND, &m6502::ABY, 4, 1},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::AND, &m6502::ABX, 4, 1},{&m6502::ROL, &m6502::ABX, 7, 0},{&m6502::___, &m6502::IMP, 2, 0},
/*4*/	{&m6502::RTI, &m6502::IMP, 6, 0},{&m6502::EOR, &m6502::ZIX, 6, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::EOR, &m6502::ZP , 3, 0},{&m6502::LSR, &m6502::ZP , 5, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::PHA, &m6502::IMP, 3, 0},{&m6502::EOR, &m6502::IMM, 2, 0},{&m6502::LSR, &m6502::ACC, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::JMP, &m6502::ABS, 3, 0},{&m6502::EOR, &m6502::ABS, 4, 0},{&m6502::LSR, &m6502::ABS, 6, 0},{&m6502::___, &m6502::IMP, 2, 0},
/*5*/	{&m6502::BVC, &m6502::REL, 2, 0},{&m6502::EOR, &m6502::ZIY, 5, 1},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::EOR, &m6502::ZPX, 4, 0},{&m6502::LSR, &m6502::ZPX, 6, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::CLI, &m6502::IMP, 2, 0},{&m6502::EOR, &m6502::ABY, 4, 1},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::EOR, &m6502::ABX, 4, 1},{&m6502::LSR, &m6502::ABX, 7, 0},{&m6502::___, &m6502::IMP, 2, 0},
/*6*/	{&m6502::RTS, &m6502::IMP, 6, 0},{&m6502::ADC, &m6502::ZIX, 6, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::ADC, &m6502::ZP , 3, 0},{&m6502::ROR, &m6502::ZP , 5, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::PLA, &m6502::IMP, 4, 0},{&m6502::ADC, &m6502::IMM, 2, 0},{&m6502::ROR, &m6502::ACC, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::JMP, &m6502::IND, 5, 0},{&m6502::ADC, &m6502::ABS, 4, 0},{&m6502::ROR, &m6502::ABS, 6, 0},{&m6502::___, &m6502::IMP, 2, 0},
/*7*/	{&m6502::BVS, &m6502::REL, 2, 0},{&m6502::ADC, &m6502::ZIY, 5, 1},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::ADC, &m6502::ZPX, 4, 0},{&m6502::ROR, &m6502::ZPX, 6, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::SEI, &m6502::IMP, 2, 0},{&m6502::ADC, &m6502::ABY, 4, 1},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::ADC, &m6502::ABX, 4, 1},{&m6502::ROR, &m6502::ABX, 7, 0},{&m6502::___, &m6502::IMP, 2, 0},
/*8*/	{&m6502::___, &m6502::IMP, 2, 0},{&m6502::STA, &m6502::ZIX, 6, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::STY, &m6502::ZP , 3, 0},{&m6502::STA, &m6502::ZP , 3, 0},{&m6502::STX, &m6502::ZP , 3, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::DEY, &m6502::IMP, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::TXA, &m6502::IMP, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::STY, &m6502::ABS, 4, 0},{&m6502::STA, &m6502::ABS, 4, 0},{&m6502::STX, &m6502::ABS, 4, 0},{&m6502::___, &m6502::IMP, 2, 0},
/*9*/	{&m6502::BCC, &m6502::REL, 2, 0},{&m6502::STA, &m6502::ZIY, 6, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::STY, &m6502::ZPX, 4, 0},{&m6502::STA, &m6502::ZPX, 4, 0},{&m6502::STX, &m6502::ZPX, 4, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::TYA, &m6502::IMP, 2, 0},{&m6502::STA, &m6502::ABY, 5, 0},{&m6502::TXS, &m6502::IMP, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::STA, &m6502::ABX, 5, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},
/*A*/	{&m6502::LDY, &m6502::IMM, 2, 0},{&m6502::LDA, &m6502::ZIX, 6, 0},{&m6502::LDX, &m6502::IMM, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::LDY, &m6502::ZP , 3, 0},{&m6502::LDA, &m6502::ZP , 3, 0},{&m6502::LDX, &m6502::ZP , 3, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::TAY, &m6502::IMP, 2, 0},{&m6502::LDA, &m6502::IMM, 2, 0},{&m6502::TAX, &m6502::IMP, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::LDY, &m6502::ABS, 4, 0},{&m6502::LDA, &m6502::ABS, 4, 0},{&m6502::LDX, &m6502::ABS, 4, 0},{&m6502::___, &m6502::IMP, 2, 0},
/*B*/	{&m6502::BCS, &m6502::REL, 2, 0},{&m6502::LDA, &m6502::ZIY, 5, 1},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::LDY, &m6502::ZPX, 4, 0},{&m6502::LDA, &m6502::ZPX, 4, 0},{&m6502::LDX, &m6502::ZPY, 4, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::CLV, &m6502::IMP, 2, 0},{&m6502::LDA, &m6502::ABY, 4, 1},{&m6502::TSX, &m6502::IMP, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::LDY, &m6502::ABX, 4, 1},{&m6502::LDA, &m6502::ABX, 4, 1},{&m6502::LDX, &m6502::ABY, 4, 1},{&m6502::___, &m6502::IMP, 2, 0},
/*C*/	{&m6502::CPY, &m6502::IMM, 2, 0},{&m6502::CMP, &m6502::ZIX, 6, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::CPY, &m6502::ZP , 3, 0},{&m6502::CMP, &m6502::ZP , 3, 0},{&m6502::DEC, &m6502::ZP , 5, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::INY, &m6502::IMP, 2, 0},{&m6502::CMP, &m6502::IMM, 2, 0},{&m6502::DEX, &m6502::IMP, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::CPY, &m6502::ABS, 4, 0},{&m6502::CMP, &m6502::ABS, 4, 0},{&m6502::DEC, &m6502::ABS, 6, 0},{&m6502::___, &m6502::IMP, 2, 0},
/*D*/	{&m6502::BNE, &m6502::REL, 2, 0},{&m6502::CMP, &m6502::ZIY, 5, 1},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::CMP, &m6502::ZPX, 4, 0},{&m6502::DEC, &m6502::ZPX, 6, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::CLD, &m6502::IMP, 2, 0},{&m6502::CMP, &m6502::ABY, 4, 1},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::CMP, &m6502::ABX, 4, 1},{&m6502::DEC, &m6502::ABX, 7, 0},{&m6502::___, &m6502::IMP, 2, 0},
/*E*/	{&m6502::CPX, &m6502::IMM, 2, 0},{&m6502::SBC, &m6502::ZIX, 6, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::CPX, &m6502::ZP , 3, 0},{&m6502::SBC, &m6502::ZP , 3, 0},{&m6502::INC, &m6502::ZP , 5, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::INX, &m6502::IMP, 2, 0},{&m6502::SBC, &m6502::IMM, 2, 0},{&m6502::NOP, &m6502::IMP, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::CPX, &m6502::ABS, 4, 0},{&m6502::SBC, &m6502::ABS, 4, 0},{&m6502::INC, &m6502::ABS, 6, 0},{&m6502::___, &m6502::IMP, 2, 0},
/*F*/	{&m6502::BEQ, &m6502::REL, 2, 0},{&m6502::SBC, &m6502::ZIY, 5, 1},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::SBC, &m6502::ZPX, 4, 0},{&m6502::INC, &m6502::ZPX, 6, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::SED, &m6502::IMP, 2, 0},{&m6502::SBC, &m6502::ABY, 4, 1},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::___, &m6502::IMP, 2, 0},{&m6502::SBC, &m6502::ABX, 4, 1},{&m6502::INC, &m6502::ABX, 7, 0},{&m6502::___, &m6502::IMP, 2, 0}
	};
}

MOS6502::m6502::~m6502()
{
	/* void */
}

// Steps one clock cycle
void MOS6502::m6502::step()
{
	if (cycles == 0)
	{
		if (nmiPending)
		{
			nmi();
			nmiPending = irqPending = false;
			return;
		}
		else if (irqPending)
		{
			irq();
			nmiPending = irqPending = false;
			return;
		}

#ifdef LOG_INFO
		uint16_t insPC = PC;
#endif // LOG_INFO
		IR = bus->read(PC);
		PC++;

		cycles = opsLUT[IR].cycles;

		setFlag(_, 1); // unused flag is always set

		if (opsLUT[IR].fetch == &m6502::ACC || opsLUT[IR].fetch == &m6502::IMM || opsLUT[IR].fetch == &m6502::IMP)
		{
			opd = (this->*opsLUT[IR].fetch)(opsLUT[IR].extraCycle) & 0x00FF;
		}
		else
		{
			fetchedAdr = (this->*opsLUT[IR].fetch)(opsLUT[IR].extraCycle);
			opd = bus->read(fetchedAdr);
		}

		(this->*opsLUT[IR].execute)();

#ifdef LOG_INFO
		printf("$%04X:%02X  opd:%02x  A:%02X X:%02X Y:%02X SP:%02X SR:%c%c%c%c%c%c%c%c\n",
			insPC, IR, opd, A, X, Y, SP,
			getFlag(N) ? 'N' : '.', getFlag(V) ? 'V' : '.', getFlag(_) ? '1' : '.',
			getFlag(B) ? 'B' : '.', getFlag(D) ? 'D' : '.', getFlag(I) ? 'I' : '.',
			getFlag(Z) ? 'Z' : '.', getFlag(C) ? 'C' : '.');
#endif // LOG_INFO
	}

	cycles--;
}

// ===== Hardware pins and Interrupts =====

void MOS6502::m6502::reset()
{
	// Clear helper variables
	irqPending = nmiPending = false;
	IR = opd = 0x00;
	fetchedAdr = 0x0000;

	// Reset internal registers
	A = X = Y = 0x00;
	SP = 0xFD;
	SR = 0x00 | Statusflags::_;

	// Fetch the reset vector
	uint8_t pcl = bus->read(0xFFFC);
	uint8_t pch = bus->read(0xFFFD);
	// Load it onto the program counter
	PC = (pch << 8) | pcl;

	cycles = 8;
}

// Interrupt Request
void MOS6502::m6502::irq()
{
	if (getFlag(I)) return;
	// Push the program counter and status register onto the stack
	stackPush((PC >> 8) & 0x00FF);
	stackPush(PC & 0x00FF);
	stackPush(SR);

	// Set interrupt disable flag
	setFlag(I, 1);

	// Fetch the interrupt handler vector
	uint8_t pcl = bus->read(0xFFFE);
	uint8_t pch = bus->read(0xFFFF);
	PC = (pch << 8) | pcl;

	cycles = 7;
}

// Non-Maskable Interrupt
void MOS6502::m6502::nmi()
{
	stackPush((PC >> 8) & 0x00FF);
	stackPush(PC & 0x00FF);
	stackPush(SR);

	// Set interrupt disable flag
	setFlag(I, 1);

	uint8_t pcl = bus->read(0xFFFA);
	uint8_t pch = bus->read(0xFFFB);
	PC = (pch << 8) | pcl;

	cycles = 7;
}

// In hardware: The SYNC pin is high when the microprocessor is fetching an opcode.
bool MOS6502::m6502::sync()
{
	return cycles == 0;
}

// ===== Status Flags =====

bool MOS6502::m6502::getFlag(Statusflags f)
{
	return (SR & f) > 0;
}

void MOS6502::m6502::setFlag(Statusflags f, bool v)
{
	if (v) SR |= f;
	else SR &= ~f;
}

// ===== Stack =====
// The stack uses memory from page 1 ($0100 to $01FF), decrementing as bytes are pushed.

void MOS6502::m6502::stackPush(uint8_t byte)
{
	bus->write(0x0100 | SP, byte);
	SP--;
}

uint8_t MOS6502::m6502::stackPop()
{
	SP++;
	return bus->read(0x0100 | SP);
}

// ===== Addressing Modes =====

// Accumulator
uint16_t MOS6502::m6502::ACC(bool extraCycle)
{
	return A;
}
// Immediate
uint16_t MOS6502::m6502::IMM(bool extraCycle)
{
	uint8_t operand = bus->read(PC);
	PC++;
	return operand;
}
// Implied
uint16_t MOS6502::m6502::IMP(bool extraCycle)
{
	return IR;
}
// Absolute
uint16_t MOS6502::m6502::ABS(bool extraCycle)
{
	uint8_t lb = bus->read(PC);
	PC++;
	uint8_t hb = bus->read(PC);
	PC++;
	return (hb << 8) | lb;
}
// Absolute Indexed X
uint16_t MOS6502::m6502::ABX(bool extraCycle)
{
	uint8_t lb = bus->read(PC);
	PC++;
	uint8_t hb = bus->read(PC);
	PC++;
	uint16_t addr = (hb << 8) | lb;
	addr += X;

	if (extraCycle)
	{
		if ((addr & 0xFF00) != (hb << 8)) cycles++;
	}

	return addr;
}
// Absolute Indexed Y
uint16_t MOS6502::m6502::ABY(bool extraCycle)
{
	uint8_t lb = bus->read(PC);
	PC++;
	uint8_t hb = bus->read(PC);
	PC++;
	uint16_t addr = (hb << 8) | lb;
	addr += Y;

	if (extraCycle)
	{
		if ((addr & 0xFF00) != (hb << 8)) cycles++;
	}

	return addr;
}
// Indirect
// This one's special because of a bug in the microprocessor.
// When instructed ($30FF):
// The 6502 takes the low byte of the address from $30FF and the high byte from $3000 instead of $3100
uint16_t MOS6502::m6502::IND(bool extraCycle)
{
	uint8_t lptr = bus->read(PC);
	PC++;
	uint8_t hptr = bus->read(PC);
	PC++;
	uint16_t ptr = (hptr << 8) | lptr;

	uint8_t lb = bus->read(ptr);
	uint8_t hb = (lptr == 0xFF) ? bus->read(ptr & 0xFF00) : bus->read(ptr + 1);
	return (hb << 8) | lb;
}
// Relative
// Offset can be negative
uint16_t MOS6502::m6502::REL(bool extraCycle)
{
	int8_t offset = bus->read(PC);
	PC++;
	return PC + offset;
}
// Indexed Indirect (adr,X)
uint16_t MOS6502::m6502::ZIX(bool extraCycle)
{
	uint8_t ptr = bus->read(PC);
	PC++;
	ptr += X;

	uint8_t lb = bus->read(ptr);
	uint8_t hb = bus->read(ptr + 1);

	return (hb << 8) | lb;
}
// Indirect Indexed (adr),Y
uint16_t MOS6502::m6502::ZIY(bool extraCycle)
{
	uint8_t ptr = bus->read(PC);
	PC++;

	uint8_t lb = bus->read(ptr);
	uint8_t hb = bus->read(ptr + 1);

	uint16_t addr = (hb << 8) | lb;
	addr += Y;

	if (extraCycle)
	{
		if ((addr & 0xFF00) != (hb << 8)) cycles++;
	}

	return addr;
}
// Zero Page
uint16_t MOS6502::m6502::ZP (bool extraCycle)
{
	uint8_t addr = bus->read(PC);
	PC++;
	return addr;
}
// Zero Page Indexed X
uint16_t MOS6502::m6502::ZPX(bool extraCycle)
{
	uint8_t addr = bus->read(PC);
	PC++;
	// Taking advantage of addr being an 8 bit integer the carry is automatically discarded
	addr += X;
	return addr;
}
// Zero Page Indexed Y
uint16_t MOS6502::m6502::ZPY(bool extraCycle)
{
	uint8_t addr = bus->read(PC);
	PC++;
	addr += Y;
	return addr;
}

// ===== Instructions =====

// Conditional Branch
void MOS6502::m6502::BranchIf(Statusflags f, bool condition)
{
	if (getFlag(f) == condition)
	{
		cycles++;
		if ((PC & 0xFF00) != (fetchedAdr & 0xFF00)) cycles++;
		PC = fetchedAdr;
	}
}

// Add with carry
void MOS6502::m6502::ADC()
{
	uint8_t lo_nibble = (A & 0x0F) + (opd & 0x0F) + getFlag(C);
	uint8_t hi_nibble = (A >> 4) + (opd >> 4);
	uint16_t result_16 = (hi_nibble << 4) + lo_nibble;
	uint8_t result_8 = result_16 & 0x00FF;

	setFlag(C, result_16 & 0xFF00);
	setFlag(N, result_8  & 0x80);
	setFlag(Z, result_8 == 0x00);

	// The overflow flag is a little more complicated
	/*
		(~(A ^ B)		// Bit 7 will be set if the two operands are of the same sign.
		&				// Given the two operands have the same sign,
		(A ^ result))	// test for different signs between the result and either of the operands.
		& 0x80			// The overflow flag is equal to bit 7 of the whole operation.
	*/
	setFlag(V, (~(A^opd)&(A^result_8)) & 0x80);

	// As I understand it, the 6502 sets the flags before applying BCD corrections, resulting in all but the carry flag being invalid in decimal mode.
	if (getFlag(D))
	{
		if (lo_nibble > 9) lo_nibble += 6;
		hi_nibble += lo_nibble > 0x0F;

		if (hi_nibble > 9) hi_nibble += 6;
		setFlag(C, hi_nibble > 0x0F);

		result_8 = (hi_nibble << 4) | (lo_nibble & 0x0F);
	}

	A = result_8;
}
// Logical AND
void MOS6502::m6502::AND()
{
	A &= opd;
	setFlag(Z, A == 0x00);
	setFlag(N, A & 0x80);
}
// Arithmetic Shift Left
void MOS6502::m6502::ASL()
{
	uint16_t result = opd << 1;
	setFlag(C, result & 0xFF00);
	setFlag(N, result & 0x80);
	setFlag(Z, (result & 0x00FF) == 0x00);

	if (opsLUT[IR].fetch == &m6502::ACC)
	{
		A = result & 0x00FF;
	}
	else
	{
		bus->write(fetchedAdr, result & 0x00FF);
	}
}
// Branch on carry clear
void MOS6502::m6502::BCC()
{
	BranchIf(C, 0);
}
// Branch on carry set
void MOS6502::m6502::BCS()
{
	BranchIf(C, 1);
}
// Branch on equal
void MOS6502::m6502::BEQ()
{
	BranchIf(Z, 1);
}
// Bit test
void MOS6502::m6502::BIT()
{
	uint8_t result = A & opd;
	setFlag(Z, result == 0x00);
	setFlag(V, result & (1 << 6));
	setFlag(N, result & (1 << 7));
}
// Branch if Negative
void MOS6502::m6502::BMI()
{
	BranchIf(N, 1);
}
// Branch on not equal
void MOS6502::m6502::BNE()
{
	BranchIf(Z, 0);
}
// Branch if Positive
void MOS6502::m6502::BPL()
{
	BranchIf(N, 0);
}
// Break
void MOS6502::m6502::BRK()
{
	PC++;

	stackPush((PC >> 8) & 0x00FF);
	stackPush(PC & 0x00FF);
	stackPush(SR | Statusflags::B);

	setFlag(I, 1);

	uint8_t pcl = bus->read(0xFFFE);
	uint8_t pch = bus->read(0xFFFF);
	PC = (pch << 8) | pcl;
}
// Branch on overflow clear
void MOS6502::m6502::BVC()
{
	BranchIf(V, 0);
}
// Branch on overflow set
void MOS6502::m6502::BVS()
{
	BranchIf(V, 1);
}
// Clear Carry flag
void MOS6502::m6502::CLC()
{
	setFlag(C, 0);
}
// Clear Decimal flag
void MOS6502::m6502::CLD()
{
	setFlag(D, 0);
}
// Clear Interrupt flag
void MOS6502::m6502::CLI()
{
	setFlag(I, 0);
}
// Clear overflow flag
void MOS6502::m6502::CLV()
{
	setFlag(V, 0);
}
// Compare accumulator
void MOS6502::m6502::CMP()
{
	setFlag(C, A >= opd);
	uint8_t result = A - opd;
	setFlag(Z, result == 0x00);
	setFlag(N, result & 0x80);
}
// Compare X
void MOS6502::m6502::CPX()
{
	setFlag(C, X >= opd);
	uint8_t result = X - opd;
	setFlag(Z, result == 0x00);
	setFlag(N, result & 0x80);
}
// Compare Y
void MOS6502::m6502::CPY()
{
	setFlag(C, Y >= opd);
	uint8_t result = Y - opd;
	setFlag(Z, result == 0x00);
	setFlag(N, result & 0x80);
}
// Decrement memory
void MOS6502::m6502::DEC()
{
	uint8_t result = opd - 1;
	setFlag(Z, result == 0x00);
	setFlag(N, result & 0x80);
	bus->write(fetchedAdr, result);
}
// Decrement X
void MOS6502::m6502::DEX()
{
	X--;
	setFlag(Z, X == 0x00);
	setFlag(N, X & 0x80);
}
// Decrement index Y
void MOS6502::m6502::DEY()
{
	Y--;
	setFlag(Z, Y == 0x00);
	setFlag(N, Y & 0x80);
}
// Exclusive Or
void MOS6502::m6502::EOR()
{
	A ^= opd;
	setFlag(Z, A == 0x00);
	setFlag(N, A & 0x80);
}
// Increment memory
void MOS6502::m6502::INC()
{
	uint8_t result = opd + 1;
	setFlag(Z, result == 0x00);
	setFlag(N, result & 0x80);
	bus->write(fetchedAdr, result);
}
// Increment X
void MOS6502::m6502::INX()
{
	X++;
	setFlag(Z, X == 0x00);
	setFlag(N, X & 0x80);
}
// Increment Y
void MOS6502::m6502::INY()
{
	Y++;
	setFlag(Z, Y == 0x00);
	setFlag(N, Y & 0x80);
}
// Jump
void MOS6502::m6502::JMP()
{
	PC = fetchedAdr;
}
// Jump to subroutine
void MOS6502::m6502::JSR()
{
	PC--;
	stackPush((PC >> 8) && 0x00FF);
	stackPush(PC & 0x00FF);
	PC = fetchedAdr;
}
// Load Accumulator
void MOS6502::m6502::LDA()
{
	A = opd;
	setFlag(Z, A == 0x00);
	setFlag(N, A & 0x80);
}
// Load index X
void MOS6502::m6502::LDX()
{
	X = opd;
	setFlag(Z, X == 0x00);
	setFlag(N, X & 0x80);
}
// Load index Y
void MOS6502::m6502::LDY()
{
	Y = opd;
	setFlag(Z, Y == 0x00);
	setFlag(N, Y & 0x80);
}
// Logical shift right
void MOS6502::m6502::LSR()
{
	setFlag(C, opd & 0x01);
	uint8_t result = opd >> 1;
	setFlag(Z, result == 0x00);
	setFlag(N, result & 0x80);

	if (opsLUT[IR].fetch == &m6502::ACC)
	{
		A = result;
	}
	else
	{
		bus->write(fetchedAdr, result);
	}
}
// No Operation
void MOS6502::m6502::NOP()
{
	return;
}
// Logical OR
void MOS6502::m6502::ORA()
{
	A |= opd;
	setFlag(Z, A == 0x00);
	setFlag(N, A & 0x80);
}
// Push Accumulator
void MOS6502::m6502::PHA()
{
	stackPush(A);
}
// Push Status Register
void MOS6502::m6502::PHP()
{
	stackPush(SR | B | _);
}
// Pull Accumulator
void MOS6502::m6502::PLA()
{
	A = stackPop();
	setFlag(Z, A == 0x00);
	setFlag(N, A & 0x80);
}
// Pull Status Register
void MOS6502::m6502::PLP()
{
	SR = stackPop();
	setFlag(_, 1);	// unused flag is always set
	setFlag(B, 0);
}
// Rotate left
void MOS6502::m6502::ROL()
{
	uint16_t result = opd << 1;
	result |= getFlag(C);

	setFlag(C, result & 0xFF00);
	setFlag(N, result & 0x0080);
	setFlag(Z, (result & 0x00FF) == 0x00);

	if (opsLUT[IR].fetch == &m6502::ACC)
	{
		A = result & 0x00FF;
	}
	else
	{
		bus->write(fetchedAdr, result & 0x00FF);
	}
}
// Rotate Right
void MOS6502::m6502::ROR()
{
	uint8_t result = opd >> 1;
	result |= getFlag(C) << 7;

	setFlag(C, opd & 0x01);
	setFlag(N, result & 0x80);
	setFlag(Z, result == 0x00);

	if (opsLUT[IR].fetch == &m6502::ACC)
	{
		A = result;
	}
	else
	{
		bus->write(fetchedAdr, result);
	}
}
// Return from interrupt
void MOS6502::m6502::RTI()
{
	SR = stackPop();
	setFlag(B, 0);

	uint8_t pcl = stackPop();
	uint8_t pch = stackPop();
	PC = (pch << 8) | pcl;
}
// Return from subroutine
void MOS6502::m6502::RTS()
{
	uint8_t pcl = stackPop();
	uint8_t pch = stackPop();
	PC = (pch << 8) | pcl;
	PC++;
}
// Substract with carry
void MOS6502::m6502::SBC()
{
	uint8_t ones_cmp = ~opd;

	uint8_t lo_nibble = (A & 0x0F) + (ones_cmp & 0x0F) + getFlag(C);
	uint8_t hi_nibble = (A >> 4) + (ones_cmp >> 4) + (lo_nibble > 0x0F);
	uint16_t result_16 = (hi_nibble << 4) | (lo_nibble & 0x0F);
	uint8_t result_8 = result_16 & 0x00FF;

	setFlag(C, result_16 & 0xFF00);
	setFlag(N, result_8 & 0x80);
	setFlag(Z, result_8 == 0x00);
	setFlag(V, (~(A ^ ones_cmp) & (A ^ result_8)) & 0x80);

	if (getFlag(D))
	{
		if ((lo_nibble & 0x10) == 0) lo_nibble += 0x0A;
		if ((hi_nibble & 0x10) == 0) hi_nibble += 0x0A;

		result_8 = (hi_nibble << 4) | (lo_nibble & 0x0F);
	}

	A = result_8;
}
// Set carry
void MOS6502::m6502::SEC()
{
	setFlag(C, 1);
}
// Set decimal
void MOS6502::m6502::SED()
{
	setFlag(D, 1);
}
// Set interrupt
void MOS6502::m6502::SEI()
{
	setFlag(I, 1);
}
// Store Accumulator
void MOS6502::m6502::STA()
{
	bus->write(fetchedAdr, A);
}
// Store index X
void MOS6502::m6502::STX()
{
	bus->write(fetchedAdr, X);
}
// Store index Y
void MOS6502::m6502::STY()
{
	bus->write(fetchedAdr, Y);
}
// Transfer A to X
void MOS6502::m6502::TAX()
{
	X = A;
	setFlag(Z, X == 0x00);
	setFlag(N, X & 0x80);
}
// Transfer A to Y
void MOS6502::m6502::TAY()
{
	Y = A;
	setFlag(Z, Y == 0x00);
	setFlag(N, Y & 0x80);
}
// Transfer Stack Pointer to X
void MOS6502::m6502::TSX()
{
	X = SP;
	setFlag(Z, X == 0x00);
	setFlag(N, X & 0x80);
}
// Transfer X to A
void MOS6502::m6502::TXA()
{
	A = X;
	setFlag(Z, A == 0x00);
	setFlag(N, A & 0x80);
}
// Transfer X to Stack Pointer
void MOS6502::m6502::TXS()
{
	SP = X;
}
// Transfer Y to A
void MOS6502::m6502::TYA()
{
	A = Y;
	setFlag(Z, A == 0x00);
	setFlag(N, A & 0x80);
}
// Illegal opcodes point to this
void MOS6502::m6502::___()
{
}

