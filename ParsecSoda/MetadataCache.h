#pragma once

#include <string>
#include <vector>
#include <mutex>
#include <algorithm>
#include <Windows.h>
#include <ShlObj.h>
#include "matoya.h"
#include "GuestData.h"
#include "GuestTier.h"
#include "Thumbnail.h"
#include "Stringer.h"
#include "ButtonLock.h"

using namespace std;

class MetadataCache
{
public:
	class SessionCache
	{
	public:
		enum class SessionType
		{
			PERSONAL = 0,
			THIRD = 1,
			ENTERPRISE = 2
		};
		
		string sessionID = "";
		string peerID = "";
		SessionType type = SessionType::THIRD;
		uint32_t start = 0;
		uint32_t expiry = 0;
		bool isValid = false;
	};

	class Preferences
	{
	public:
		uint32_t audioInputDevice = 0;
		uint32_t audioOutputDevice = 0;
		uint32_t micVolume = 80;
		bool micEnabled = true;
		uint32_t speakersVolume = 30;
		bool speakersEnabled = true;
		uint32_t monitor = 0;
		uint32_t adapter = 0;
		string roomName = "";
		string gameID = "";
		string secret = "play-now";
		uint32_t guestCount = 1;
		bool publicRoom = false;
		bool isValid = false;
		int windowX = 0;
		int windowY = 0;
		uint32_t windowW = 1280;
		uint32_t windowH = 720;
		uint32_t fps = 60;
		uint32_t bandwidth = 20;
		uint32_t xboxPuppetCount = 4;
		uint32_t ds4PuppetCount = 0;
		bool latencyLimiterEnabled = false;
		uint32_t maxLatencyMs = 100;
		uint32_t hotseatDurationSeconds = 300;
		uint32_t defaultMultitapPadLimit = 4;
		bool defaultMultitapValue = false;
		bool defaultMirrorValue = false;
		bool enableGuideButton = false;
		ButtonLock buttonLock;
	};

	static SessionCache loadSessionCache();
	static bool saveSessionCache(SessionCache sessionCache);

	static Preferences loadPreferences();
	static bool savePreferences(Preferences preferences);
	static bool savePreferences();

	static vector<GuestData> loadBannedUsers();
	static bool saveBannedUsers(vector<GuestData> guests);

	static vector<GuestTier> loadGuestTiers();
	static bool saveGuestTiers(vector<GuestTier> guestTiers);

	static vector<Thumbnail> loadThumbnails();
	static bool saveThumbnails(vector<Thumbnail> thumbnails);

	static Preferences preferences;

	static GuestData host;

private:
	static string getUserDir();
	
	// This is not ideal, especially in an open source environment.
	// I'm using these values just as placeholders until I find an
	// actual solution. You should change them in your build.
	/** Must have 16 bytes. */
	static string _key;
	/** Must have 12 bytes. */
	static string _nonce;

	static mutex _mutex;
};