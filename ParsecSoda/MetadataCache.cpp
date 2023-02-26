#include "MetadataCache.h"

// This is not ideal, especially in an open source environment.
// I'm using these values just as placeholders until I find an
// actual solution. You should change them in your build.
string MetadataCache::_key = "ParsecSodaKey***";
string MetadataCache::_nonce = "ParsecSoda**";
MetadataCache::Preferences MetadataCache::preferences = MetadataCache::Preferences();
mutex MetadataCache::_mutex;

GuestData MetadataCache::host = GuestData();

MetadataCache::SessionCache MetadataCache::loadSessionCache()
{
    SessionCache result = SessionCache();

    string dirPath = getUserDir();

    if (!dirPath.empty())
    {
        string filepath = dirPath + "session.json";

        if (MTY_FileExists(filepath.c_str()))
        {
            size_t size;
            const char* encryptedContent;
            encryptedContent = (char*)MTY_ReadFile(filepath.c_str(), &size);

            char originalText[256];
            MTY_AESGCM* ctx = MTY_AESGCMCreate(_key.c_str());
            char tag[16] = "ParsecSodaTag**";
            
            MTY_AESGCMDecrypt(ctx, _nonce.c_str(), encryptedContent, size, tag, (void*)originalText);
            MTY_JSON* json = MTY_JSONParse(originalText);
            if (json != nullptr)
            {
                char sessionID[128];
                char peerID[64];
                uint32_t type = (uint32_t)SessionCache::SessionType::THIRD;
                uint32_t expiry = 0, start = 0;

                bool success =
                    MTY_JSONObjGetString(json, "sessionID", sessionID, 128)
                    && MTY_JSONObjGetString(json, "peerID", peerID, 64)
                    && MTY_JSONObjGetUInt(json, "type", &type)
                    && MTY_JSONObjGetUInt(json, "start", &start)
                    && MTY_JSONObjGetUInt(json, "expiry", &expiry)
                    ;

                if (success)
                {
                    result.isValid = true;
                    result.sessionID = sessionID;
                    result.peerID = peerID;
                    result.type = (SessionCache::SessionType)type;
                    result.start = start;
                    result.expiry = expiry;
                }
                
                MTY_JSONDestroy(&json);
            }

            MTY_AESGCMDestroy(&ctx);
        }
    }

    return result;
}

bool MetadataCache::saveSessionCache(SessionCache sessionCache)
{
    bool result = false;
    string dirPath = getUserDir();

    if (!dirPath.empty())
    {
        string filepath = dirPath + "session.json";

        if (sessionCache.isValid)
        {
            MTY_JSON* json = MTY_JSONObjCreate();
            MTY_JSONObjSetString(json, "sessionID", sessionCache.sessionID.c_str());
            MTY_JSONObjSetString(json, "peerID", sessionCache.peerID.c_str());
            MTY_JSONObjSetUInt(json, "type", (uint32_t)sessionCache.type);
            MTY_JSONObjSetUInt(json, "start", sessionCache.start);
            MTY_JSONObjSetUInt(json, "expiry", sessionCache.expiry);

            string jsonStr = MTY_JSONSerialize(json);
            char encryptedJson[256];
            char tag[16] = "ParsecSodaTag**";
            MTY_AESGCM* ctx = MTY_AESGCMCreate(_key.c_str());
            if (MTY_AESGCMEncrypt(ctx, _nonce.c_str(), jsonStr.c_str(), jsonStr.size(), tag, encryptedJson))
            {
                if (MTY_WriteFile(filepath.c_str(), encryptedJson, jsonStr.size()))
                {
                    result = true;
                }
            }

            MTY_AESGCMDestroy(&ctx);
            MTY_JSONDestroy(&json);
        }
    }

    return result;
}

