#ifndef FISK_INPUT_INPUT_INPUT_H
#define FISK_INPUT_INPUT_INPUT_H

#include "common/Types.h"

#include "tools/Event.h"
#include "tools/StreamReader.h"
#include "tools/TCPListenSocket.h"
#include "tools/TCPSocket.h"


namespace fisk::input
{
	class InputDevice;
	class Action;
	class DigitalAction;
	class AnalogueAction;

	class Input
	{
	public:
		static std::string DefaultConfigPath();

		Input(std::string aConfigPath = DefaultConfigPath(), fisk::tools::Port aPort = DefaultPort);

		bool Update();
		fisk::tools::Port GetPort() const;

		void RegisterAction(DigitalAction* aAction, std::string aName);
		void RegisterAction(AnalogueAction* aAction, std::string aName);

		void SaveConfig();

		fisk::tools::Event<std::string, std::vector<std::string>> OnNewInputDevice;
		fisk::tools::Event<std::string> OnInputDeviceDisconnected;

		std::vector<std::shared_ptr<InputDevice>>& GetDevices();

	private:
		void OnNewSocket(std::shared_ptr<fisk::tools::TCPSocket> aSocket);

		std::string myConfigFilePath;

		std::unordered_map<std::string, Action*> myActions;
		std::unordered_map<std::string, std::vector<std::string>> myLoadedPreferences;

		std::vector<std::shared_ptr<InputDevice>> myDevices;

		fisk::tools::TCPListenSocket myListenSocket;
		fisk::tools::EventReg myNewSocketEventRegistration;
	};

	class InputDevice
	{
	public:
		InputDevice(std::shared_ptr<fisk::tools::TCPSocket> aSocket);

		bool Update();

		struct Channel
		{
			fisk::tools::Event<float> OnChanged;
			std::string myName;
		};

		fisk::tools::SingleFireEvent<std::string, std::vector<std::string>> OnSetUp;

		std::vector<Channel>& GetChannels();
		const std::string& GetName();

	private:
		void OnData();

		std::shared_ptr<fisk::tools::TCPSocket> mySocket;
		fisk::tools::EventReg myDataEventRegistration;

		fisk::tools::StreamReader myStreamReader;

		bool myIsSetUp = false;
		std::vector<Channel> myChannels;
		std::string myName;
	};

	class Action
	{
	public:
		virtual ~Action() = default;
		virtual void AttachTo(InputDevice::Channel& aChannel) = 0;

		std::vector<std::string> myWantedChannels;
	};



}

#endif