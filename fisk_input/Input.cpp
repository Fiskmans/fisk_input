#include "fisk_input/Input.h"

#include "tools/File.h"
#include "tools/Json.h"
#include "tools/Logger.h"

#include <filesystem>

namespace fisk::input
{
	Input::Input(fisk::tools::Port aPort)
		: myListenSocket(aPort)
	{
		myNewSocketEventRegistration = myListenSocket.OnNewConnection.Register(std::bind(&Input::OnNewSocket, this, std::placeholders::_1));

		LoadConfig(LocalConfigFile);
	}

	bool Input::Update()
	{
		if (!myListenSocket.Update())
			return false;

		for (int i = static_cast<int>(myDevices.size()) - 1; i >= 0; i--)
		{
			if (!myDevices[i]->Update())
			{
				OnDeviceDisconnected(*myDevices[i]);
				myDevices.erase(myDevices.begin() + i);
			}
		}

		return true;
	}

	fisk::tools::Port Input::GetPort() const
	{
		return myListenSocket.GetPort();
	}

	void Input::RegisterAction(Action& aAction, std::string aName)
	{
		decltype(myLoadedPreferences)::iterator it = myLoadedPreferences.find(aName);
		if (it != myLoadedPreferences.end())
			aAction.myWantedChannels = it->second;

		myActions[aName] = &aAction;
	}

	void Input::SaveConfig()
	{
		fisk::tools::Json root;

		root.AddValue("extends", myParentFile);
		root.AddValue("version", CurrentVersion);

		fisk::tools::Json& actions = root.AddChild("actions");

		for (const auto& [key, value] : myActions)
		{
			fisk::tools::Json& action = actions.AddChild(key);

			for (std::string& channel : value->myWantedChannels)
				action.PushValue(channel);
		}
	}

	std::vector<std::shared_ptr<InputDevice>>& Input::GetDevices()
	{
		return myDevices;
	}

	void Input::LoadConfig(std::string aFilePath)
	{
		LOG_SYS_INFO("Loading input mapping config", aFilePath);

		if (!std::filesystem::exists(aFilePath))
		{
			LOG_SYS_WARNING("Input mapping file missing", aFilePath);
			return;
		}

		std::string fileContent = fisk::tools::ReadWholeFile(aFilePath);

		if (fileContent.empty())
		{
			LOG_SYS_WARNING("Input mapping file empty", aFilePath);
			return;
		}

		fisk::tools::Json root;
		if (!root.Parse(fileContent.c_str()))
		{
			LOG_SYS_WARNING("Input mapping file not valid json", aFilePath);
			return;
		}

		std::string parent;
		if (root["extends"].GetIf(parent))
			LoadConfig(parent);

		int version;
		if (!root["version"].GetIf(version))
		{
			LOG_SYS_WARNING("Input mapping file missing version number", aFilePath);
			return;
		}

		if (version != CurrentVersion)
		{
			LOG_SYS_WARNING("Input mapping file missing wrong version", "file: " + aFilePath, "Was: " + std::to_string(version) + " expected: " + std::to_string(CurrentVersion));
			return;
		}

		for (const auto& [key, value] : root["actions"].IterateObject())
		{
			std::vector<std::string> channels;
			for (fisk::tools::Json& channel : value.IterateArray())
			{
				std::string c;
				if (!channel.GetIf(c))
				{
					LOG_SYS_WARNING("Input mapping file not valid", "File: " + aFilePath, "Action [" + key + "] contained non-string value");
					return;
				}
				channels.push_back(c);
			}

			myLoadedPreferences[key] = channels;
		}
	}

	void Input::OnNewSocket(std::shared_ptr<fisk::tools::TCPSocket> aSocket)
	{
		std::shared_ptr<InputDevice> device = std::make_shared<InputDevice>(aSocket);
		device->OnSetUp.Register(std::bind(&Input::OnDeviceSetUp, this, std::placeholders::_1));

		myDevices.push_back(device);
	}

	void Input::OnDeviceSetUp(InputDevice& aDevice)
	{
		for (const auto& [key, action] : myActions)
			action->OnDeviceSetUp(aDevice);

		OnNewInputDevice.Fire(aDevice);
	}

	void Input::OnDeviceDisconnected(InputDevice& aDevice)
	{
		for (const auto& [key, action] : myActions)
			action->OnDeviceDisconnected(aDevice);

		OnInputDeviceDisconnected.Fire(aDevice.GetName());
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

	std::vector<std::unique_ptr<InputDevice::Channel>>& InputDevice::GetChannels()
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

			for (std::unique_ptr<Channel>& channel : myChannels)
				if (!channel)
					channel = std::make_unique<Channel>();

			for (std::unique_ptr<Channel>& channel : myChannels)
				if (!myStreamReader.Process(channel->myName))
					return;

			OnSetUp.Fire(*this);
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

			float newValue = InputValueToFloat(value);
			myChannels[channel]->myCurrentValue = newValue;
			myChannels[channel]->OnChanged.Fire(newValue);
		}
	}

	void Action::OnDeviceSetUp(InputDevice& aDevice)
	{
		for (std::string& wanted : myWantedChannels)
		{
			if (wanted == myBoundTo)
				break;

			if (!wanted.starts_with(aDevice.GetName()))
				continue;
			
			for (std::unique_ptr<InputDevice::Channel>& channel : aDevice.GetChannels())
			{
				if (wanted.ends_with(channel->myName))
				{
					BindTo(*channel, wanted);
					myBoundTo = wanted;
					return;
				}
			}
		}
	}

	void Action::OnDeviceDisconnected(InputDevice& aDevice)
	{
		if (myBoundTo.empty())
			return;

		// TODO: unbind from the channel;
	}

	bool DigitalAction::IsHeld()
	{
		return myIsHeld;
	}

	void DigitalAction::BindTo(InputDevice::Channel& aChannel, std::string aName)
	{
		OnChange(0.f);
		myChannelEventReg = aChannel.OnChanged.Register(std::bind(&DigitalAction::OnChange, this, std::placeholders::_1));
	}

	void DigitalAction::OnChange(float aNewValue)
	{
		if (myIsHeld)
		{
			if (aNewValue < ReleaseThreshhold)
			{
				OnReleased.Fire();
				myIsHeld = false;
			}
		}
		else
		{
			if (aNewValue > PressThreshhold)
			{
				OnPressed.Fire();
				myIsHeld = true;
			}
		}
	}
}
