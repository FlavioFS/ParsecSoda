#include "StylePickerWidget.h"

void updateSource(char * source)
{
    try
    {
        sprintf_s(
            source, 1024,
            "alert = ImVec4(%1.2ff, %1.2ff, %1.2ff, %1.2ff);\nerror = ImVec4(%1.2ff, %1.2ff, %1.2ff,%1.2ff);",
            AppColors::alert.x, AppColors::alert.y, AppColors::alert.z, AppColors::alert.w,
            AppColors::error.x, AppColors::error.y, AppColors::error.z, AppColors::error.w
        );
    }
    catch (const std::exception&) {}
}

bool StylePickerWidget::render()
{
    AppStyle::pushTitle();
    ImGui::SetNextWindowSizeConstraints(ImVec2(400, 250), ImVec2(500, 900));
    ImGui::Begin("Style picker", (bool*)0);
    AppStyle::pushInput();

    static ImVec2 size;
    size = ImGui::GetContentRegionAvail();
    static char source[1024] = "\0";

    static float alert[4] = { AppColors::alert.x, AppColors::alert.y, AppColors::alert.z, AppColors::alert.w };
    static float err[4] = { AppColors::error.x, AppColors::error.y, AppColors::error.z, AppColors::error.w };
    static float refalert[4] = { AppColors::alert.x, AppColors::alert.y, AppColors::alert.z, AppColors::alert.w };
    static float referr[4] = { AppColors::error.x, AppColors::error.y, AppColors::error.z, AppColors::error.w };
    
    static bool showAlert = false, showError = false;

    if (ImGui::ColorButton("Alert button", AppColors::alert))
        showAlert = !showAlert;
    if (ImGui::ColorButton("Error button", AppColors::error))
        showError = !showError;


    if (showAlert && ImGui::ColorPicker4("PickAlert", alert, 0, refalert))
    {
        AppColors::alert = ImVec4(alert[0], alert[1], alert[2], alert[3]);
        updateSource(source);
    }
    if (showError && ImGui::ColorPicker4("PickError", err, 0, referr))
    {
        AppColors::error = ImVec4(err[0], err[1], err[2], err[3]);
        updateSource(source);
    }
    
    ImGui::InputTextMultiline("  ", source, 1024, ImVec2(size.x, 200));

    AppStyle::pop();
    ImGui::End();
    AppStyle::pop();

    return false;
}
