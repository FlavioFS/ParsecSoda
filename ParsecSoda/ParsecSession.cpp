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

const bool ParsecSession::fetchSession(const char* email, const char* password, const char* tfa)
{
	if (!hostPeerId.empty() && !sessionId.empty())
	{
		return true;
	}
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

	if (responseSize > 0 && status == 201)
	{
		const char* responseStr = (const char*)response;
		const MTY_JSON* json = MTY_JSONParse(responseStr);
		hostPeerId = MTY_JSONSerialize(MTY_JSONObjGetItem(json, "host_peer_id"));
		sessionId = MTY_JSONSerialize(MTY_JSONObjGetItem(json, "session_id"));

		Utils::removeCharFromString(&hostPeerId, '"');
		Utils::removeCharFromString(&sessionId, '"');

		return true;
	}

	return false;
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

const bool ParsecSession::fetchAccountData(Guest& user)
{
	if (sessionId.empty()) {
		return false;
	}

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
	size_t responseSize;
	uint16_t status;

	const bool rekt = MTY_HttpRequest(
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

		try
		{
			string responseStr = (const char*)response;
			json = MTY_JSONParse(responseStr.c_str());

			user.name = MTY_JSONSerialize(MTY_JSONObjGetItem(json, "name"));
			user.userID = stoi(MTY_JSONSerialize(MTY_JSONObjGetItem(json, "id")));

			result = true;
		}
		catch (const std::exception&) {}

		MTY_JSONDestroy(&json);
		return result;
	}

	return false;
}