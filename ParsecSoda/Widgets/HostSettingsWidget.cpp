#include "HostSettingsWidget.h"

HostSettingsWidget::HostSettingsWidget(Hosting& hosting, function<void(bool)> onHostRunningStatusCallback)
    : _hosting(hosting), _audioIn(_hosting.audioIn), _audioOut(_hosting.audioOut), _onHostRunningStatusCallback(onHostRunningStatusCallback)
{
    ParsecHostConfig cfg = hosting.getHostConfig();
    try
    {
        strcpy_s(_roomName, cfg.name);
        strcpy_s(_gameID, cfg.gameID);
        strcpy_s(_secret, cfg.secret);
    }
    catch (const std::exception&)
    {
        try
        {
            strcpy_s(_roomName, "");
            strcpy_s(_gameID, "");
            strcpy_s(_secret, "");
        } catch (const std::exception&) {}
    }
    _publicGame = cfg.publicGame;
    _maxGuests = cfg.maxGuests;
    
    _micVolume = MetadataCache::preferences.micVolume;
    _audioIn.volume = (float)_micVolume / 100.0f;
    
    _speakersVolume = MetadataCache::preferences.speakersVolume;
    _audioOut.volume = (float)_speakersVolume / 100.0f;

    _audioIn.isEnabled = MetadataCache::preferences.micEnabled;
    _audioOut.isEnabled = MetadataCache::preferences.speakersEnabled;


    vector<Thumbnail>::iterator it;
    _hosting.getSession().findThumbnail(_gameID, [&](Thumbnail& thumb) {
        _thumbnailName = thumb.name;
    });

    updateSecretLink();
}