MetadataCache::Preferences MetadataCache::loadPreferences()
{
    preferences = Preferences();

    string dirPath = getUserDir();
    if (!dirPath.empty())
    {
        string filepath = dirPath + "preferences.json";

        if (MTY_FileExists(filepath.c_str()))
        {
            MTY_JSON *json = MTY_JSONReadFile(filepath.c_str());
            char roomName[256] = "", gameID[72] = "", secret[32] = "play-now";

            const auto clamp = [](int32_t& value, const int32_t& min = -8192, const int32_t& max = 8192) {
                if (value < min) value = min;
                if (value > max) value = max;
            };

            const auto tryLoadBool = [&json](const char* key, bool& pref, const bool& fallback = true) {
                if (!MTY_JSONObjGetBool(json, key, &pref)) pref = fallback;
            };

            const auto tryLoadUInt = [&json](const char* key, uint32_t& pref, const uint32_t& fallback = 0) {
                if (!MTY_JSONObjGetUInt(json, key, &pref)) pref = fallback;
            };

            const auto tryLoadUShort = [&json](const char* key, uint16_t& pref, const uint16_t& fallback = 0) {
                if (!MTY_JSONObjGetUInt16(json, key, &pref)) pref = fallback;
            };

            const auto tryLoadInt = [&json](const char* key, int32_t& pref, const int32_t& fallback = 0) {
                if (!MTY_JSONObjGetInt(json, key, &pref)) pref = fallback;
            };

            const auto tryLoadString = [&json](const char* key, char *val, const size_t size, string& pref, const char* fallback = "") {
                pref = (MTY_JSONObjGetString(json, key, val, size)) ? val : fallback;
            };


            tryLoadUInt("audioInputDevice", preferences.audioInputDevice);
            tryLoadUInt("audioOutputDevice", preferences.audioOutputDevice);
            tryLoadUInt("micVolume", preferences.micVolume, 80);
            tryLoadBool("micEnabled", preferences.micEnabled, true);
            tryLoadUInt("speakersVolume", preferences.speakersVolume, 30);
            tryLoadBool("speakersEnabled", preferences.speakersEnabled, true);
            tryLoadUInt("monitor", preferences.monitor);
            tryLoadUInt("adapter", preferences.adapter);           
            tryLoadString("roomName", roomName, 256, preferences.roomName, "Let's have fun!");
            tryLoadString("gameID", gameID, 72, preferences.gameID);
            tryLoadString("secret", secret, 72, preferences.secret, "play-now");
            tryLoadUInt("guestCount", preferences.guestCount, 1);
            tryLoadBool("publicRoom", preferences.publicRoom, false);            
            tryLoadInt("windowX", preferences.windowX); clamp(preferences.windowX);
            tryLoadInt("windowY", preferences.windowY); clamp(preferences.windowY);
            tryLoadUInt("windowW", preferences.windowW, 1280); if (preferences.windowW < 400) preferences.windowW = 1280;
            tryLoadUInt("windowH", preferences.windowH, 720);  if (preferences.windowH < 400) preferences.windowH = 720;
            tryLoadUInt("fps", preferences.fps, 60);
            tryLoadUInt("bandwidth", preferences.bandwidth, 20);
            tryLoadUInt("xboxPuppetCount", preferences.xboxPuppetCount, 4);
            tryLoadUInt("ds4PuppetCount", preferences.ds4PuppetCount, 0);            
            tryLoadBool("latencyLimiterEnabled", preferences.latencyLimiterEnabled, false);
            tryLoadUInt("maxLatencyMs", preferences.maxLatencyMs, 100);
            tryLoadUInt("hotseatDurationSeconds", preferences.hotseatDurationSeconds, 300);
            tryLoadUInt("defaultMultitapPadLimit", preferences.defaultMultitapPadLimit, 4);
            tryLoadBool("defaultMultitapValue", preferences.defaultMultitapValue, false);
            tryLoadBool("defaultMirrorValue", preferences.defaultMirrorValue, false);
            tryLoadUShort("buttonLock.buttons", preferences.buttonLock.buttons, 0);
            tryLoadBool("buttonLock.leftTrigger", preferences.buttonLock.leftTrigger, false);
            tryLoadBool("buttonLock.rightTrigger", preferences.buttonLock.rightTrigger, false);
            tryLoadBool("buttonLock.leftStick", preferences.buttonLock.leftStick, false);
            tryLoadBool("buttonLock.rightStick", preferences.buttonLock.rightStick, false);
            tryLoadBool("buttonLock.isEnabled", preferences.buttonLock.isEnabled, false);
            tryLoadBool("enableNotifications", preferences.enableNotifications, true);
            tryLoadBool("enableChatSoundNotification", preferences.enableChatSoundNotification, false);
            tryLoadBool("enableSfx", preferences.enableSfx, false);
            tryLoadBool("enableKickSfx", preferences.enableKickSfx, true);
            tryLoadBool("enableBanSfx", preferences.enableBanSfx, true);

            preferences.isValid = true;

            MTY_JSONDestroy(&json);
        }
    }

    return preferences;
}

