#include "ParsecSession.h"
bool ParsecSession::loadSessionCache()
{
	MetadataCache::SessionCache cache = MetadataCache::loadSessionCache();
	if (cache.isValid)
	{
		sessionId = cache.sessionID;
		hostPeerId = cache.peerID;
		type = cache.type;
		_start = cache.start;
		_expiry = cache.expiry;
		
		_isValid = false;
		return true;
	}

	return false;
}

bool ParsecSession::saveSessionCache(bool isValid)
{
	MetadataCache::SessionCache cache = MetadataCache::SessionCache();
	cache.sessionID = sessionId;
	cache.peerID = hostPeerId;
	cache.type = type;
	cache.start = _start;
	cache.expiry = _expiry;
	cache.isValid = isValid;

	bool success = MetadataCache::saveSessionCache(cache);

	return success;
}

const ParsecSession::AuthResult ParsecSession::authenticate()
{
	AuthResult result;

	// Body
	const string bodyString = "{\"game_id\": \"1wdoHfhhZH5lPuZCwGBete0HIAj\"}";
	const char* body = bodyString.c_str();
	size_t bodySize = sizeof(char) * bodyString.length();

	// Other Params
	const bool isHttps = true;

	// Response
	void* response;
	size_t responseSize;
	uint16_t status;

	const bool rekt = MTY_HttpRequest(
		PARSEC_API_HOST, HTTP_AUTO_PORT, isHttps, "POST", PARSEC_API_CODES, HEADER_JSON,
		body, bodySize,
		HTTP_TIMEOUT_CODES,
		&response, &responseSize, &status
	);

	const char* responseStr = (const char*)response;
	const MTY_JSON* json = MTY_JSONParse(responseStr);
	bool success = false;

	if (responseSize > 0 && status == 201)
	{
		const MTY_JSON *data = MTY_JSONObjGetItem(json, "data");

		result.success =
			MTY_JSONObjGetString(data, "verification_uri", result.verificationUri, 256)
			&& MTY_JSONObjGetString(data, "user_code", result.userCode, 16)
			&& MTY_JSONObjGetString(data, "hash", result.hash, 64)
			//&& MTY_JSONObjGetUInt(data, "expires_at", &result.expiresAt);
			&& MTY_JSONObjGetUInt(data, "interval", &result.interval)
			;

		if (result.success)
		{
			_isAuthenticating = true;
		}

		return result;
	}
	else
	{
		_sessionStatus = status;
		char error[256] = "";
		if (MTY_JSONObjGetString(json, "error", error, 256))
		{
			_sessionError = error;
		}
	}

	_isValid = false;
	return result;
}

const ParsecSession::SessionStatus ParsecSession::pollSession(ParsecSession::AuthResult auth)
{
	if (!auth.success)
	{
		return SessionStatus::INVALID;
	}

	bool result = false;

	// Body
	ostringstream ss;
	ss << "{"
		<< "\"" << "grant_type" << "\":\"" << "auth_code" << "\", "
		<< "\"" << "auth_code_hash" << "\":\"" << auth.hash << "\"}";
	const string bodyString = ss.str();
	const char* body = bodyString.c_str();
	size_t bodySize = sizeof(char) * bodyString.length();

	// Other Params
	const bool isHttps = true;

	// Response
	void* response;
	size_t responseSize;
	uint16_t status;

	const bool rekt = MTY_HttpRequest(
		PARSEC_API_HOST, HTTP_AUTO_PORT, isHttps, "POST", PARSEC_API_SESSIONS, HEADER_JSON,
		body, bodySize,
		HTTP_TIMEOUT_MS,
		&response, &responseSize, &status
	);

	const char* responseStr = (const char*)response;
	const MTY_JSON* json = MTY_JSONParse(responseStr);
	bool success = false;

	if (responseSize > 0 && status == 201)
	{
		const MTY_JSON* data = MTY_JSONObjGetItem(json, "data");

		bool success = false;

		char id[256], peerId[128];
		uint32_t expiry = 0;

		success =
			MTY_JSONObjGetString(data, "id", id, 256)
			&& MTY_JSONObjGetString(data, "host_peer_id", peerId, 128)
			;

		if (success)
		{
			this->sessionId = id;
			this->hostPeerId = peerId;
			this->type = MetadataCache::SessionCache::SessionType::THIRD;
			_start = Clock::now();
			_expiry = _start + SESSION_LIFETIME;
			saveSessionCache();

			_isAuthenticating = true;
			result = true;
			_isValid = true;
		}
	}
	else
	{
		_sessionStatus = status;
		char error[256] = "";
		if (MTY_JSONObjGetString(json, "error", error, 256))
		{
			_sessionError = error;
		}

		_isValid = false;
	}
	
	return (SessionStatus)status;
}

