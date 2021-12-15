#include "InfoWidget.h"

bool InfoWidget::render()
{
    AppStyle::pushTitle();

    static ImVec2 res;
    static ImVec2 size = ImVec2(875, 700);

    res = ImGui::GetMainViewport()->Size;

    ImGui::SetNextWindowPos(ImVec2(
        (res.x - size.x) * 0.5f,
        (res.y - size.y) * 0.5f
    ));
    ImGui::SetNextWindowSize(size);
    ImGui::Begin("About###About Window", (bool*)0, ImGuiWindowFlags_NoResize | ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoCollapse);
    AppStyle::pushInput();
    
    ImGui::Dummy(ImVec2(0, 10));

    renderSugoiButton("github.com/FlavioFS/ParsecSoda", L"https://github.com/FlavioFS/ParsecSoda");
    ImGui::SetCursorPosX(25);
    ImGui::TextWrapped("Parsec Soda is FREE and Open Source. If you paid for this app, you have been scammed. You can find the source code at this address.");

    ImGui::Dummy(ImVec2(0, 15));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0, 15));

    renderSugoiButton("linktr.ee/flafcodes", L"https://linktr.ee/flafcodes");
    ImGui::SetCursorPosX(25);
    ImGui::TextWrapped("Parsec Soda was created by FlaF. You can find all my social media links there.");

    ImGui::Dummy(ImVec2(0, 15));
    ImGui::Separator();
    ImGui::Dummy(ImVec2(0, 15));

    ImGui::BeginGroup();
    renderSugoiButton("Donate", L"https://github.com/FlavioFS/ParsecSoda#donate");
    ImGui::SetCursorPosX(25);
    ImGui::TextWrapped("Want to support my work?");

    ImGui::Dummy(ImVec2(0, 10));
    
    renderLinkButton("Paypal (click here)", L"https://www.paypal.com/donate?hosted_button_id=28PBV9DFYQC72");

    static char pix[] = "1194a8f0-ac1f-40f3-8f89-524697c072ea";
    AppFonts::pushInput();
    ImGui::SetCursorPosX(25);
    ImGui::Text("Pix (Brazil)");
    ImGui::SameLine();
    ImGui::SetCursorPosX(120);
    ImGui::SetNextItemWidth(250);
    ImGui::InputText("### Donate Pix", pix, 64, ImGuiInputTextFlags_ReadOnly);
    ImGui::SameLine();
    ImGui::Text("   (Flavio Freitas)");

    static char btc[] = "bc1qmvuljqen7tcs57z6pems5jam7dt0taswp6y5x9";
    ImGui::SetCursorPosX(25);
    ImGui::Text("BTC");
    ImGui::SameLine();
    ImGui::SetCursorPosX(120);
    ImGui::SetNextItemWidth(365);
    ImGui::InputText("### Donate BTC", btc, 64, ImGuiInputTextFlags_ReadOnly);

    static char ltc[] = "ltc1q349yxyzt09u7kvdz5xvkln0pappxhcwl4g4mt9";
    ImGui::SetCursorPosX(25);
    ImGui::Text("LTC");
    ImGui::SameLine();
    ImGui::SetCursorPosX(120);
    ImGui::SetNextItemWidth(365);
    ImGui::InputText("### Donate LTC", ltc, 64, ImGuiInputTextFlags_ReadOnly);

    static char eth[] = "0x4B0d629d60e7b910d03f3f15d0Db25146Be05642";
    ImGui::SetCursorPosX(25);
    ImGui::Text("ETH");
    ImGui::SameLine();
    ImGui::SetCursorPosX(120);
    ImGui::SetNextItemWidth(365);
    ImGui::InputText("### Donate ETH", eth, 64, ImGuiInputTextFlags_ReadOnly);
    AppFonts::pop();
    ImGui::EndGroup();

    ImGui::SameLine();
    ImGui::Dummy(ImVec2(20, 0));
    ImGui::SameLine();
    
    // QR
    ImGui::BeginGroup();
    ImGui::BeginGroup();
    ImGui::Text("BTC");
    ImGui::Image(AppIcons::btc, ImVec2(145, 145));
    ImGui::EndGroup();
    
    ImGui::SameLine();
    ImGui::Dummy(ImVec2(20, 0));
    ImGui::SameLine();
    
    ImGui::BeginGroup();
    ImGui::Text("LTC");
    ImGui::Image(AppIcons::ltc, ImVec2(145, 145));
    ImGui::EndGroup();
    
    ImGui::Dummy(ImVec2(0, 10));

    ImGui::BeginGroup();
    ImGui::Text("ETH");
    ImGui::Image(AppIcons::eth, ImVec2(145, 145));
    ImGui::EndGroup();
    ImGui::EndGroup();

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

bool InfoWidget::renderLinkButton(const char* text, const wchar_t* link)
{
    bool result = false;

    AppColors::pushPrimary();
    AppFonts::pushTitle();
    if (ImGui::Button(text))
    {
        ShellExecute(0, 0, link, 0, 0, SW_SHOW);
        result = true;
    }
    AppFonts::pop();
    AppColors::pop();

    return result;
}