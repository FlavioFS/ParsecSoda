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
	std::ostringstream ss;
	ss << "{"
		<< "\"" << "email" << "\":\"" << email << "\","
		<< "\"" << "password" << "\":\"" << password << "\","
		<< "\"" << "tfa" << "\":\"" << tfa << "\"}";
	const std::string bodyString = ss.str();
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
	std::ostringstream ss;
	ss << PARSEC_API_V2_HOSTS << "?mode=game&public=true";
	const std::string pathString = ss.str();
	const char *path = pathString.c_str();

	ss.str("");
	ss.clear();
	const char *session = ParsecSession::sessionId.c_str();
	ss << HEADER_AUTH_BEARER << ParsecSession::sessionId.c_str();
	const std::string headerString = ss.str();
	const char *header = headerString.c_str();

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
		std::string responseStr = (const char*)response;
		const MTY_JSON* json = MTY_JSONParse(responseStr.c_str());

		return true;
	}

	//Soda::ParsecSession::hostPeerId = MTY_JSONSerialize(MTY_JSONObjGetItem(json, "host_peer_id"));
	//Soda::ParsecSession::sessionId = MTY_JSONSerialize(MTY_JSONObjGetItem(json, "session_id"));

	return false;
}

bool ParsecSession::compareHostConfigs(ParsecHostConfig cfg, ParsecHostConfig other)
{
	return (
		cfg.gamepadType == other.gamepadType &&
		cfg.adminMute == other.adminMute &&
		cfg.exclusiveInput == other.exclusiveInput &&
		cfg.maxGuests == other.maxGuests &&
		(strcmp(cfg.name, other.name) == 0) &&
		(strcmp(cfg.desc, other.desc) == 0) &&
		(strcmp(cfg.gameID, other.gameID) == 0) &&
		(strcmp(cfg.secret, other.secret) == 0) &&
		cfg.publicGame == other.publicGame
	);
}
