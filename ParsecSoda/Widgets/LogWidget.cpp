#include "LogWidget.h"

LogWidget::LogWidget(Hosting& hosting)
    : _chatLog(hosting.getChatLog()), _messageCount(0)
{}

bool LogWidget::render()
{
    AppStyle::pushTitle();
    ImGui::SetNextWindowSizeConstraints(ImVec2(300, 200), ImVec2(800, 900));
    ImGui::Begin("Log", (bool*)0);
    AppStyle::pushInput();

    ImVec2 size = ImGui::GetContentRegionAvail();

    static vector<string>::iterator it;
    _chatLog.getCommandLog([&](vector<string>& commands) {
        
        it = commands.begin();
        ImGui::BeginChild("Log text", ImVec2(size.x, size.y));
        string message;
        for (; it != commands.end(); ++it)
        {
            if ((*it)[0] == '@')
            {
                AppStyle::pushPositive();
                ImGui::TextWrapped(Stringer::stripFirst(*it).c_str());
                AppStyle::pop();
            }
            else if ((*it)[0] == '!')
            {
                AppStyle::pushNegative();
                ImGui::TextWrapped(Stringer::stripFirst(*it).c_str());
                AppStyle::pop();
            }
            else
            {
                ImGui::TextWrapped(Stringer::stripFirst(*it).c_str());
            }
        }
        if (_messageCount != commands.size())
        {
            if (ImGui::GetScrollY() >= ImGui::GetScrollMaxY() - 10)
            {
                ImGui::SetScrollHereY(1.0f);
            }
            _messageCount = commands.size();
        }
        ImGui::EndChild();

    });

    AppStyle::pop();
    ImGui::End();
    AppStyle::pop();

    return true;
}
