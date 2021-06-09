#include "matoya.h"

#define D3D_DEBUG_INFO

#include <iostream>
#include <d3d11.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <thread>
#include <chrono>
#include "ParsecSession.h"
#include "DX11.h"
#include "AdminList.h"
#include "ChatBot.h"
#include "Stringer.h"
#include "StressTest.h"
#include "Mock.h"
#include "AudioIn.h"
#include "AudioOut.h"
#include "AudioMix.h"
#include "GamepadClient.h"
#include "BanList.h"
#include "Dice.h"

#define PARSEC_APP_CHAT_MSG 0

#define ROOM_NAME "Coding my own Parsec\nGamepad streaming\0"
#define ROOM_SECRET "melonsod"

const std::vector<int> admins { 3888558 , 6711547 };
const std::vector<GuestData> banned {};
std::vector<COMMAND_TYPE> filteredCommands { COMMAND_TYPE::IP };

// ============================================================
//
//  Enter the THREAD zone
//
// ============================================================
Dice dice;
GamepadClient padClient;
bool isRunning = true;
ParsecGuest* guests = NULL;
ParsecSoda::ParsecSession parsecSession;
AdminList adminList(admins);
BanList banList(banned);
ChatBot chatBot;
Parsec* ps;
DX11 dx11;
AudioOut audioOut;
AudioIn audioIn;
AudioMix audioMix(0.8, 0.3);
ParsecHostConfig hostConfig;

// ============================================================


using namespace ParsecSoda;

struct context {
	MTY_App *app;
	void *image;
	uint32_t image_w;
	uint32_t image_h;
	bool quit;
};

static void event_func(const MTY_Event *evt, void *opaque)
{
	struct context *ctx = (context*)opaque;

	MTY_PrintEvent(evt);

	if (evt->type == MTY_EVENT_CLOSE)
		ctx->quit = true;
}

static bool app_func(void *opaque)
{
	struct context *ctx = (context*)opaque;

	// Set up a render description for the PNG
	MTY_RenderDesc desc {};
	desc.format = MTY_COLOR_FORMAT_BGRA;
	desc.effect = MTY_EFFECT_SCANLINES;
	desc.imageWidth = ctx->image_w;
	desc.imageHeight = ctx->image_h;
	desc.cropWidth = ctx->image_w;
	desc.cropHeight = ctx->image_h;
	desc.aspectRatio = (float)ctx->image_w / (float)ctx->image_h;

	// Draw the quad
	MTY_WindowDrawQuad(ctx->app, 0, ctx->image, &desc);
	MTY_WindowPresent(ctx->app, 0, 1);

	return !ctx->quit;
}

static void logCallback(ParsecLogLevel level, const char *msg, void *opaque)
{
	opaque;
	if (level != LOG_DEBUG)
	{
		std::cout << "[I] " << msg << std::endl;
	}
	//printf("[%s] %s\n", level == LOG_DEBUG ? "D" : "I", msg);
}

Parsec* ParsecArcadeStart(ParsecSession *parsecSession, ParsecHostConfig hostConfig)
{
	Parsec *ps = nullptr;
	ParsecStatus status = ParsecInit(PARSEC_VER, NULL, NULL, &ps);
	if (status == PARSEC_OK) {
		ParsecSetLogCallback(logCallback, NULL);
		ParsecHostStart(ps, HOST_GAME, &hostConfig, parsecSession->sessionId.c_str());
		return ps;
	}

	return NULL;
}

void clearGuests(Parsec *ps, int *guestCount)
{
	if (*guestCount > 0)
	{
		ParsecFree(ps);
		*guestCount = 0;
	}
}

void logDataMessage(const char* msg, ParsecHostEvent event)
{
	std::string msgStr = msg;
	std::cout
		<< std::endl
		<< "DATA"
		<< " | userid: " << event.userData.guest.userID
		<< " | guestid: " << event.userData.guest.id
		<< " | msgid: " << event.userData.id
		<< " | key: " << event.userData.key
		<< " | msg: " << msg;
}

void broadcastChatMessage(Parsec *ps, ParsecGuest *guests, int guestCount, std::string message)
{
	ParsecGuest it;
	for (int i = 0; i < guestCount; i++)
	{
		it = guests[i];
		ParsecHostSendUserData(ps, it.id, PARSEC_APP_CHAT_MSG, message.c_str());
	}
}

