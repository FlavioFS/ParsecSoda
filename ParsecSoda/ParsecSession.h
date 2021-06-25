#pragma once

#include "matoya.h"
#include <string>
#include <sstream>
#include <thread>
#include "Utils.h"
#include "Mock.h"
#include "GuestList.h"
#include "MetadataCache.h"

#define PARSEC_API_HOST "kessel-api.parsecgaming.com"
#define PARSEC_API_V1_AUTH "/v1/auth/"
#define PARSEC_API_V2_HOSTS "/v2/hosts/"
#define PARSEC_API_ME "/me/"
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

using namespace std;

class ParsecSession
{
public:

	/**
		*	Remove this in final version
		*/ 
	void mockSession(const char * sessionID, const char * hostPeerId);
		
	/**
		*	Remove this in final version
		*/
	void mockSession(bool isTestAccount = true);
	

	bool loadSessionCache();
	bool saveSessionCache();
	const bool fetchSession(const char* email, const char* password, const char* tfa = "");
	const bool fetchArcadeRoomList();
	const bool fetchAccountData(Guest* user);
	bool& isValid();
	const string getSessionError();
	const int getSessionStatus();

	string hostPeerId;
	string sessionId;
	MTY_JSON *arcadeRooms;

private:
	thread _accountDataThread;
	bool _isValid = false;
	int _sessionStatus = 0;
	string _sessionError = "";
};