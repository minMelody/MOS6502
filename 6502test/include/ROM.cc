#include "ROM.hpp"
#include <iterator>
#include <stdexcept>

MOS6502::ROM::ROM()
{
	std::fill(std::begin(data), std::end(data), 0xea);
}

MOS6502::ROM::~ROM()
{
}

uint16_t MOS6502::ROM::size()
{
	return std::size(data);
}

uint8_t MOS6502::ROM::read(uint16_t adr)
{
	return data[adr];
}

void MOS6502::ROM::write(uint16_t adr, uint8_t byte)
{
	throw std::runtime_error("Cannot write to read-only memory.");
}