bool MetadataCache::savePreferences(MetadataCache::Preferences preferences)
{
    if (!preferences.isValid)
    {
        return false;
    }

    string dirPath = getUserDir();
    if (!dirPath.empty())
    {
        string filepath = dirPath + "preferences.json";

        MTY_JSON *json = MTY_JSONObjCreate();
            
        MTY_JSONObjSetUInt(json, "audioInputDevice", preferences.audioInputDevice);
        MTY_JSONObjSetUInt(json, "audioOutputDevice", preferences.audioOutputDevice);
        MTY_JSONObjSetUInt(json, "micVolume", preferences.micVolume);
        MTY_JSONObjSetBool(json, "micEnabled", preferences.micEnabled);
        MTY_JSONObjSetUInt(json, "speakersVolume", preferences.speakersVolume);
        MTY_JSONObjSetBool(json, "speakersEnabled", preferences.speakersEnabled);
        MTY_JSONObjSetUInt(json, "monitor", preferences.monitor);
        MTY_JSONObjSetString(json, "roomName", preferences.roomName.c_str());
        MTY_JSONObjSetString(json, "gameID", preferences.gameID.c_str());
        MTY_JSONObjSetString(json, "secret", preferences.secret.c_str());
        MTY_JSONObjSetUInt(json, "guestCount", preferences.guestCount);
        MTY_JSONObjSetBool(json, "publicRoom", preferences.publicRoom);
        MTY_JSONObjSetInt(json, "windowX", preferences.windowX);
        MTY_JSONObjSetInt(json, "windowY", preferences.windowY);
        MTY_JSONObjSetUInt(json, "windowW", preferences.windowW);
        MTY_JSONObjSetUInt(json, "windowH", preferences.windowH);
        MTY_JSONObjSetUInt(json, "fps", preferences.fps);
        MTY_JSONObjSetUInt(json, "bandwidth", preferences.bandwidth);
        MTY_JSONObjSetUInt(json, "monitor", preferences.monitor);
        MTY_JSONObjSetUInt(json, "adapter", preferences.adapter);
        MTY_JSONObjSetUInt(json, "xboxPuppetCount", preferences.xboxPuppetCount);
        MTY_JSONObjSetUInt(json, "ds4PuppetCount", preferences.ds4PuppetCount);
        MTY_JSONObjSetBool(json, "latencyLimiterEnabled", preferences.latencyLimiterEnabled);
        MTY_JSONObjSetUInt(json, "maxLatencyMs", preferences.maxLatencyMs);
        MTY_JSONObjSetUInt(json, "hotseatDurationSeconds", preferences.hotseatDurationSeconds);
        MTY_JSONObjSetUInt(json, "defaultMultitapPadLimit", preferences.defaultMultitapPadLimit);
        MTY_JSONObjSetBool(json, "defaultMultitapValue", preferences.defaultMultitapValue);
        MTY_JSONObjSetBool(json, "defaultMirrorValue", preferences.defaultMirrorValue);
        MTY_JSONObjSetUInt(json, "buttonLock.buttons", preferences.buttonLock.buttons);
        MTY_JSONObjSetBool(json, "buttonLock.leftTrigger", preferences.buttonLock.leftTrigger);
        MTY_JSONObjSetBool(json, "buttonLock.rightTrigger", preferences.buttonLock.rightTrigger);
        MTY_JSONObjSetBool(json, "buttonLock.leftStick", preferences.buttonLock.leftStick);
        MTY_JSONObjSetBool(json, "buttonLock.rightStick", preferences.buttonLock.rightStick);
        MTY_JSONObjSetBool(json, "buttonLock.isEnabled", preferences.buttonLock.isEnabled);
        MTY_JSONObjSetBool(json, "enableNotifications", preferences.enableNotifications);
        MTY_JSONObjSetBool(json, "enableChatSoundNotification", preferences.enableChatSoundNotification);
        MTY_JSONObjSetBool(json, "enableSfx", preferences.enableSfx);
        MTY_JSONObjSetBool(json, "enableKickSfx", preferences.enableKickSfx);
        MTY_JSONObjSetBool(json, "enableBanSfx", preferences.enableBanSfx);

        MTY_JSONWriteFile(filepath.c_str(), json);
        MTY_JSONDestroy(&json);
            
        return true;
    }

    return false;
}

