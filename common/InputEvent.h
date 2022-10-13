#ifndef FISK_INPUT_INPUT_EVENT_H
#define FISK_INPUT_INPUT_EVENT_H

#include "Input/Types.h"

namespace fisk::input
{
	struct InputEvent
	{
	public:
		Channel myChannel = InvalidChannel;
		Value myValue = InvalidValue;

		void Process()
	};
}

#endif