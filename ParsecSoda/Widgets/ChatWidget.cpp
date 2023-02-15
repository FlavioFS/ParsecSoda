#include "ChatWidget.h"

ChatWidget::ChatWidget(Hosting& hosting, function<void(void)> onMessageCallback)
    : _hosting(hosting), _chatLog(hosting.getChatLog()), _messageCount(0), _onMessageCallback(onMessageCallback)
{
    setSendBuffer("\0");
}

bool ChatWidget::render()
{
    ImGui::PushStyleVar(ImGuiStyleVar_WindowPadding, ImVec2(10, 5));

    static bool isClearChat = false;
    static bool isWindowLocked = false;
    static Stopwatch stopwatch = Stopwatch(2000);
    stopwatch.start();

    AppStyle::pushTitle();
    ImGui::SetNextWindowSizeConstraints(ImVec2(300, 400), ImVec2(800, 900));
    ImGui::Begin("Chat", (bool*)0, isWindowLocked ? (ImGuiWindowFlags_NoMove | ImGuiWindowFlags_NoResize) : 0);
    AppStyle::pushInput();

    static ImVec2 size;
    size = ImGui::GetContentRegionAvail();

    ImVec2 cursor;

    static size_t index;
    index = 0;

    static vector<string>::iterator it;
    _chatLog.getMessageLog([&](vector<string>& messages) {
    
        it = messages.begin();

        renderTopBar(isWindowLocked, isClearChat);
        ImGui::Separator();

        ImGui::BeginChild("Chat Log", ImVec2(size.x, size.y - 210));
        for (; it != messages.end(); ++it)
        {
            static float textHeight;
            cursor = ImGui::GetCursorPos();
        
            ImGui::TextWrapped((*it).c_str());
            textHeight = ImGui::GetCursorPosY() - cursor.y;

            ImGui::SetCursorPos(cursor);
            if (ImGui::Button(
                (string() + "### Chat Message " + to_string(index)).c_str(),
                ImVec2(size.x, textHeight)
            ))
            {
                toClipboard((*it));
                stopwatch.reset();
            }
            ++index;
        }
        if (_messageCount != messages.size())
        {
            if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 10)
            {
                ImGui::SetScrollHereY(1.0f);
            }
            _messageCount = messages.size();

            if (_onMessageCallback != nullptr)
            {
                _onMessageCallback();
            }
        }
        ImGui::EndChild();

    });


    ImGui::BeginChild("Message Preview", ImVec2(size.x, 60));
    ImGui::Separator();
    ImGui::TextWrapped(_previewBuffer);
    ImGui::EndChild();

    if (ImGui::IsWindowFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
        ImGui::SetKeyboardFocusHere(0);
    ImGui::SetNextItemWidth(size.x);
    try
    {
        strcpy_s(_lastBuffer, SEND_BUFFER_LEN, _sendBuffer);
    }
    catch (const std::exception&) {}
    if (ImGui::InputText(" ", _sendBuffer, SEND_BUFFER_LEN, ImGuiInputTextFlags_EnterReturnsTrue))
    {
        sendMessage();
    }
    if (strcmp(_sendBuffer, _lastBuffer) != 0)
    {
        string newSendBuffer = _sendBuffer;
        Stringer::replacePatternOnce(newSendBuffer, "%", "%%");

        string check = newSendBuffer.substr(0, SEND_BUFFER_LEN-1);

        try
        {
            strcpy_s(_previewBuffer, SEND_BUFFER_LEN, newSendBuffer.substr(0, SEND_BUFFER_LEN-1).c_str());
        }
        catch (const std::exception&) {}
    }

    cursor = ImGui::GetCursorPos();
    ImGui::Dummy(ImVec2(0, 5));
    if (stopwatch.getRemainingTime() > 0)
    {
        static float fill = 1.0f;
        fill = (float)stopwatch.getRemainingTime() / stopwatch.getDuration();
        ImGui::TextColored(ImVec4(1.0f, 1.0f, 1.0f, fill*fill), "Message copied.");
    }

    ImGui::SetCursorPos(cursor);

    ImGui::Indent(size.x - 50);
    
    if (ToggleIconButtonWidget::render(
        AppIcons::send, AppIcons::send, isDirty(),
        AppColors::primary, AppColors::alpha(AppColors::primary, 0.25f)
    ))
    {
        sendMessage();
    }

    AppStyle::pop();
    ImGui::End();
    AppStyle::pop();

    ImGui::PopStyleVar();

    if (isClearChat)
    {
        _chatLog.clearMessages();
        ChatBot* chatBot = _hosting.getChatBot();
        if (chatBot != nullptr) {
            chatBot->setLastUserId();
        }
        isClearChat = false;
    }

    return true;
}