bool MetadataCache::savePreferences()
{
    return savePreferences(preferences);
}

vector<GuestData> MetadataCache::loadBannedUsers()
{
    vector<GuestData> result;

    string dirPath = getUserDir();
    if (!dirPath.empty())
    {
        string filepath = dirPath + "banned.json";

        if (MTY_FileExists(filepath.c_str()))
        {
            MTY_JSON* json = MTY_JSONReadFile(filepath.c_str());
            uint32_t size = MTY_JSONGetLength(json);

            for (size_t i = 0; i < size; i++)
            {
                const MTY_JSON* guest = MTY_JSONArrayGetItem(json, (uint32_t)i);
                
                char name [128] = "";
                uint32_t userID = 0;
                bool nameSuccess = MTY_JSONObjGetString(guest, "name", name, 128);
                bool userIDSuccess = MTY_JSONObjGetUInt(guest, "userID", &userID);

                if (nameSuccess && userIDSuccess)
                {
                    result.push_back(GuestData(name, userID));
                }
            }

            std::sort(result.begin(), result.end(), [](const GuestData a, const GuestData b) {
                return a.userID < b.userID;
            });

            MTY_JSONDestroy(&json);
        }
    }

    return result;
}

bool MetadataCache::saveBannedUsers(vector<GuestData> guests)
{
    string dirPath = getUserDir();

    if (!dirPath.empty())
    {
        string filepath = dirPath + "banned.json";

        MTY_JSON* json = MTY_JSONArrayCreate();

        vector<GuestData>::iterator gi = guests.begin();
        for (; gi != guests.end(); ++gi)
        {
            MTY_JSON* guest = MTY_JSONObjCreate();

            MTY_JSONObjSetString(guest, "name", (*gi).name.c_str());
            MTY_JSONObjSetUInt(guest, "userID", (*gi).userID);
            MTY_JSONArrayAppendItem(json, guest);
        }

        MTY_JSONWriteFile(filepath.c_str(), json);
        MTY_JSONDestroy(&json);

        return true;
    }

    return false;
}

vector<GuestTier> MetadataCache::loadGuestTiers()
{
    vector<GuestTier> result;

    string dirPath = getUserDir();
    if (!dirPath.empty())
    {
        string filepath = dirPath + "tiers.json";

        if (MTY_FileExists(filepath.c_str()))
        {
            MTY_JSON* json = MTY_JSONReadFile(filepath.c_str());
            uint32_t size = MTY_JSONGetLength(json);

            for (size_t i = 0; i < size; i++)
            {
                const MTY_JSON* guest = MTY_JSONArrayGetItem(json, (uint32_t)i);

                char name[128] = "";
                uint32_t userID, tier = 0;
                bool tierSuccess = MTY_JSONObjGetUInt(guest, "tier", &tier);
                bool userIDSuccess = MTY_JSONObjGetUInt(guest, "userID", &userID);

                if (tierSuccess && userIDSuccess)
                {
                    result.push_back(GuestTier(userID, (Tier)tier));
                }
            }

            std::sort(result.begin(), result.end(), [](const GuestTier a, const GuestTier b) {
                return a.userID < b.userID;
                });

            MTY_JSONDestroy(&json);
        }
    }

    return result;
}

bool MetadataCache::saveGuestTiers(vector<GuestTier> guestTiers)
{
    string dirPath = getUserDir();

    if (!dirPath.empty())
    {
        string filepath = dirPath + "tiers.json";

        MTY_JSON* json = MTY_JSONArrayCreate();

        vector<GuestTier>::iterator gi = guestTiers.begin();
        for (; gi != guestTiers.end(); ++gi)
        {
            MTY_JSON* guest = MTY_JSONObjCreate();

            MTY_JSONObjSetUInt(guest, "tier", (uint32_t)(*gi).tier);
            MTY_JSONObjSetUInt(guest, "userID", (*gi).userID);
            MTY_JSONArrayAppendItem(json, guest);
        }

        MTY_JSONWriteFile(filepath.c_str(), json);
        MTY_JSONDestroy(&json);

        return true;
    }

    return false;
}

