#pragma once

#include <string>
#include <sstream>
#include <vector>
#include <regex>
#include "parsec.h"
#include "ParsecSession.h"
#include "Stringer.h"
#include "GamepadClient.h"
#include "DX11.h"

#include "Commands/ACommand.h"
#include "Commands/CommandAFK.h"
#include "Commands/CommandBan.h"
#include "Commands/CommandBonk.h"
#include "Commands/CommandBotMessage.h"
#include "Commands/CommandDefaultMessage.h"
#include "Commands/CommandDC.h"
#include "Commands/CommandFF.h"
#include "Commands/CommandGameId.h"
#include "Commands/CommandGuests.h"
#include "Commands/CommandHelp.h"
#include "Commands/CommandIpFilter.h"
#include "Commands/CommandJoin.h"
#include "Commands/CommandKick.h"
#include "Commands/CommandMic.h"
#include "Commands/CommandMirror.h"
#include "Commands/CommandName.h"
#include "Commands/CommandLimit.h"
#include "Commands/CommandPads.h"
#include "Commands/CommandPrivate.h"
#include "Commands/CommandPublic.h"
#include "Commands/CommandQuit.h"
#include "Commands/CommandSetConfig.h"
#include "Commands/CommandSpeakers.h"
#include "Commands/CommandSwap.h"
#include "Commands/CommandUnban.h"
#include "Commands/CommandVideoFix.h"

#define BOT_GUESTID 0

class ChatBot
{
public:
	ChatBot(
		AudioMix& audioMixer, BanList& ban, Dice& dice, DX11 &dx11,
		GamepadClient& gamepadClient, GuestList &guests, Parsec* parsec, ParsecHostConfig &hostConfig,
		ParsecSession parsecSession, bool &hostingLoopController
	)
		: _audioMixer(audioMixer), _ban(ban), _dice(dice), _dx11(dx11), _gamepadClient(gamepadClient),
		_guests(guests), _parsec(parsec), _hostConfig(hostConfig), _parsecSession(parsecSession),
		_hostingLoopController(hostingLoopController)
	{}

	ACommand * identifyUserDataMessage(const char* msg, Guest& sender, bool& isAdmin);

	const uint32_t getLastUserId() const;
	void setLastUserId(const uint32_t lastId = BOT_GUESTID);
	const std::string formatGuestConnection(Guest guest, ParsecGuestState state, bool isAdmin = false);
	const std::string formatBannedGuestMessage(Guest guest);
	CommandBotMessage sendBotMessage(const char * msg);

private:
	static bool msgStartsWith(const char* msg, const char* pattern);
	static bool msgStartsWith(const char* msg, vector<const char*> patterns);
	static bool msgIsEqual(const char* msg, const char* pattern);
	static bool msgIsEqual(const char* msg, vector<const char*> patterns);

	uint32_t _lastUserId = 0;

	// Dependency Injection
	Parsec* _parsec;
	AudioMix& _audioMixer;
	BanList &_ban;
	Dice &_dice;
	DX11 &_dx11;
	GamepadClient& _gamepadClient;
	GuestList& _guests;
	ParsecHostConfig &_hostConfig;
	ParsecSession &_parsecSession;
	bool &_hostingLoopController;
};