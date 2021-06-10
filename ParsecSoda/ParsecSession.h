#pragma once

//#pragma comment(lib, "parsec.dll")

#include "parsec.h"
#include "matoya.h"
#include <string>
#include <sstream>
#include "Utils.h"
#include "Mock.h"

#define PARSEC_API_HOST "kessel-api.parsecgaming.com"
#define PARSEC_API_V1_AUTH "/v1/auth/"
#define PARSEC_API_V2_HOSTS "/v2/hosts/"
#define HTTP_AUTO_PORT 0
#define HTTP_TIMEOUT_MS 5000
#define HEADER_JSON "Content-Type: application/json"
#define HEADER_AUTH_BEARER "Authorization: Bearer "

#define EMPTY_HOST_CONFIG {                                                          \
	/*video             */ {PARSEC_HOST_VIDEO_DEFAULTS, PARSEC_HOST_VIDEO_DEFAULTS}, \
	/*gamepadType       */ GAMEPAD_TYPE_X360,                                        \
	/*adminMute         */ 1,                                                        \
	/*exclusiveInput    */ 0,                                                        \
	/*maxGuests         */ 4,                                                        \
	/*name              */ "",                                                       \
	/*desc              */ "",                                                       \
	/*gameID            */ "",                                                       \
	/*secret            */ "",                                                       \
	/*publicGame        */ true,                                                     \
}

class ParsecSession
{
public:
	std::string hostPeerId;
	std::string sessionId;
	MTY_JSON *arcadeRooms;

	/**
		*	Remove this in final version
		*/ 
	void mockSession(const char * sessionID, const char * hostPeerId);
		
	/**
		*	Remove this in final version
		*/
	void mockSession(bool isTestAccount = true);
		
		
	const bool fetchSession(const char* email, const char* password, const char* tfa = "");
	const bool fetchArcadeRoomList();
	static bool compareHostConfigs(ParsecHostConfig cfg, ParsecHostConfig other);
};

