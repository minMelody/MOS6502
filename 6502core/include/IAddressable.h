#pragma once

#include <cstdint>

namespace MOS6502
{
	struct IAddressable
	{
	public:
		uint16_t start;	// This component's first address in the address space.
		uint16_t end;	// This component's last address in the address space.
		virtual uint16_t size() = 0;

		virtual uint8_t read(uint16_t adr) = 0;
		virtual void write(uint16_t adr, uint8_t byte) = 0;
	};
}