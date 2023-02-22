#ifndef FISK_INPUT_INPUT_INPUT_H
#define FISK_INPUT_INPUT_INPUT_H

#include "fisk_input_common/Types.h"

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
		static constexpr const char* LocalConfigFile = "input_mapping.json";
		static constexpr int CurrentVersion = 1;

		struct DeviceInfo
		{
			std::string myName;
			std::vector<std::string> myChannels;
		};

		Input(fisk::tools::Port aPort = DefaultPort);

		bool Update();
		fisk::tools::Port GetPort() const;

		void RegisterAction(Action& aAction, std::string aName);

		void SaveConfig();

		fisk::tools::Event<InputDevice&> OnNewInputDevice;
		fisk::tools::Event<std::string> OnInputDeviceDisconnected;

		std::vector<std::shared_ptr<InputDevice>>& GetDevices();

	private:
		void LoadConfig(std::string aFilePath);
		void OnNewSocket(std::shared_ptr<fisk::tools::TCPSocket> aSocket);
		void OnDeviceSetUp(InputDevice& aDevice);
		void OnDeviceDisconnected(InputDevice& aDevice);

		std::unordered_map<std::string, Action*> myActions;
		std::unordered_map<std::string, std::vector<std::string>> myLoadedPreferences;

		std::vector<std::shared_ptr<InputDevice>> myDevices;

		fisk::tools::TCPListenSocket myListenSocket;
		fisk::tools::EventReg myNewSocketEventRegistration;

		std::string myParentFile = "%appdata%fisk/input/main_config.json";
	};

	class InputDevice
	{
	public:
		InputDevice(std::shared_ptr<fisk::tools::TCPSocket> aSocket);

		bool Update();

		struct Channel
		{
			fisk::tools::Event<float> OnChanged;
			float myCurrentValue;
			std::string myName;
		};

		fisk::tools::SingleFireEvent<InputDevice&> OnSetUp;

		std::vector<std::unique_ptr<Channel>>& GetChannels();
		const std::string& GetName();

	private:
		void OnData();

		std::shared_ptr<fisk::tools::TCPSocket> mySocket;
		fisk::tools::EventReg myDataEventRegistration;

		fisk::tools::StreamReader myStreamReader;

		bool myIsSetUp = false;
		std::vector<std::unique_ptr<Channel>> myChannels;
		std::string myName;
	};

	class Action
	{
	public:
		virtual ~Action() = default;
		virtual void BindTo(InputDevice::Channel& aChannel, std::string aName) = 0;

		void OnDeviceSetUp(InputDevice& aDevice);
		void OnDeviceDisconnected(InputDevice& aDevice);

		std::vector<std::string> myWantedChannels;
		std::string myBoundTo;
	};

	class DigitalAction
		: public Action
	{
	public:
		fisk::tools::Event<> OnPressed;
		fisk::tools::Event<> OnReleased;

		bool IsHeld();

	private:
		static constexpr float PressThreshhold = 0.7;
		static constexpr float ReleaseThreshhold = 0.3;

		void BindTo(InputDevice::Channel& aChannel, std::string aName) override;

		void OnChange(float aNewValue);

		bool myIsHeld = false;
		fisk::tools::EventReg myChannelEventReg;
	};

}

#endif