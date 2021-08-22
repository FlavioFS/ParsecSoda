#include "ParsecSession.h"
bool ParsecSession::loadSessionCache()
{
	MetadataCache::SessionCache cache = MetadataCache::loadSessionCache();
	if (cache.isValid)
	{
		sessionId = cache.sessionID;
		hostPeerId = cache.peerID;
		_isValid = true;
		return true;
	}

	return false;
}

bool ParsecSession::saveSessionCache()
{
	MetadataCache::SessionCache cache = MetadataCache::SessionCache();
	cache.sessionID = sessionId;
	cache.peerID = hostPeerId;
	cache.isValid = true;

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

		result.success = MTY_JSONObjGetString(data, "verification_uri", result.verificationUri, 256);
		result.success = result.success && MTY_JSONObjGetString(data, "user_code", result.userCode, 16);
		result.success = result.success && MTY_JSONObjGetString(data, "hash", result.hash, 64);
		result.success = result.success && MTY_JSONObjGetUInt(data, "expires_at", &result.expiresAt);
		result.success = result.success && MTY_JSONObjGetUInt(data, "interval", &result.interval);

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

		success = MTY_JSONObjGetString(data, "id", id, 256);
		success = success && MTY_JSONObjGetString(data, "host_peer_id", peerId, 128);

		if (success)
		{
			this->sessionId = id;
			this->hostPeerId = peerId;

			MetadataCache::SessionCache cache;
			cache.sessionID = sessionId;
			cache.peerID = hostPeerId;
			cache.isValid = true;
			MetadataCache::saveSessionCache(cache);
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

		MetadataCache::SessionCache cache;
		cache.sessionID = sessionId;
		cache.peerID = hostPeerId;
		cache.isValid = true;
		MetadataCache::saveSessionCache(cache);

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

const bool ParsecSession::fetchArcadeRoomList()
{
	if (sessionId.empty()) {
		return false;
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
			const MTY_JSON* json = MTY_JSONParse(responseStr.c_str());

			return true;
		}
	}
	catch (const std::exception&)
	{
		bool debug = true;
	}

	//Soda::ParsecSession::hostPeerId = MTY_JSONSerialize(MTY_JSONObjGetItem(json, "host_peer_id"));
	//Soda::ParsecSession::sessionId = MTY_JSONSerialize(MTY_JSONObjGetItem(json, "session_id"));

	return false;
}

const bool ParsecSession::fetchAccountData(Guest *user)
{
	if (!_isValid || sessionId.empty()) {
		return false;
	}

	_accountDataThread = thread ([user, this]() {
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

				user->status = Guest::Status::OK;
				result = true;
			}
			catch (const std::exception&) {}

			MTY_JSONDestroy(&json);
		}
		else
		{
			user->name = "Host";
			user->userID = 0;
			user->status = Guest::Status::INVALID;
		}

		_accountDataThread.detach();
	});
}

bool& ParsecSession::isValid()
{
	return _isValid;
}

const string ParsecSession::getSessionError()
{
	return _sessionError;
}

const int ParsecSession::getSessionStatus()
{
	return _sessionStatus;
}
