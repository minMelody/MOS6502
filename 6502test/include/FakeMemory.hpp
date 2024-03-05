#pragma once

#include <vector>
#include "IAddressable.h"

namespace MOS6502
{
	namespace UnitTests
	{
		struct FakeMemory : IAddressable
		{
		public:
			FakeMemory(unsigned int size);
			~FakeMemory();
			std::vector<uint8_t> data;
			uint16_t size();
			uint8_t read(uint16_t adr);
			void write(uint16_t adr, uint8_t byte);
		};
	}
}
