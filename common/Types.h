#ifndef FISK_INPUT_TYPES_H
#define FISK_INPUT_TYPES_H

#include <cstdint>
#include <limits>

namespace fisk::input
{
	using Channel = uint16_t;
	constexpr Channel InvalidChannel = std::numeric_limits<Channel>::max();
	
	using Value = uint32_t;
	constexpr Value InvalidValue = std::numeric_limits<Value>::max();

	using StreamSize = uint32_t;
	constexpr StreamSize InvalidStreamSize = std::numeric_limits<StreamSize>::max();
}

#endif