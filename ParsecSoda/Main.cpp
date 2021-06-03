#include "matoya.h"

#define D3D_DEBUG_INFO

#include <iostream>
#include <d3d11.h>
#include <dxgi.h>
#include <dxgi1_2.h>
#include <sstream>
#include "ParsecSession.h"
#include "DX11.h"
#include "AdminList.h"
#include "ChatBot.h"
#include "TestChatBot.h"
#include "Stringer.h"
#include "StressTest.h"
#include "Mock.h"
#include "AudioIn.h"
#include "AudioOut.h"
#include "AudioMix.h"
#include "GamepadClient.h"
#include "BanList.h"

const std::vector<int> admins { 3888558, 6711547 };
const std::vector<BannedUser> banned {};

#define PARSEC_APP_CHAT_MSG 0

#define ROOM_NAME "Coding my own Parsec\nGamepad streaming\0"
#define ROOM_SECRET "melonsod"

GamepadClient padClient;

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

static void logGuestStateChange(ParsecGuest *guest)
{
	switch (guest->state) {
	case GUEST_CONNECTED:
		printf("%s (#%d)    >> joined\n", guest->name, guest->userID);
		break;
	case GUEST_DISCONNECTED:
		padClient.onRageQuit(*guest);
		printf("%s (#%d)    quit >>\n", guest->name, guest->userID);
		break;
	default:
		break;
	}
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

void tryBroadcastChatMessage(Parsec * ps, ParsecGuest * guests, int guestCount, std::string replyMsg)
{
	if (!replyMsg.empty())
	{
		broadcastChatMessage(ps, guests, guestCount, replyMsg);
		std::cout << std::endl << replyMsg;
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

int main(int argc, char **argv)
{
	// Instance all gamepads at once
	padClient.init();
	padClient.createMaximumGamepads();
	padClient.setLimit(3888558, 0);		// Remove myself
	padClient.setLimit(6711547, 0);

	ParsecGuest *guests = NULL;
	ParsecGuest inputGuest;
	ParsecMessage inputGuestMsg;
	int guestCount = 0;

	// Data is mocked for now - arguments don't matter
	#define EMAIL ""
	#define PASSWORD ""
	ParsecSoda::ParsecSession parsecSession;
	parsecSession.mockSession(true);	// Replace with fetchSession in final version
	//parsecSession.fetchSession(EMAIL, PASSWORD);

	const int stop = 2;
	
	AdminList adminList(admins);
	BanList banList(banned);
	
	ChatBot chatBot;
	std::string guestMsg;
	std::string chatBotReply;
	bool isAdmin = false;
	
	ParsecHostConfig cfg = EMPTY_HOST_CONFIG;
	strcpy_s(cfg.name, ROOM_NAME);
	strcpy_s(cfg.secret, ROOM_SECRET);
	//strcpy_s(cfg.gameID, "");
	cfg.maxGuests = 19;
	cfg.publicGame = false;

	Parsec *ps = ParsecArcadeStart(&parsecSession, cfg);

	DX11 dx11;
	dx11.init();


	AudioOut audioOut;
	audioOut.fetchDevices();
	audioOut.setOutputDevice();		// TODO Fix leak in setOutputDevice
	std::vector<AudioOutDevice> audioOutDevices = audioOut.getDevices();
	audioOut.captureAudio();
	
	AudioIn audioIn;
	std::vector<AudioInDevice> devices = audioIn.listInputDevices();
	AudioInDevice device = audioIn.selectInputDevice("cable out");
	audioIn.init(device);

	AudioMix audioMix (0.8, 0.3);

	if (ps != NULL)
	{
		while (true) {
			dx11.captureScreen(ps);
			
			audioIn.captureAudio();
			audioOut.captureAudio();
			if (audioIn.isReady() && audioOut.isReady())
			{
				std::vector<int16_t> inBuffer = audioIn.popBuffer();
				std::vector<int16_t> outBuffer = audioOut.popBuffer();
				std::vector<int16_t> mixBuffer = audioMix.mix(&inBuffer, &outBuffer);
				ParsecHostSubmitAudio(ps, PCM_FORMAT_INT16, audioOut.getFrequency(), mixBuffer.data(), mixBuffer.size()/2);
			}

			if (ParsecHostPollInput(ps, 1, &inputGuest, &inputGuestMsg))
			{
				padClient.sendMessage(inputGuest, inputGuestMsg);
			}

			ParsecHostEvent event;
			if (ParsecHostPollEvents(ps, 1, &event)) {
				ParsecGuest guest = event.guestStateChange.guest;
				
				switch (event.type) {
				case HOST_EVENT_GUEST_STATE_CHANGE:
					logGuestStateChange(&event.guestStateChange.guest);
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
					}
					break;
				case HOST_EVENT_USER_DATA:
					char *msg = (char*)ParsecGetBuffer(ps, event.userData.key);
					std::cout << msg << "\n";
					if (event.userData.id == PARSEC_APP_CHAT_MSG)
					{
						guestCount = ParsecHostGetGuests(ps, GUEST_CONNECTED, &guests);
						isAdmin = adminList.isAdmin(event.userData.guest.userID);

						// Banned messages
						chatBotReply.clear();
						if (chatBot.isTrollMessage(msg, event.userData.guest, &chatBotReply))
						{
							ParsecHostKickGuest(ps, event.userData.guest.id);
							tryBroadcastChatMessage(ps, guests, guestCount, chatBotReply);
							ParsecFree(msg);
							break;
						}

						// Pleb user message
						guestMsg.clear();
						guestMsg = chatBot.formatGuestMessage(event.userData.guest, msg, isAdmin);
						broadcastChatMessage(ps, guests, guestCount, guestMsg);
						logDataMessage(msg, event);
						std::cout << guestMsg;

						chatBotReply.clear();
						chatBot.tryBonkMessage(msg, event.userData.guest, guests, guestCount, &chatBotReply);

						chatBot.processCommandsListMessage(msg, isAdmin, &chatBotReply);

						// This user is admin, thus allowed to use commands
						if (isAdmin && chatBotReply.empty())
						{
							chatBotReply.clear();
							float volume = 50;
							ParsecGuest targetGuest;
							
							if (chatBot.isSetConfigMessage(msg, &chatBotReply))
							{
								ParsecHostSetConfig(ps, &cfg, parsecSession.sessionId.c_str());
							}
							else if (chatBot.isDX11RefreshMessage(msg, &chatBotReply))
							{
								dx11.recover();
							}
							else if (chatBot.isQuitMessage(msg, &chatBotReply))
							{
								broadcastChatMessage(ps, guests, guestCount, chatBotReply);
								goto gameover;
							}
							else if (chatBot.isMicVolumeMessage(msg, &chatBotReply, &volume))
							{
								audioMix.setVolume1(volume);
							}
							else if (chatBot.isSpeakersVolumeMessage(msg, &chatBotReply, &volume))
							{
								audioMix.setVolume2(volume);
							}
							else if (chatBot.tryKickMessage(msg, event.userData.guest, guests, guestCount, &chatBotReply, &targetGuest))
							{
								if (event.userData.guest.userID != targetGuest.userID)
								{
									ParsecHostKickGuest(ps, targetGuest.id);
								}
							}
							else if (!chatBot.isPadsLimitMessage(msg, guests, guestCount, &padClient, &chatBotReply))
							{
								chatBot.processHostConfigMessage(msg, &cfg, &chatBotReply);
							}
						}


						tryBroadcastChatMessage(ps, guests, guestCount, chatBotReply);
					}
					
					ParsecFree(msg);
					break;
				}
			}
		}
	}

	gameover:

	padClient.release();
	ParsecHostStop(ps);
	ParsecDestroy(ps);

	return 0;
}