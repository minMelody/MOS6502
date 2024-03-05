#include <iterator>
#include "FakeMemory.hpp"

MOS6502::UnitTests::FakeMemory::FakeMemory(unsigned int size)
{
    data.resize(size);
    std::fill(std::begin(data), std::end(data), 0x00);
}

MOS6502::UnitTests::FakeMemory::~FakeMemory()
{
}

uint16_t MOS6502::UnitTests::FakeMemory::size()
{
    return data.size();
}

uint8_t MOS6502::UnitTests::FakeMemory::read(uint16_t adr)
{
    return data[adr];
}

void MOS6502::UnitTests::FakeMemory::write(uint16_t adr, uint8_t byte)
{
    data[adr] = byte;
}