void displayInputMessage(ParsecMessage inputGuestMsg)
{
	switch (inputGuestMsg.type)
	{
	case MESSAGE_GAMEPAD_BUTTON:
		std::cout
			<< "===============================\n"
			<< " Button\n"
			<< "===============================\n";
		ParsecGamepadButtonMessage btnMsg = inputGuestMsg.gamepadButton;
		std::cout
			<< "pad id: " << btnMsg.id << "\n"
			<< "pressed: " << (btnMsg.pressed ? "true" : "false") << "\n"
			<< "button: " << btnMsg.button << "\n\n";
		break;

	case MESSAGE_GAMEPAD_AXIS:
		std::cout
			<< "===============================\n"
			<< " Axis\n"
			<< "===============================\n";
		ParsecGamepadAxisMessage axisMsg = inputGuestMsg.gamepadAxis;
		std::cout
			<< "pad id: " << axisMsg.id << "\n"
			<< "axis: " << axisMsg.axis << "\n"
			<< "value: " << axisMsg.value << "\n\n";
		break;

	case MESSAGE_GAMEPAD_STATE:
		std::cout
			<< "===============================\n"
			<< " State\n"
			<< "===============================\n";
		ParsecGamepadStateMessage stateMsg = inputGuestMsg.gamepadState;
		std::cout
			<< "pad id: " << stateMsg.id << "\n"
			<< "buttons: " << stateMsg.buttons << "\n"
			<< "L2: " << (int)stateMsg.leftTrigger << "\n"
			<< "R2: " << (int)stateMsg.rightTrigger << "\n"
			<< "LX: " << stateMsg.thumbLX << "\n"
			<< "LY: " << stateMsg.thumbLY << "\n"
			<< "RX: " << stateMsg.thumbRX << "\n"
			<< "RY: " << stateMsg.thumbRY << "\n\n";

		break;

	case MESSAGE_GAMEPAD_UNPLUG:
		std::cout
			<< "===============================\n"
			<< " Unplug\n"
			<< "===============================\n";
		ParsecGamepadUnplugMessage unplugMsg = inputGuestMsg.gamepadUnplug;
		std::cout << "pad id: " << unplugMsg.id << "\n\n";
		break;

	case MESSAGE_RELEASE:
		std::cout
			<< "===============================\n"
			<< " Release\n"
			<< "===============================\n\n";

	default:
		break;
	}
}

bool isFilteredCommand(ACommand *command)
{
	COMMAND_TYPE type;
	for (std::vector<COMMAND_TYPE>::iterator it = filteredCommands.begin(); it != filteredCommands.end(); ++it)
	{
		type = command->type();
		if (command->type() == *it)
		{
			return true;
		}
	}

	return false;
}


void initAllModules()
{
	dice.init();

	// Instance all gamepads at once
	padClient.init();
	padClient.createMaximumGamepads();
	padClient.connectAllGamepads();
	padClient.sortGamepads();
	padClient.setLimit(3888558, 0);		// Remove myself
	padClient.setLimit(6711547, 1);

	// Data is mocked for now - arguments don't matter
	parsecSession.mockSession();	// Replace with fetchSession in final version
	//parsecSession.fetchSession(EMAIL, PASSWORD);

	hostConfig = EMPTY_HOST_CONFIG;
	strcpy_s(hostConfig.name, ROOM_NAME);
	strcpy_s(hostConfig.secret, ROOM_SECRET);
	hostConfig.maxGuests = 19;
	hostConfig.publicGame = false;

	ps = ParsecArcadeStart(&parsecSession, hostConfig);

	dx11.init();

	audioOut.fetchDevices();
	audioOut.setOutputDevice();		// TODO Fix leak in setOutputDevice
	std::vector<AudioOutDevice> audioOutDevices = audioOut.getDevices();
	audioOut.captureAudio();

	std::vector<AudioInDevice> devices = audioIn.listInputDevices();
	AudioInDevice device = audioIn.selectInputDevice("cable out");
	audioIn.init(device);
}

void liveStreamMedia()
{
	using clock = std::chrono::system_clock;
	using milli = std::chrono::duration<double, std::milli>;
	const double FPS = 250.0;
	const double MS_PER_FRAME = 1000.0 / FPS;

	while (isRunning)
	{
		const auto before = clock::now();
		dx11.captureScreen(ps);

		audioIn.captureAudio();
		audioOut.captureAudio();
		if (audioIn.isReady() && audioOut.isReady())
		{
			std::vector<int16_t> inBuffer = audioIn.popBuffer();
			std::vector<int16_t> outBuffer = audioOut.popBuffer();
			std::vector<int16_t> mixBuffer = audioMix.mix(&inBuffer, &outBuffer);
			ParsecHostSubmitAudio(ps, PCM_FORMAT_INT16, audioOut.getFrequency(), mixBuffer.data(), mixBuffer.size() / 2);
		}

		const milli duration = clock::now() - before;
		
		if (duration.count() < MS_PER_FRAME)
		{
			Sleep( MS_PER_FRAME - duration.count() );
		}
	}
}

