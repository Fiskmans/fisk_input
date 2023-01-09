
#include <catch2/catch_all.hpp>

#include "fisk_input/Input.h"

#include "cpp_output/Output.h"

#include "tools/EggClock.h"
using namespace std::chrono_literals;

TEST_CASE("InputValue", "[input]")
{
	for (float i = -1.f; i <= 1.003f; i += 0.001f)
		REQUIRE(fisk::input::InputValueToFloat(fisk::input::FloatToInputValue(i)) == Catch::Approx(i).margin(.6f / 128.f));
}


TEST_CASE("Input", "[input]")
{
	{
		fisk::input::Input constructDestruct;
	}

	fisk::input::Input input;

	REQUIRE(input.GetPort() == fisk::input::DefaultPort);

	std::string deviceName;
	std::vector<std::string> channels;

	fisk::tools::EventReg deviceEventReg = input.OnNewInputDevice.Register(
		[&deviceName, &channels](fisk::input::InputDevice& aDevice)
	{
		deviceName = aDevice.GetName();

		for (std::unique_ptr<fisk::input::InputDevice::Channel>& channel : aDevice.GetChannels())
			channels.push_back(channel->myName);
	});

	fisk::tools::EggClock timer(500ms);

	bool inputUpdate = true;
	bool outputUpdate = true;

	fisk::output::Output output(std::make_shared<fisk::tools::TCPSocket>("localhost", std::to_string(fisk::input::DefaultPort).c_str(), 200ms), "test", {"channel_1", "channel_2", "channel_3" });
	while (deviceName.empty())
	{
		inputUpdate &= input.Update();
		outputUpdate &= output.Update();

		if (timer.IsDone())
			break;

		if (!inputUpdate)
			break;

		if (!outputUpdate)
			break;
	}

	REQUIRE(!timer.IsDone());
	REQUIRE(inputUpdate);
	REQUIRE(outputUpdate);

	REQUIRE(deviceName == "test");
	
	REQUIRE(channels.size() == 3);
	REQUIRE(channels[0] == "channel_1");
	REQUIRE(channels[1] == "channel_2");
	REQUIRE(channels[2] == "channel_3");

	REQUIRE(input.GetDevices().size() == 1);
	REQUIRE(input.GetDevices()[0]->GetName() == "test");

	REQUIRE(input.GetDevices()[0]->GetChannels().size() == 3);
	REQUIRE(input.GetDevices()[0]->GetChannels()[0]->myName == "channel_1");
	REQUIRE(input.GetDevices()[0]->GetChannels()[1]->myName == "channel_2");
	REQUIRE(input.GetDevices()[0]->GetChannels()[2]->myName == "channel_3");

	double c1 = 0.f;
	double c3 = 0.f;

	output.UpdateChannel(0, fisk::input::InputValueMin);
	output.UpdateChannel(1, fisk::input::InputValueZero);
	output.UpdateChannel(2, fisk::input::InputValueMax);

	bool update1 = false;
	bool update3 = false;

	bool doubleEvent = false;

	fisk::tools::EventReg channel1 = input.GetDevices()[0]->GetChannels()[0]->OnChanged.Register([&update1, &doubleEvent, &c1](float aValue)
	{
		if (update1)
			doubleEvent = true;

		update1 = true;
		c1 = aValue;
	});

	fisk::tools::EventReg channel2 = input.GetDevices()[0]->GetChannels()[1]->OnChanged.Register([&doubleEvent](float aValue)
	{
		doubleEvent = true;
	});

	fisk::tools::EventReg channel3 = input.GetDevices()[0]->GetChannels()[2]->OnChanged.Register([&update3, &doubleEvent, &c3](float aValue)
	{
		if (update3)
			doubleEvent = true;

		update3 = true;
		c3 = aValue;
	});


	while (!(update1 && update3))
	{
		inputUpdate &= input.Update();
		outputUpdate &= output.Update();

		if (timer.IsDone())
			break;

		if (!inputUpdate)
			break;

		if (!outputUpdate)
			break;
	}

	REQUIRE(!timer.IsDone());
	REQUIRE(inputUpdate);
	REQUIRE(outputUpdate);

	REQUIRE(!doubleEvent);

	REQUIRE(c1 == Catch::Approx(-1.0));
	REQUIRE(c3 == Catch::Approx(1.0));
}

TEST_CASE("Action", "[input]")
{
	fisk::input::DigitalAction action1;
	action1.myWantedChannels = { "test.channel_1" };
	
	fisk::input::Input input;
	input.RegisterAction(action1, "action-1");

	bool inputUpdate = true;
	bool outputUpdate = true;

	bool pressed = false;
	bool released = false;
	bool inOrder = true;

	fisk::tools::EggClock timer(500ms);

	fisk::tools::EventReg actionPressedReg = action1.OnPressed.Register([&pressed, &released, &inOrder]()
	{
		if (released)
			inOrder = false;

		if (pressed)
			inOrder = false;

		pressed = true;
	});

	fisk::tools::EventReg actionReleasedReg = action1.OnPressed.Register([&pressed, &released, &inOrder]()
	{
		if (released)
			inOrder = false;

		if (!pressed)
			inOrder = false;

		 released = true;
	});

	fisk::output::Output output(std::make_shared<fisk::tools::TCPSocket>("localhost", std::to_string(fisk::input::DefaultPort).c_str(), 200ms), "test", { "channel_1"});

	output.UpdateChannel(0, 1.f);
	output.UpdateChannel(0, 0.f);


	while (!pressed || !released)
	{
		inputUpdate &= input.Update();
		outputUpdate &= output.Update();

		if (timer.IsDone())
			break;

		if (!inputUpdate)
			break;

		if (!outputUpdate)
			break;

		if (!inOrder)
			break;
	}

	REQUIRE(!timer.IsDone());
	REQUIRE(inputUpdate);
	REQUIRE(outputUpdate);

	REQUIRE(pressed);
	REQUIRE(released);
	REQUIRE(inOrder);
}