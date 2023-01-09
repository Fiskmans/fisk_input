#ifndef FISK_INPUT_OUTPUT_OUTPUT_H
#define FISK_INPUT_OUTPUT_OUTPUT_H

#include "fisk_input_common/Types.h"

#include "tools/StreamWriter.h"
#include "tools/TCPSocket.h"

#include <memory>
#include <vector>

namespace fisk::output
{
	class Output
	{
	public:
		Output(std::shared_ptr<fisk::tools::TCPSocket> aSocket, std::string aDeviceName, std::vector<std::string> aChannels);

		bool Update();

		void UpdateChannel(fisk::input::InputChannel aIndex, float aValue);
		void UpdateChannel(fisk::input::InputChannel aIndex, fisk::input::InputValue aValue);

	private:

		std::shared_ptr<fisk::tools::TCPSocket> mySocket;

		fisk::tools::StreamWriter myStreamWriter;

		std::vector<fisk::input::InputValue> myChannels;
	};

}

#endif