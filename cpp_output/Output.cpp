#include "Output.h"

#include "tools/StreamWriter.h"

#include <cassert>

fisk::output::Output::Output(std::shared_ptr<fisk::tools::TCPSocket> aSocket, std::string aDeviceName, std::vector<std::string> aChannels)
	: mySocket(aSocket)
	, myStreamWriter(mySocket->GetWriteStream())
{
	myStreamWriter.Process(aDeviceName);

	assert(aChannels.size() < fisk::input::InvalidChannel);

	fisk::input::InputChannel count = static_cast<fisk::input::InputChannel>(aChannels.size());
	myStreamWriter.Process(count);

	for (std::string& name : aChannels)
		myStreamWriter.Process(name);

	myChannels.resize(aChannels.size());

	for (fisk::input::InputChannel c = 0; c < myChannels.size(); c++)
	{
		fisk::input::InputValue v = fisk::input::InputValueZero;

		myStreamWriter.Process(c);
		myStreamWriter.Process(v);

		myChannels[c] = v;
	}

}

bool fisk::output::Output::Update()
{
	if (!mySocket->Update())
		return false;

	return true;
}

void fisk::output::Output::UpdateChannel(fisk::input::InputChannel aIndex, float aValue)
{
	UpdateChannel(aIndex, fisk::input::FloatToInputValue(aValue));
}

void fisk::output::Output::UpdateChannel(fisk::input::InputChannel aIndex, fisk::input::InputValue aValue)
{
	assert(aIndex < myChannels.size());

	if (myChannels[aIndex] != aValue)
	{
		myStreamWriter.Process(aIndex);
		myStreamWriter.Process(aValue);

		myChannels[aIndex] = aValue;
	}
}
