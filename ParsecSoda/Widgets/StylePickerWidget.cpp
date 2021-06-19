#include "StylePickerWidget.h"

void updateSource(char * source)
{
    try
    {
        sprintf_s(
            source, 1024,
            (
                string("positive = ImVec4(%1.2ff, %1.2ff, %1.2ff, %1.2ff);\n") +
                string("negative = ImVec4(%1.2ff, %1.2ff, %1.2ff, %1.2ff);\n") +
                string("warning  = ImVec4(%1.2ff, %1.2ff, %1.2ff, %1.2ff);")
            ).c_str(),
            AppColors::positive.x, AppColors::positive.y, AppColors::positive.z, AppColors::positive.w,
            AppColors::negative.x, AppColors::negative.y, AppColors::negative.z, AppColors::negative.w,
            AppColors::warning.x, AppColors::warning.y, AppColors::warning.z, AppColors::warning.w
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

    static float positive[4] = { AppColors::positive.x, AppColors::positive.y, AppColors::positive.z, AppColors::positive.w };
    static float refpositive[4] = { AppColors::positive.x, AppColors::positive.y, AppColors::positive.z, AppColors::positive.w };
    static float negative[4] = { AppColors::negative.x, AppColors::negative.y, AppColors::negative.z, AppColors::negative.w };
    static float refnegative[4] = { AppColors::negative.x, AppColors::negative.y, AppColors::negative.z, AppColors::negative.w };
    static float warning[4] = { AppColors::warning.x, AppColors::warning.y, AppColors::warning.z, AppColors::warning.w };
    static float refwarning[4] = { AppColors::warning.x, AppColors::warning.y, AppColors::warning.z, AppColors::warning.w };
    
    static bool showPositive = false, showNegative = false, showWarning = false;

    if (ImGui::ColorButton("Positive button", AppColors::positive))
        showPositive = !showPositive;
    if (ImGui::ColorButton("Negative button", AppColors::negative))
        showNegative = !showNegative;
    if (ImGui::ColorButton("Warning button", AppColors::warning))
        showWarning = !showWarning;

    if (showPositive && ImGui::ColorPicker4("PickPositive", positive, 0, refpositive))
    {
        AppColors::positive = ImVec4(positive[0], positive[1], positive[2], positive[3]);
        updateSource(source);
    }
    if (showNegative && ImGui::ColorPicker4("PickNegative", negative, 0, refnegative))
    {
        AppColors::negative = ImVec4(negative[0], negative[1], negative[2], negative[3]);
        updateSource(source);
    }
    if (showWarning && ImGui::ColorPicker4("PickWarning", warning, 0, refwarning))
    {
        AppColors::warning = ImVec4(warning[0], warning[1], warning[2], warning[3]);
        updateSource(source);
    }
    
    ImGui::InputTextMultiline("  ", source, 1024, ImVec2(size.x, 200));

    AppStyle::pop();
    ImGui::End();
    AppStyle::pop();

    return false;
}