bool HostSettingsWidget::render()
{
    static float indentSize = 0;
    static ImVec2 dummySize = ImVec2(0.0f, 5.0f);
    static ImVec2 cursor;

    AppStyle::pushTitle();

    ImGui::SetNextWindowSizeConstraints(ImVec2(340, 620), ImVec2(600, 900));
    ImGui::Begin("Host Settings", (bool*)0);
    AppStyle::pushLabel();

    static ImVec2 size;
    static ImVec2 pos;
    size = ImGui::GetContentRegionAvail();
    pos = ImGui::GetWindowPos();


    // ====================================================
    // Edit Button
    // ====================================================
    if (_hosting.isRunning() && isDirty())
    {
        cursor = ImGui::GetCursorPos();
        ImGui::SetCursorPos(ImVec2(size.x - 30.0f, 35.0f));

        if (IconButton::render(AppIcons::submit, AppColors::primary))
        {
            _hosting.setHostConfig(_roomName, _gameID, _maxGuests, _publicGame, _secret);
            _hosting.applyHostConfig();
            savePreferences();
        }
        TitleTooltipWidget::render("Update Room Settings", "The room will be instantly updated with your new settings.");

        ImGui::SetCursorPos(cursor);
    }
    
    ImGui::Dummy(ImVec2(0, 10.0f));


    // ====================================================
    // Name
    // ====================================================
    ImGui::Text("Room name");
    AppStyle::pushInput();
    ImGui::InputTextMultiline(" ", _roomName, MAX_ROOM_NAME_SIZE, ImVec2(size.x, 50));
    TitleTooltipWidget::render("Room Title", "The text displayed below thumbnails in Arcade list.");
    AppStyle::pop();

    ImGui::Dummy(dummySize);


    // ====================================================
    // Thumbnail
    // ====================================================
    ImGui::Text("Thumbnail");
    ImGui::SetNextItemWidth(size.x);
    AppStyle::pushInput();

    static vector<Thumbnail>::iterator it;
    _hosting.getSession().getThumbnails([&](vector<Thumbnail>& thumbnails) {
        if (ImGui::BeginCombo("### Thumbnail picker combo", _thumbnailName.c_str(), ImGuiComboFlags_HeightLarge))
        {
            it = thumbnails.begin();
            for (; it != thumbnails.end(); ++it)
            {
                bool isSelected = (it->gameId.compare(_gameID) == 0);
                if (ImGui::Selectable(it->name.c_str(), isSelected))
                {
                    _thumbnailName = it->name;
                    try
                    {
                        strcpy_s(_gameID, GAME_ID_LEN, it->gameId.c_str());
                    }
                    catch (const std::exception&) {}
                }
                if (isSelected)
                {
                    ImGui::SetItemDefaultFocus();
                }
            }
            ImGui::EndCombo();
        }
        TitleTooltipWidget::render("Thumbnail Picker", "To find new thumbnails, go to the Arcade Thumbnails window.");
    });
    
    AppStyle::pop();

    ImGui::Dummy(dummySize);


    // ====================================================
    // Room Link
    // ====================================================
    ImGui::Text("Room link");
    ImGui::SetNextItemWidth(size.x);
    AppStyle::pushInput();
    if (strlen(_secret) < LINK_COMPATIBLE_SECRET_SIZE - 1) AppColors::pushNegative();
    else AppColors::pushPositive();
    if (ImGui::InputText("##Secret input", _secret, LINK_COMPATIBLE_SECRET_SIZE))
    {
        updateSecretLink();
    }
    AppColors::pop();
    TitleTooltipWidget::render("Room Secret", "Generates the share link that lets people\njoin your room anytime.\nMust have 8 characters.");
    AppStyle::pop();

    if (BadgeButtonWidget::render(AppIcons::copy, "Copy Link", "Press to copy the room link.", "###Copy secret link btn", ImVec2(20, 20)))
    {
        Utils::copyToClipboard(_secretLink);
    }

    ImGui::SameLine();
    
    ImGui::SetNextItemWidth(size.x - 35);
    ImGui::InputText("##Secret link", _secretLink, 128, ImGuiInputTextFlags_ReadOnly);

    ImGui::Dummy(dummySize);

    cursor = ImGui::GetCursorPos();

    // ====================================================
    // Guest Slots
    // ====================================================
    ImGui::BeginChild("##Guest Slots child", ImVec2(90.0f, 60.0f));
    ImGui::Text("Guest slots");
    if (IntRangeWidget::render<uint32_t>("guest count", _maxGuests, 0, 64, nullptr, 0.025f))
    {
        TitleTooltipWidget::render("Room Slots", "How many guests do you want in this room?");
    }
    ImGui::EndChild();

    ImGui::SameLine();


    // ====================================================
    // Latency Limiter
    // ====================================================
    ImGui::BeginChild("##Latency limiter child", ImVec2(120.0f, 60.0f));
    ImGui::Text("Latency Limit");

    BoolButtonWidget::render(nullptr, MetadataCache::preferences.latencyLimiterEnabled, ImVec2(30, 30));
    if (MetadataCache::preferences.latencyLimiterEnabled)   TitleTooltipWidget::render("Enabled", "Guests kicked if ping exceeds maximum limit.");
    else                                                    TitleTooltipWidget::render("Disabled", "Ping does not matter.");

    ImGui::SameLine();

    if (IntRangeWidget::render<uint32_t>("Latency Limit intrange", MetadataCache::preferences.maxLatencyMs, 0, 999, nullptr, 0.025f))
    {
        TitleTooltipWidget::render("Latency Limit (ms)", "Any guest whose ping is higher than this value (ms)\nwill be kicked automatically.");
    }
    ImGui::EndChild();

    ImGui::SameLine();

    // ====================================================
    // Public / Private
    // ====================================================
    if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
    {
        bool debug = true;
    }
    //ImGui::SetCursorPos(ImVec2(cursor.x + size.x - 90.0f, cursor.y));
    BoolButtonWidget::render("Public room", _publicGame, ImVec2(30, 30));
    if (_publicGame)    TitleTooltipWidget::render("Public Game", "Anyone can enter this room.");
    else                TitleTooltipWidget::render("Private Game", "All guests must use the secret link to enter this room.");

    ImGui::Dummy(dummySize);


    // ====================================================
    // Start / Stop
    // ====================================================
    static bool showPopup = false;
    static string popupTitle = "";
    popupTitle = (_hosting.isRunning() ? "Stop hosting?" : "Start hosting?");

    indentSize = 0.5f * size.x - 45.0f;
    ImGui::Indent(indentSize);
    if (ToggleIconButtonWidget::render(
        AppIcons::stop, AppIcons::play, _hosting.isRunning(),
        AppColors::negative, AppColors::positive, ImVec2(90, 90)
    ))
    {
        showPopup = true;
        ImGui::OpenPopup(popupTitle.c_str());
        
        savePreferences();
    }
    ImGui::Unindent(indentSize);

    if (_hosting.isRunning())   TitleTooltipWidget::render("Stop hosting", "Close current room.");
    else                        TitleTooltipWidget::render("Start hosting", "Open a new room using these settings.");

    // ================================================================================

    if (ConfirmPopupWidget::render(popupTitle.c_str(), showPopup))
    {
        // Was clicked and is already running (must stop)
        if (_hosting.isRunning())
        {
            _hosting.stopHosting();
            if (_onHostRunningStatusCallback != nullptr) _onHostRunningStatusCallback(false);
        }

        // Was clicked and is not running (must start)
        else
        {
            _hosting.setHostConfig(_roomName, _gameID, _maxGuests, _publicGame, _secret);
            _hosting.applyHostConfig();
            _hosting.startHosting();
            if (_onHostRunningStatusCallback != nullptr) _onHostRunningStatusCallback(true);
        }
    }

    // ================================================================================

    ImGui::Dummy(ImVec2(0.0f, size.y - 548.0f));



    // ====================================================
    // Audio
    // ====================================================
    if (!_hosting.isRunning() && _hosting.isReady())
    {
        _audioIn.captureAudio();
        _audioOut.captureAudio();
    }

    static int previousMicVolume, previousSpeakersVolume;
    static bool isVolumeChanged = false;
    previousMicVolume = _micVolume;
    previousSpeakersVolume = _speakersVolume;

    // ====================================================
    // Microphone
    // ====================================================
    static float micPreview, targetPreview;
    _micVolume = (int)(100.0f * _audioIn.volume);
    targetPreview = AudioTools::decibelToFloat(_audioIn.popPreviewDecibel());
    micPreview = lerp(micPreview, targetPreview, easing(targetPreview - micPreview));

    const static auto volumeReleaseCallback = [&]() {
        savePreferences();
    };

    if (AudioControlWidget::render("Microphone", &_micVolume, _audioIn.isEnabled, micPreview, AppIcons::micOn, AppIcons::micOff, volumeReleaseCallback))
    {
        _audioIn.isEnabled = !_audioIn.isEnabled;
        savePreferences();
    }
    _audioIn.volume = (float)_micVolume / 100.0f;

    // ====================================================
    // Speakers
    // ====================================================
    static float speakersPreview;
    _speakersVolume = (int)(100.0f *_audioOut.volume);
    targetPreview = AudioTools::decibelToFloat(_audioOut.popPreviewDecibel());
    speakersPreview = lerp(speakersPreview, targetPreview, easing(targetPreview - speakersPreview));
    if (AudioControlWidget::render("Speakers", &_speakersVolume, _audioOut.isEnabled, speakersPreview, AppIcons::speakersOn, AppIcons::speakersOff, volumeReleaseCallback))
    {
        _audioOut.isEnabled = !_audioOut.isEnabled;
        savePreferences();
    }
    _audioOut.volume = (float)_speakersVolume / 100.0f;

    AppStyle::pop();
    ImGui::End();
    AppStyle::pop();

    return true;
}