const bool ParsecSession::fetchSession(const char* email, const char* password, const char* tfa)
{
	// Body
	ostringstream ss;
	ss << "{"
		<< "\"" << "email" << "\":\"" << email << "\","
		<< "\"" << "password" << "\":\"" << password << "\","
		<< "\"" << "tfa" << "\":\"" << tfa << "\"}";
	const string bodyString = ss.str();
	const char* body = bodyString.c_str();
	size_t bodySize = sizeof(char) * bodyString.length();

	// Other Params
	const bool isHttps = true;

	// Response
	void* response;
	size_t responseSize;
	uint16_t status;

	const bool rekt = MTY_HttpRequest(
		PARSEC_API_HOST, HTTP_AUTO_PORT, isHttps, "POST", PARSEC_API_V1_AUTH, HEADER_JSON,
		body, bodySize,
		HTTP_TIMEOUT_MS,
		&response, &responseSize, &status
	);

	const char* responseStr = (const char*)response;
	const MTY_JSON* json = MTY_JSONParse(responseStr);
	bool success = false;

	if (responseSize > 0 && status == 201)
	{
		hostPeerId = MTY_JSONSerialize(MTY_JSONObjGetItem(json, "host_peer_id"));
		sessionId = MTY_JSONSerialize(MTY_JSONObjGetItem(json, "session_id"));

		Utils::removeCharFromString(&hostPeerId, '"');
		Utils::removeCharFromString(&sessionId, '"');
		_isValid = true;

		this->sessionId = sessionId;
		this->hostPeerId = hostPeerId;
		this->type = MetadataCache::SessionCache::SessionType::PERSONAL;
		_start = Clock::now();
		_expiry = 0;
		saveSessionCache();

		return true;
	}
	else
	{
		_sessionStatus = status;
		char error[256] = "";
		if (MTY_JSONObjGetString(json, "error", error, 256))
		{
			_sessionError = error;
		}
	}

	_isValid = false;
	return success;
}

void ParsecSession::fetchArcadeRoomList()
{
	_roomListThread = thread([&]() {
		fetchArcadeRoomListSync();
		_roomListThread.detach();
	});
}

void ParsecSession::fetchArcadeRoomListSync()
{
	if (sessionId.empty()) {
		return;
	}

	// Body
	ostringstream ss;
	ss << PARSEC_API_V2_HOSTS << "?mode=game&public=true";
	const string pathString = ss.str();
	const char *path = pathString.c_str();

	ss.str("");
	ss.clear();
	const char *session = ParsecSession::sessionId.c_str();
	ss << HEADER_AUTH_BEARER << ParsecSession::sessionId.c_str();
	const string headerString = ss.str();
	const char *header = headerString.c_str();

	// Other Params
	const bool isHttps = true;

	// Response
	void* response = nullptr;
	size_t responseSize = 0;
	uint16_t status = 0;

	try
	{
		const bool rekt = MTY_HttpRequest(
			PARSEC_API_HOST, HTTP_AUTO_PORT, isHttps, "GET", path, header,
			NULL, 0,
			HTTP_TIMEOUT_MS,
			&response, &responseSize, &status
		);

		if (responseSize > 0 && status == 200)
		{
			string responseStr = (const char*)response;
			MTY_JSON* json = MTY_JSONParse(responseStr.c_str());

			const MTY_JSON* data = MTY_JSONObjGetItem(json, "data");
			const uint32_t length = MTY_JSONGetLength(data);

			char name[256], gameId[64];
			bool success = false;

			for (size_t i = 0; i < length; i++)
			{
				const MTY_JSON* room = MTY_JSONArrayGetItem(data, i);
				success =
					MTY_JSONObjGetString(room, "name", name, 256)
					&& MTY_JSONObjGetString(room, "game_id", gameId, 64)
					;

				if (success)
				{
					_thumbnailList.add(Thumbnail(gameId, name));
				}
			}

			MTY_JSONDestroy(&json);
		}
	}
	catch (const std::exception&) {}
}

