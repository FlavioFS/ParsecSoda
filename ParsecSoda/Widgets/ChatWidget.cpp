#include "ChatWidget.h"

ChatWidget::ChatWidget(AppIcons& icons, Hosting& hosting)
    : _icons(icons), _hosting(hosting), _chatLog(hosting.getMessageLog())
{
    _sendBtn = ToggleTexButtonWidget (icons.send, icons.send_off, false);
    setSendBuffer("\0");
}

bool ChatWidget::render(AppStyle& style)
{
    style.pushTitle();
    ImGui::SetNextWindowSizeConstraints(ImVec2(300, 400), ImVec2(800, 900));
    ImGui::Begin("Chat", (bool*)0);
    style.pushInput();
    
    ImVec2 size = ImGui::GetContentRegionAvail();

    static vector<string>::iterator it;
    it = _chatLog.begin();
    ImGui::BeginChild("Chat Log", ImVec2(size.x, size.y - 160));
    for (; it != _chatLog.end(); ++it)
    {
        ImGui::TextWrapped((*it).c_str());
    }
    //ImGui::TextWrapped(_logBuffer.c_str());
    ImGui::EndChild();

    ImGui::Separator();

    ImGui::BeginChild("Message Preview", ImVec2(size.x, 60));
    ImGui::TextWrapped(_sendBuffer);
    ImGui::EndChild();
    
    if (ImGui::IsWindowFocused() && !ImGui::IsAnyItemActive() && !ImGui::IsMouseClicked(0))
        ImGui::SetKeyboardFocusHere(0);
    ImGui::SetNextItemWidth(size.x);
    if (ImGui::InputText(" ", _sendBuffer, SEND_BUFFER_LEN, ImGuiInputTextFlags_EnterReturnsTrue))
    {
        sendMessage();
    }

    ImGui::Indent(size.x - 50);
    _sendBtn.setOn(isDirty());
    if (_sendBtn.render())
    {
        sendMessage();
    }

    style.pop();
    ImGui::End();
    style.pop();

    return true;
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
    if (_hosting.isRunning())
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
        return true;
    }
    catch (const std::exception&)
    {
        return false;
    }

    return false;
}