void HostSettingsWidget::savePreferences()
{
    MetadataCache::preferences.roomName = _roomName;
    MetadataCache::preferences.gameID = strlen(_gameID) <= 25 ? "1wdoHfhhZH5lPuZCwGBete0HIAj" : _gameID;
    MetadataCache::preferences.guestCount = _maxGuests;
    MetadataCache::preferences.publicRoom = _publicGame;
    MetadataCache::preferences.secret = _secret;
    MetadataCache::preferences.micVolume = _micVolume;
    MetadataCache::preferences.micEnabled = _audioIn.isEnabled;
    MetadataCache::preferences.speakersVolume = _speakersVolume;
    MetadataCache::preferences.speakersEnabled = _audioOut.isEnabled;
    MetadataCache::savePreferences();
}

void HostSettingsWidget::updateSecretLink()
{
    try
    {
        strcpy_s(
            _secretLink,
            128,
            (string("https://parsec.gg/g/") + _hosting.getSession().hostPeerId + "/" + _secret + "/").c_str()
        );
    }
    catch (const std::exception&) {}
}

bool HostSettingsWidget::isDirty()
{
    ParsecHostConfig cfg = _hosting.getHostConfig();

    if (
        _publicGame != cfg.publicGame ||
        _maxGuests != cfg.maxGuests ||
        (strcmp(_roomName, cfg.name) != 0) ||
        (strcmp(_gameID, cfg.gameID) != 0) ||
        (strcmp(_secret, cfg.secret) != 0)
    ) return true;

    return false;
}

float HostSettingsWidget::lerp(float val1, float val2, float t)
{
    return (1-t) * val1 + t * val2;
}

float HostSettingsWidget::easing(float t)
{
    return 0.3f * max(abs(t), 0.4f);
}
