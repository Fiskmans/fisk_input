#ifndef FISK_INPUT_TYPES_H
#define FISK_INPUT_TYPES_H

#include "tools/Socket.h"

#include <cmath>
#include <cstdint>
#include <limits>

namespace fisk::input
{
	constexpr fisk::tools::Port DefaultPort = 36212;

	using InputChannel = uint8_t;
	constexpr InputChannel InvalidChannel = std::numeric_limits<InputChannel>::max();

	using InputValue = uint8_t;
	constexpr InputValue InputValueMin = std::numeric_limits<InputValue>::lowest();
	constexpr InputValue InputValueZero = 128;
	constexpr InputValue InputValueMax = std::numeric_limits<InputValue>::max();

	constexpr float InputValueToFloat(InputValue aValue)
	{
		if (aValue < InputValueZero)
			return static_cast<float>(InputValueZero - aValue) / static_cast<float>(InputValueZero - InputValueMin) * -1.f;

		return static_cast<float>(aValue - InputValueZero) / static_cast<float>(InputValueMax - InputValueZero);
	}

	constexpr InputValue FloatToInputValue(float aValue)
	{
		if (aValue > 1.f)
			return InputValueMax;

		if (aValue < -1.f)
			return InputValueMin;

		int scaled;
		if (aValue < 0.f)
		{
			scaled = static_cast<int>(::round((1.f + aValue) * static_cast<float>(InputValueZero - InputValueMin)));
		}
		else
		{
			scaled = static_cast<int>(::round(aValue * static_cast<float>(InputValueMax - InputValueZero)) + InputValueZero);
		}

		if (scaled > InputValueMax)
			return InputValueMax;

		if (scaled < InputValueMin)
			return InputValueMin;

		return static_cast<InputValue>(scaled);
	}
}

#endif