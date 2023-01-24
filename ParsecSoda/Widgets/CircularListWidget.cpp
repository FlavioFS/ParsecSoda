#include "CircularListWidget.h"

template <typename T>
static void CircularListWidget::render(const char* titleID, const char* labelID, CircularList<T>& circularList, float padding, const char* tagNumberFormat)
{
    static const int yAxisFlags = ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_AutoFit;
    static const int xAxisFlags = ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoDecorations;

    static char tag[32];
    static double currentY;
    static size_t MAX_SIZE;
    static size_t HALF_SIZE;

    if (ImPlot::BeginPlot(titleID, ImVec2(-1, 0), ImPlotFlags_NoMouseText))
    {

        circularList.getList([&](std::vector<float>& items) {
            
            MAX_SIZE = circularList.capacity();
            HALF_SIZE = MAX_SIZE >> 1;
            currentY = items[circularList.lastIndex()];
            snprintf(tag, 32, tagNumberFormat, currentY);

            ImPlot::SetupAxes(NULL, NULL, xAxisFlags, yAxisFlags);
            ImPlot::SetupAxisLimits(ImAxis_X1, 0, MAX_SIZE, ImGuiCond_Always);
            //ImPlot::SetupAxisLimits(ImAxis_Y1, circularList.minimum() - padding, circularList.maximum() + padding);
            ImPlot::PlotLine(labelID, &items[0], (int)circularList.size(), 1.0, 0.0, ImPlotLegendFlags_NoMenus, (double)circularList.offset(), sizeof(float));

            ImPlot::DragLineY(0, &currentY, AppColors::positive, 1.0f, ImPlotDragToolFlags_NoInputs);
            ImPlot::Annotation(HALF_SIZE, currentY, AppColors::positive, ImVec2(), true, tag);
        });

        ImPlot::EndPlot();
    }
}


//void backup()
//{
//    const static float M_PI = 3.1415f;
//    const static size_t MAX_SIZE = 360;
//    const static size_t HALF_SIZE = MAX_SIZE >> 1;
//    const static float Y_SCALE = 50;
//    const static float PADDING = 10;
//    static CircularList<float> cl(MAX_SIZE);
//    static ImVec2 point{ 0, 1 };
//    const static float CYCLE_LEN = 2.0f * M_PI;
//    point.x++;
//    point.y = Y_SCALE * sinf(point.x * M_PI / 180.0f);
//    cl.push(point.y);
//
//    if (ImPlot::BeginPlot("Alpha Plot", ImVec2(-1, 0), ImPlotFlags_NoMouseText))
//    {
//        static const int yAxisFlags = ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoGridLines;
//        static const int xAxisFlags = ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoDecorations;
//        static char tag[10];
//        static double currentY = 0;
//
//        ImPlot::SetupAxes(NULL, NULL, xAxisFlags, yAxisFlags);
//
//        cl.getList([&](std::vector<float>& items) {
//            
//            currentY = items[cl.lastIndex()];
//            snprintf(tag, 10, "%4.0f", currentY);
//
//            ImPlot::SetupAxisLimits(ImAxis_X1, 0, cl.capacity(), ImGuiCond_Always);
//            ImPlot::SetupAxisLimits(ImAxis_Y1, cl.minimum() - PADDING, cl.maximum() + PADDING);
//            ImPlot::PlotLine("Alpha", &items[0], (int)cl.size(), 1.0, 0.0, ImPlotLegendFlags_None, (double)cl.offset(), sizeof(float));
//
//            ImPlot::DragLineY(0, &currentY, AppColors::positive, 1.0f, ImPlotDragToolFlags_NoInputs);
//            ImPlot::Annotation(HALF_SIZE, currentY, AppColors::positive, ImVec2(), true, tag);
//        });
//
//
//
//        ImPlot::EndPlot();
//    }
//}