vector<Thumbnail> MetadataCache::loadThumbnails()
{
    _mutex.lock();

    vector<Thumbnail> result;

    string dirPath = getUserDir();
    if (!dirPath.empty())
    {
        string filepath = dirPath + "thumbnails.json";

        if (MTY_FileExists(filepath.c_str()))
        {
            size_t size;
            const char* encryptedContent;
            encryptedContent = (char*)MTY_ReadFile(filepath.c_str(), &size);

            char *originalText = new char[size + 100];
            MTY_AESGCM *ctx = MTY_AESGCMCreate(_key.c_str());
            char tag[16] = "ParsecSodaTag**";

            MTY_AESGCMDecrypt(ctx, _nonce.c_str(), encryptedContent, size + 100, tag, (void*)originalText);
            MTY_JSON* json = MTY_JSONParse(originalText);
            uint32_t thumbnailCount = MTY_JSONGetLength(json);

            for (size_t i = 0; i < thumbnailCount; i++)
            {
                const MTY_JSON* guest = MTY_JSONArrayGetItem(json, (uint32_t)i);

                char name[256] = "", gameId[64] = "";

                bool success =
                    MTY_JSONObjGetString(guest, "gameId", gameId, 64) &&
                    MTY_JSONObjGetString(guest, "name", name, 256);

                if (success)
                {
                    result.push_back(Thumbnail(gameId, name, true));
                }
            }
            delete[] originalText;

            //std::sort(result.begin(), result.end(), [](const Thumbnail a, const Thumbnail b) {
            //    int compare = Stringer::toLower(a.name).compare(Stringer::toLower(b.name));
            //    return compare;
            //});

            MTY_JSONDestroy(&json);
            MTY_AESGCMDestroy(&ctx);
        }
    }

    _mutex.unlock();
    return result;
}

bool MetadataCache::saveThumbnails(vector<Thumbnail> thumbnails)
{
    _mutex.lock();

    bool result = false;
    string dirPath = getUserDir();

    if (!dirPath.empty())
    {
        string filepath = dirPath + "thumbnails.json";

        MTY_JSON* json = MTY_JSONArrayCreate();

        vector<Thumbnail>::iterator it = thumbnails.begin();
        for (; it != thumbnails.end(); ++it)
        {
            if ((*it).saved)
            {
                MTY_JSON* thumb = MTY_JSONObjCreate();

                MTY_JSONObjSetString(thumb, "gameId", (*it).gameId.c_str());
                MTY_JSONObjSetString(thumb, "name", (*it).name.c_str());
                MTY_JSONArrayAppendItem(json, thumb);
            }
        }

        string jsonStr = MTY_JSONSerialize(json);
        char *encryptedJson = new char[jsonStr.size() + 100];
        char tag[16] = "ParsecSodaTag**";
        MTY_AESGCM* ctx = MTY_AESGCMCreate(_key.c_str());
        if (MTY_AESGCMEncrypt(ctx, _nonce.c_str(), jsonStr.c_str(), jsonStr.size() + 100, tag, encryptedJson))
        {
            if (MTY_WriteFile(filepath.c_str(), encryptedJson, jsonStr.size()))
            {
                result = true;
            }
        }
        delete[] encryptedJson;

        MTY_JSONDestroy(&json);
        MTY_AESGCMDestroy(&ctx);
    }

    _mutex.unlock();
    return result;
}

const string MetadataCache::getSfxPath()
{
    return getUserDir() + "sfx\\";
}

const string MetadataCache::getUserDir()
{
    TCHAR tAppdata[1024];
    if (SUCCEEDED(SHGetFolderPath(nullptr, CSIDL_APPDATA, NULL, 0, tAppdata)))
    {
        wstring wAppdata(tAppdata);
        string appdata(wAppdata.begin(), wAppdata.end());
        string dirPath = appdata + "\\ParsecSoda\\";
        
        bool isDirOk = false;

        if (!MTY_FileExists(dirPath.c_str()))
        {
            if (MTY_Mkdir(dirPath.c_str()))
            {
                isDirOk = true;
            }
        }
        else
        {
            isDirOk = true;
        }

        if (isDirOk)
        {
            return dirPath;
        }
    }

    return string();
}