#include "ParsecSession.h"

void ParsecSession::mockSession(const char * sessionId, const char * hostPeerId)
{
	this->sessionId = sessionId;
	this->hostPeerId = hostPeerId;
}


void ParsecSession::mockSession(bool isTestAccount)
{
	const char* session;
	const char* hostpeerid;

	if (isTestAccount)
	{
		session = MOCK_HOST_SESSION_ACC_TEST;
		hostpeerid = MOCK_HOST_PEER_ACC_TEST;
	}
	else
	{
		session = MOCK_HOST_SESSION_ACC_MAIN;
		hostpeerid = MOCK_HOST_PEER_ACC_MAIN;
	}

	this->mockSession(session, hostpeerid);
}

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