bool ChatWidget::renderTopBar(bool& isWindowLocked, bool& isClearChat)
{
    static ImVec2 cursor;
    static bool result;
    result = false;

    static bool isDeletingChat = false;
    if (!isDeletingChat)
    {
        if (IconButton::render(AppIcons::trash, AppColors::primary, ImVec2(30, 30)))
        {
            isDeletingChat = true;
        }
        TitleTooltipWidget::render("Clear Chat", "Deletes all chat messages.");
    }
    else
    {
        ImGui::BeginGroup();

        AppStyle::pushInput();
        ImGui::Text("Delete chat history?");
        AppStyle::pop();

        static bool btnYes, btnNo;

        btnNo = IconButton::render(AppIcons::no, AppColors::negative, ImVec2(30, 30));
        ImGui::SameLine();
        btnYes = IconButton::render(AppIcons::yes, AppColors::positive, ImVec2(30, 30));

        if (btnYes) {
            isClearChat = true;
            isDeletingChat = false;
            result = true;
        }
        else if (btnNo)
        {
            isDeletingChat = false;
        }

        ImGui::EndGroup();
    }

    ImGui::SameLine();
    cursor = ImGui::GetCursorPos();
    ImGui::SetCursorPosX(ImGui::GetWindowWidth() - 40);

    if (ToggleIconButtonWidget::render(AppIcons::moveOff, AppIcons::move, isWindowLocked, AppColors::negative, AppColors::positive, ImVec2(30, 30)))
    {
        isWindowLocked = !isWindowLocked;
    }
    if (isWindowLocked) TitleTooltipWidget::render("Window Locked", "This window cannot move or resize.");
    else TitleTooltipWidget::render("Window Unlocked", "This window can move and resize.");

    return result;
}

bool ChatWidget::isDirty()
{
    static size_t len;
    try
    {
        len = strnlen_s(_sendBuffer, SEND_BUFFER_LEN);
    }
    catch (const std::exception&)
    {
        len = 0;
    }
    return len > 0;
}


void ChatWidget::sendMessage()
{
    if (_hosting.isRunning() && strlen(_sendBuffer) > 0)
    {
        _hosting.sendHostMessage(_sendBuffer);
    }

    setSendBuffer("\0");
}

bool ChatWidget::setSendBuffer(const char* value)
{
    try
    {
        strcpy_s(_sendBuffer, SEND_BUFFER_LEN, value);
        strcpy_s(_lastBuffer, SEND_BUFFER_LEN, value);
        strcpy_s(_previewBuffer, SEND_BUFFER_LEN, value);
        return true;
    }
    catch (const std::exception&)
    {
        return false;
    }

    return false;
}

void ChatWidget::toClipboard(const string& message)
{
    string adjustedMessage = message;

    if (Stringer::startsWithPattern(adjustedMessage.c_str(), "\t\t "))
    {
        Stringer::replacePatternOnce(adjustedMessage, "\t\t ", "");
    }

    OpenClipboard(0);
    EmptyClipboard();
    HGLOBAL hg = GlobalAlloc(GMEM_MOVEABLE, adjustedMessage.size()+1);
    if (!hg) {
        CloseClipboard();
        return;
    }
    memcpy(GlobalLock(hg), adjustedMessage.c_str(), adjustedMessage.size());
    GlobalUnlock(hg);
    SetClipboardData(CF_TEXT, hg);
    CloseClipboard();
    GlobalFree(hg);
}