#pragma once

#include "matoya.h"
#include <string>
#include <sstream>
#include <thread>
#include <functional>
#include "Utils.h"
#include "GuestList.h"
#include "MetadataCache.h"
#include "Clock.h"
#include "ThumbnailList.h"

#define SESSION_LIFETIME (uint32_t)(HOURS(2))

#define PARSEC_API_HOST "kessel-api.parsecgaming.com"
#define PARSEC_API_V1_AUTH "/v1/auth/"
#define PARSEC_API_CODES "/auth/codes"
#define PARSEC_API_SESSIONS "/auth/sessions"
#define PARSEC_API_V2_HOSTS "/v2/hosts/"
#define PARSEC_API_ME "/me/"
#define HTTP_AUTO_PORT 0
#define HTTP_TIMEOUT_MS 5000
#define HTTP_TIMEOUT_CODES 120000
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
	class AuthResult
	{
	public:
		AuthResult() :
			verificationUri(""), userCode(""), hash(""),
			expiresAt(0), interval(0), success(false)
		{}
		char verificationUri[256], userCode[16], hash[64];
		uint32_t expiresAt, interval;
		bool success;
	};

	enum class SessionStatus
	{
		PENDING = 202,
		APPROVED = 201,
		INVALID = 401
	};

	bool loadSessionCache();
	bool saveSessionCache(bool isValid = true);
	const bool fetchSession(const char* email, const char* password, const char* tfa = "");
	const ParsecSession::SessionStatus pollSession(ParsecSession::AuthResult auth);
	const AuthResult authenticate();
	void fetchArcadeRoomList();
	void fetchArcadeRoomListSync();
	void fetchAccountData(Guest* user);
	void fetchAccountDataSync(Guest* user);
	bool& isValid();
	bool isUpdating();
	const string getSessionError();
	const int getSessionStatus();
	const uint32_t getRemainingTime();
	const uint32_t getLifespan();
	vector<Thumbnail>& getThumbnails();

	string hostPeerId;
	string sessionId;
	MetadataCache::SessionCache::SessionType type;

	MTY_JSON *arcadeRooms;

private:
	const void extendSessionTime();

	uint32_t _start, _expiry;
	thread _accountDataThread, _roomListThread;
	bool _isValid = false, _isAuthenticating = false;
	bool _isUpdating = false;
	int _sessionStatus = 0;
	string _sessionError = "";

	ThumbnailList _thumbnailList;
};