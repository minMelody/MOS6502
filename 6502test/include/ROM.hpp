#pragma once

#include "IAddressable.h"

namespace MOS6502
{
	struct ROM : IAddressable
	{
	public:
		ROM();
		~ROM();
		uint16_t size();
		uint8_t read(uint16_t adr);
		void write(uint16_t adr, uint8_t byte);
	private:
		uint8_t data[0x8000];
	};
}