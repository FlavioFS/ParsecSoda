#include "InfoWidget.h"

bool InfoWidget::render()
{
    AppStyle::pushTitle();

    static ImVec2 res;
    static ImVec2 size = ImVec2(600, 300);

    res = ImGui::GetMainViewport()->Size;

    ImGui::SetNextWindowPos(ImVec2(
        (res.x - size.x) * 0.5f,
        (res.y - size.y) * 0.5f
    ));
    ImGui::SetNextWindowSize(size);
    ImGui::Begin("About###About Window", (bool*)0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse);
    AppStyle::pushInput();
    
    ImGui::Dummy(ImVec2(0, 20));

    ImGui::TextWrapped("Parsec Soda is FREE and Open Source. If you paid for this app, you have been scammed. You can find the source code at:");
    
    renderSugoiButton("github.com/FlavioFS/ParsecSoda", L"https://github.com/FlavioFS/ParsecSoda");

    ImGui::Dummy(ImVec2(0, 15));

    ImGui::TextWrapped("Parsec Soda was created by @flafdraws. Find me on social media:");

    renderSugoiButton("linktr.ee/flafdraws", L"https://linktr.ee/flafdraws");

    AppStyle::pop();
    ImGui::End();
    AppStyle::pop();

    return true;
}

bool InfoWidget::renderSugoiButton(const char* text, const wchar_t* link)
{
    bool result = false;

    AppColors::pushPrimary();
    AppFonts::pushSugoiDekai();
    if (ImGui::Button(text))
    {
        ShellExecute(0, 0, link, 0, 0, SW_SHOW);
        result = true;
    }
    AppFonts::pop();
    AppColors::pop();

    return result;
}
