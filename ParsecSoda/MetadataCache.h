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
		unsigned int audioInputDevice = 0;
		unsigned int audioOutputDevice = 0;
		unsigned int micFrequency = 44100;
		unsigned int micVolume = 80;
		bool micEnabled = true;
		unsigned int speakersFrequency = 44100;
		unsigned int speakersVolume = 30;
		bool speakersEnabled = true;
		unsigned int monitor = 0;
		unsigned int adapter = 0;
		string roomName = "";
		string gameID = "";
		string secret = "";
		unsigned int guestCount = 1;
		bool publicRoom = false;
		bool isValid = false;
		int windowX = 0;
		int windowY = 0;
		unsigned int windowW = 1280;
		unsigned int windowH = 720;
		unsigned int fps = 60;
		unsigned int bandwidth = 20;
		unsigned int xboxPuppetCount = 4;
		unsigned int ds4PuppetCount = 0;
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