#pragma once

#include <cstdint>
#include <vector>
#include "IAddressable.h"

namespace MOS6502
{
	struct Bus
	{
	public:
		uint8_t read(uint16_t address);
		void write(uint16_t address, uint8_t data);
		void connect(IAddressable* addressable, uint16_t startAdr);
	private:
		std::vector<IAddressable*> addressSpace = {};
		IAddressable* getAddressable(uint16_t adr);
	};
}