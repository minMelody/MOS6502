#include "RAM.hpp"
#include <iterator>

MOS6502::RAM::RAM()
{
	std::fill(std::begin(data), std::end(data), 0x00);
}

MOS6502::RAM::~RAM()
{
}

uint16_t MOS6502::RAM::size()
{
	return std::size(data);
}

uint8_t MOS6502::RAM::read(uint16_t adr)
{
	return data[adr];
}

void MOS6502::RAM::write(uint16_t adr, uint8_t byte)
{
	data[adr] = byte;
}