void ParsecSession::fetchAccountData(Guest *user)
{
	if (sessionId.empty()) {
		return;
	}

	_accountDataThread = thread ([user, this]() {
		fetchAccountDataSync(user);
		_accountDataThread.detach();
	});
}

void ParsecSession::fetchAccountDataSync(Guest* user)
{
	_isUpdating = true;

	// Body
	ostringstream ss;
	ss << PARSEC_API_ME;
	const string pathString = ss.str();
	const char* path = pathString.c_str();

	ss.str("");
	ss.clear();
	const char* session = ParsecSession::sessionId.c_str();
	ss << HEADER_AUTH_BEARER << ParsecSession::sessionId.c_str();
	const string headerString = ss.str();
	const char* header = headerString.c_str();

	// Other Params
	const bool isHttps = true;

	// Response
	void* response;
	size_t responseSize = 0;
	uint16_t status = 0;

	MTY_HttpRequest(
		PARSEC_API_HOST, HTTP_AUTO_PORT, isHttps, "GET", path, header,
		NULL, 0,
		HTTP_TIMEOUT_MS,
		&response, &responseSize, &status
	);

	if (responseSize > 0 && status == 200)
	{
		static bool result = false;
		result = false;
		MTY_JSON* json;
		const MTY_JSON* data;

		try
		{
			string responseStr = (const char*)response;
			json = MTY_JSONParse(responseStr.c_str());
			data = MTY_JSONObjGetItem(json, "data");

			char name[GUEST_NAME_LEN];
			if (MTY_JSONObjGetString(data, "name", name, GUEST_NAME_LEN))
			{
				user->name = name;
			}

			uint32_t userID = 0;
			if (MTY_JSONObjGetUInt(data, "id", &userID))
			{
				user->userID = userID;
			}

			if (_expiry < Clock::now())
			{
				extendSessionTime();
				saveSessionCache();
			}

			user->status = Guest::Status::OK;
			result = true;
			_isValid = true;
		}
		catch (const std::exception&) {}

		MTY_JSONDestroy(&json);
	}
	else
	{
		user->name = "Session Expired";
		user->userID = 0;
		user->status = Guest::Status::EXPIRED;
		_isValid = false;
	}

	_isUpdating = false;
}

bool& ParsecSession::isValid()
{
	return _isValid;
}

bool ParsecSession::isUpdating()
{
	return _isUpdating;
}

const string ParsecSession::getSessionError()
{
	return _sessionError;
}

const int ParsecSession::getSessionStatus()
{
	return _sessionStatus;
}

const uint32_t ParsecSession::getRemainingTime()
{
	uint32_t now = Clock::now();

	if (_expiry > now)
	{
		return _expiry - now;
	}

	return 0;
}

const uint32_t ParsecSession::getLifespan()
{
	return _expiry - _start;
}

vector<Thumbnail>& ParsecSession::getThumbnails()
{
	return _thumbnailList.getThumbnails();
}

void ParsecSession::loadThumbnails()
{
	_thumbnailList.load();
	_thumbnailList.add(Thumbnail("1wdoHfhhZH5lPuZCwGBete0HIAj", "Parsec Soda"));
}

void ParsecSession::saveThumbnails()
{
	_thumbnailList.save();
}

const void ParsecSession::extendSessionTime()
{
	_start = Clock::now();
	_expiry = _start + SESSION_LIFETIME;
}