void pollInputs()
{
	ParsecGuest inputGuest;
	ParsecMessage inputGuestMsg;

	while (isRunning)
	{
		if (ParsecHostPollInput(ps, 4, &inputGuest, &inputGuestMsg))
		{
			padClient.sendMessage(inputGuest, inputGuestMsg);
		}
	}
}

void pollEvents ()
{
	ParsecGuest dataGuest;
	std::string guestMsg;
	std::string chatBotReply;
	bool isAdmin = false;
	int guestCount = 0;

	ParsecHostEvent event;

	while (isRunning)
	{
		if (ParsecHostPollEvents(ps, 30, &event)) {
			ParsecGuest guest = event.guestStateChange.guest;

			switch (event.type)
			{
			case HOST_EVENT_GUEST_STATE_CHANGE:
				if ((guest.state == GUEST_CONNECTED || guest.state == GUEST_CONNECTING) && banList.isBanned(guest.userID))
				{
					ParsecHostKickGuest(ps, guest.id);
					broadcastChatMessage(ps, guests, guestCount, chatBot.formatBannedGuestMessage(guest));
				}
				else if (guest.state == GUEST_CONNECTED || guest.state == GUEST_DISCONNECTED)
				{
					guestCount = ParsecHostGetGuests(ps, GUEST_CONNECTED, &guests);
					guestMsg.clear();
					guestMsg = std::string(event.guestStateChange.guest.name);

					isAdmin = adminList.isAdmin(event.userData.guest.userID);
					broadcastChatMessage(
						ps, guests, guestCount,
						chatBot.formatGuestConnection(event.guestStateChange.guest, isAdmin)
					);

					if (guest.state == GUEST_CONNECTED)
					{
						printf("%s (#%d)    >> joined\n", guest.name, guest.userID);
					}
					else
					{
						printf("%s (#%d)    quit >>\n", guest.name, guest.userID);
						int droppedPads = 0;
						CommandFF command;
						command.run(guest, &padClient, &droppedPads);
						if (droppedPads > 0)
						{
							broadcastChatMessage(ps, guests, guestCount, command.replyMessage());
						}
					}
				}
				break;

			case HOST_EVENT_USER_DATA:
				char* msg = (char*)ParsecGetBuffer(ps, event.userData.key);
				std::cout << msg << "\n";
				if (event.userData.id == PARSEC_APP_CHAT_MSG)
				{
					dataGuest = event.userData.guest;
					guestCount = ParsecHostGetGuests(ps, GUEST_CONNECTED, &guests);
					isAdmin = adminList.isAdmin(dataGuest.userID);
					ACommand* command = chatBot.identifyUserDataMessage(msg);
					COMMAND_TYPE type = command->type();


					// =================================
					//  Pleb commands
					// =================================
					if (!isAdmin)
					{
						switch (command->type())
						{
						case COMMAND_TYPE::AFK:
							((CommandAFK*)command)->run(guests, guestCount, &padClient);
							break;
						case COMMAND_TYPE::BONK:
							((CommandBonk*)command)->run(msg, dataGuest, guests, guestCount, &dice);
							break;
						case COMMAND_TYPE::COMMANDS:
							((CommandListCommands*)command)->run(isAdmin);
							break;
						case COMMAND_TYPE::FF:
							((CommandFF*)command)->run(dataGuest, &padClient);
							break;
						case COMMAND_TYPE::IP:
							((CommandIpFilter*)command)->run(ps, dataGuest, &banList);
							break;
						case COMMAND_TYPE::MIRROR:
							((CommandMirror*)command)->run(dataGuest, &padClient);
							break;
						case COMMAND_TYPE::PADS:
							((CommandPads*)command)->run(&padClient);
							break;
						case COMMAND_TYPE::SWAP:
							((CommandSwap*)command)->run(msg, dataGuest, &padClient);
							break;
						default:
							break;
						}
					}

					// =================================
					//  Admin commands
					// =================================
					else
					{
						switch (command->type())
						{
						case COMMAND_TYPE::AFK:
							((CommandAFK*)command)->run(guests, guestCount, &padClient);
							break;
						case COMMAND_TYPE::BAN:
							((CommandBan*)command)->run(msg, ps, dataGuest, guests, guestCount, &banList);
							break;
						case COMMAND_TYPE::BONK:
							((CommandBonk*)command)->run(msg, dataGuest, guests, guestCount, &dice);
							break;
						case COMMAND_TYPE::COMMANDS:
							((CommandListCommands*)command)->run(isAdmin);
							break;
						case COMMAND_TYPE::DC:
							((CommandDC*)command)->run(msg, &padClient);
							break;
						case COMMAND_TYPE::FF:
							((CommandFF*)command)->run(dataGuest, &padClient);
							break;
						case COMMAND_TYPE::GAMEID:
							((CommandGameId*)command)->run(msg, &hostConfig);
							break;
						case COMMAND_TYPE::GUESTS:
							((CommandGuests*)command)->run(msg, &hostConfig);
							break;
						case COMMAND_TYPE::IP:
							((CommandIpFilter*)command)->run(ps, dataGuest, &banList);
							break;
						case COMMAND_TYPE::KICK:
							((CommandKick*)command)->run(msg, ps, dataGuest, guests, guestCount);
							break;
						case COMMAND_TYPE::LIMIT:
							((CommandLimit*)command)->run(msg, guests, guestCount, &padClient);
							break;
						case COMMAND_TYPE::MIC:
							((CommandMic*)command)->run(msg, &audioMix);
							break;
						case COMMAND_TYPE::MIRROR:
							((CommandMirror*)command)->run(dataGuest, &padClient);
							break;
						case COMMAND_TYPE::NAME:
							((CommandName*)command)->run(msg, &hostConfig);
							break;
						case COMMAND_TYPE::PADS:
							((CommandPads*)command)->run(&padClient);
							break;
						case COMMAND_TYPE::PRIVATE:
							((CommandPrivate*)command)->run(&hostConfig);
							break;
						case COMMAND_TYPE::PUBLIC:
							((CommandPublic*)command)->run(&hostConfig);
							break;
						case COMMAND_TYPE::SWAP:
							((CommandSwap*)command)->run(msg, dataGuest, &padClient);
							break;
						case COMMAND_TYPE::QUIT:
							((CommandQuit*)command)->run(&isRunning);
							break;
						case COMMAND_TYPE::SETCONFIG:
							((CommandSetConfig*)command)->run(ps, &hostConfig, parsecSession.sessionId.c_str());
							break;
						case COMMAND_TYPE::SPEAKERS:
							((CommandSpeakers*)command)->run(msg, &audioMix);
							break;
						case COMMAND_TYPE::UNBAN:
							((CommandUnban*)command)->run(msg, dataGuest, &banList);
							break;
						case COMMAND_TYPE::VIDEOFIX:
							((CommandVideoFix*)command)->run(&dx11);
							break;
							//case COMMAND_TYPE::GIVE:
							//	break;
							//case COMMAND_TYPE::TAKE:
							//	break;
						default:
							break;
						}
					}

					// Blocked messages
					if (!isFilteredCommand(command))
					{
						CommandDefaultMessage defaultMessage;
						defaultMessage.run(msg, dataGuest, chatBot.getLastUserId(), isAdmin);
						chatBot.setLastUserId(dataGuest.userID);

						if (!defaultMessage.replyMessage().empty())
						{
							broadcastChatMessage(ps, guests, guestCount, defaultMessage.replyMessage());
							std::cout << std::endl << defaultMessage.replyMessage();
						}
					}

					if (!command->replyMessage().empty())
					{
						broadcastChatMessage(ps, guests, guestCount, command->replyMessage());
						std::cout << std::endl << command->replyMessage();
						chatBot.setLastUserId();
					}

					delete command;
				}

				ParsecFree(msg);
				break;
			}
		}
	}
}

void guiLoop()
{
	//MTY_App *app = MTY_AppCreate();
}

int main(int argc, char** argv)
{
	initAllModules();

	if (ps != NULL)
	{
		std::thread mediaThread (liveStreamMedia);
		std::thread inputThread (pollInputs);
		std::thread eventsThread (pollEvents);

		inputThread.join();
		eventsThread.join();
		mediaThread.join();

		padClient.release();
		ParsecHostStop(ps);
		ParsecDestroy(ps);
	}

	return 0;
}