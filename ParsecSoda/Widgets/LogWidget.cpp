#include "LogWidget.h"

LogWidget::LogWidget(Hosting& hosting)
    : _commandLog(hosting.getCommandLog()), _messageCount(0)
{
}

bool LogWidget::render()
{
    AppStyle::pushTitle();
    ImGui::SetNextWindowSizeConstraints(ImVec2(300, 200), ImVec2(800, 900));
    ImGui::Begin("Log", (bool*)0);
    AppStyle::pushInput();

    ImVec2 size = ImGui::GetContentRegionAvail();

    static vector<string>::iterator it;
    it = _commandLog.begin();
    ImGui::BeginChild("Log text", ImVec2(size.x, size.y));
    for (; it != _commandLog.end(); ++it)
    {
        if ((*it)[0] == '@')
        {
            AppStyle::pushPositive();
            ImGui::TextWrapped((*it).c_str());
            AppStyle::pop();
        }
        else if ((*it)[0] == '!')
        {
            AppStyle::pushNegative();
            ImGui::TextWrapped((*it).c_str());
            AppStyle::pop();
        }
        else
        {
            ImGui::TextWrapped((*it).c_str());
        }
    }
    if (_messageCount != _commandLog.size())
    {
        if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 10)
        {
            ImGui::SetScrollHereY(1.0f);
        }
        _messageCount = _commandLog.size();
    }
    ImGui::EndChild();

    AppStyle::pop();
    ImGui::End();
    AppStyle::pop();

    return true;
}