#include "TestChatBot.h"

using namespace ParsecSoda;

const char * TestChatBot::test()
{
	ChatBot bot;
	std::string reply;
	bool success;
	ParsecHostConfig config = EMPTY_HOST_CONFIG;

	CommandSetConfig cmd;
	cmd.run()
	if (!bot.isSetConfigMessage("/setconfig", &reply)) { return "/setconfig failed."; }
	if (reply.compare("[ChatBot] | Room settings applied.\0") != 0) { return "/setconfig gives wrong message."; }

	if (!bot.isQuitMessage("/q", &reply)) { return "/q failed."; }
	if (reply.compare("[ChatBot] | Closing stream...\0") != 0) { return "/q gives wrong message."; }

	if (!bot.isDX11RefreshMessage("/dx11refresh", &reply)) { return "/dx11refresh failed."; }
	if (reply.compare("[ChatBot] | Refreshing Directx11...\0") != 0) { return "/dx11refresh gives wrong message."; }

	success = bot.processHostConfigMessage("/private", &config, &reply);
	if (!success) { return "/private failed"; }
	if (reply.compare("[ChatBot] | Room set to private.\0") != 0) { return "/private gives wrong message."; }
	if (config.publicGame) { return "/private edited config file incorrectly."; }

	success = bot.processHostConfigMessage("/public", &config, &reply);
	if (!success) { return "/public failed"; }
	if (reply.compare("[ChatBot] | Room set to public.\0") != 0) { return "/public gives wrong message."; }
	if (!config.publicGame) { return "/public edited config file incorrectly."; }

	success = bot.processHostConfigMessage("/name My disappointment is immesurable, and my day is ruined.", &config, &reply);
	if (!success) { return "/name failed"; }
	if (reply.compare("[ChatBot] | Room name changed:\nMy disappointment is immesurable, and my day is ruined.\0") != 0)
	{
		return "/private gives wrong message.";
	}
	if (strcmp(config.name, "My disappointment is immesurable, and my day is ruined.") != 0) { return "/private edited config file incorrectly."; }

	success = bot.processHostConfigMessage("/gameid 1R7BARGGu3H72xyzvyCNqQ6sr4G", &config, &reply);
	if (!success) { return "/name failed"; }
	if (reply.compare("[ChatBot] | Game ID changed:\n1R7BARGGu3H72xyzvyCNqQ6sr4G\0") != 0) { return "/private gives wrong message."; }
	if (strcmp(config.gameID, "1R7BARGGu3H72xyzvyCNqQ6sr4G") != 0) { return "/private edited config file incorrectly."; }

	success = bot.processHostConfigMessage("/guests 17", &config, &reply);
	if (!success) { return "/guests failed"; }
	if (reply.compare("[ChatBot] | Max users set to 17\0") != 0) { return "/guests gives wrong message."; }
	if (config.maxGuests != 17) { return "/guest edited config file incorrectly."; }

	return NULL;
}
