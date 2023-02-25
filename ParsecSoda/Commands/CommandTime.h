#pragma once

#include <iostream>
#include <sstream>
#include "ACommand.h"
#include "../GamepadClient.h"
#include "../HotseatManager.h"
#include "../Guest.h"

class CommandTime : public ACommand
{
public:
	const COMMAND_TYPE type() override { return COMMAND_TYPE::HOTSEATTIME; }

	CommandTime(Guest &sender, HotseatManager& hotseatManager)
		: _sender(sender), _hotseatManager(hotseatManager), _droppedPadCount(0)
	{}

	bool run() override
	{
		bool result = false;
		static const float FILL_FRACTION_INVERSE = 1.0f/0.25f;

		_hotseatManager.runThreadSafe([&]() {

			if (!_hotseatManager.isQueueEmpty())
			{
				_replyMessage = "[ChatBot]\t" + _sender.name + ", waiting queue must be empty to refresh timer.\0";
			}
			else
			{
				bool found = _hotseatManager.findSeatByGuest(_sender.userID, [&](Hotseat& seat) {
					if (seat.timer.getRemainingTime() * FILL_FRACTION_INVERSE > seat.timer.getDuration())
					{
						_replyMessage = "[ChatBot]\t" + _sender.name + ", your seat timer must be below 25% to run this command.\0";
					}
					else
					{
						_hotseatManager.refresh(seat);
						_replyMessage = "[ChatBot]\t" + _sender.name + ", seat timer refreshed.\0";
						result = true;
					}
				});

				if (!found)
				{
					_replyMessage = "[ChatBot]\t" + _sender.name + ", you must be in a seat to run this command.\0";
				}
			}
		});

		return result;
	}

	static vector<const char*> prefixes()
	{
		return vector<const char*> { "!time", "!cd" };
	}

protected:
	Guest& _sender;
	HotseatManager& _hotseatManager;
	int _droppedPadCount;
};