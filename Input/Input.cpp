#include "Input.h"

namespace fisk::input
{
	Input::Input(std::string aConfigPath, fisk::tools::Port aPort)
		: myConfigFilePath(aConfigPath)
		, myListenSocket(aPort)
	{
		myNewSocketEventRegistration = myListenSocket.OnNewConnection.Register(std::bind(&Input::OnNewSocket, this, std::placeholders::_1));
	}

	bool Input::Update()
	{
		if (!myListenSocket.Update())
			return false;

		for (int i = static_cast<int>(myDevices.size()) - 1; i >= 0; i--)
		{
			if (!myDevices[i]->Update())
			{
				OnInputDeviceDisconnected.Fire(myDevices[i]->GetName());
				myDevices.erase(myDevices.begin() + i);
			}
		}

		return true;
	}

	fisk::tools::Port Input::GetPort() const
	{
		return myListenSocket.GetPort();
	}

	void Input::SaveConfig()
	{

	}

	std::vector<std::shared_ptr<InputDevice>>& Input::GetDevices()
	{
		return myDevices;
	}

	void Input::OnNewSocket(std::shared_ptr<fisk::tools::TCPSocket> aSocket)
	{
		std::shared_ptr<InputDevice> device = std::make_shared<InputDevice>(aSocket);
		device->OnSetUp.Register([this](std::string aName, std::vector<std::string> aChannels)
		{
			OnNewInputDevice.Fire(aName, aChannels);
		});

		myDevices.push_back(device);

	}

	InputDevice::InputDevice(std::shared_ptr<fisk::tools::TCPSocket> aSocket)
		: mySocket(aSocket)
		, myStreamReader(mySocket->GetReadStream())
	{
		myDataEventRegistration = mySocket->OnDataAvailable.Register(std::bind(&InputDevice::OnData, this));
	}

	bool InputDevice::Update()
	{
		if (!mySocket->Update())
			return false;

		return true;
	}

	std::vector<InputDevice::Channel>& InputDevice::GetChannels()
	{
		return myChannels;
	}

	const std::string& InputDevice::GetName()
	{
		return myName;
	}

	void InputDevice::OnData()
	{
		mySocket->GetReadStream().RestoreRead();

		if (!myIsSetUp)
		{
			if (!myStreamReader.Process(myName))
				return;

			InputChannel channelCount;

			if (!myStreamReader.Process(channelCount))
				return;

			if (channelCount == InvalidChannel)
			{
				mySocket->Close();
				return;
			}

			myChannels.resize(channelCount);

			for (Channel& channel : myChannels)
				if (!myStreamReader.Process(channel.myName))
					return;

			std::vector<std::string> channelNames;
			channelNames.reserve(myChannels.size());
			for (Channel& channel : myChannels)
				channelNames.push_back(channel.myName);

			OnSetUp.Fire(myName, channelNames);
			myIsSetUp = true;
			mySocket->GetReadStream().CommitRead();
		}


		while (true)
		{
			InputChannel channel;
			InputValue value;

			if (!myStreamReader.Process(channel))
				break;

			if (!myStreamReader.Process(value))
				break;

			mySocket->GetReadStream().CommitRead();

			if (channel >= myChannels.size())
			{
				mySocket->Close();
				return;
			}

			myChannels[channel].OnChanged.Fire(InputValueToFloat(value));
		}
	}
}
