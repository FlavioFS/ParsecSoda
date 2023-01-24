#include "GuestMetricsHistoryGraphWidget.h"

void GuestMetricsHistoryGraphWidget::render(GuestData guest, CircularList<GuestMetrics>& metricsHistory)
{
    static const int yAxisFlags = ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_AutoFit;
    static const int xAxisFlags = ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoDecorations;

    static const float padding = 10.0f;

    static char tag[32];
    static double currentY;
    static size_t MAX_SIZE;
    static size_t HALF_SIZE;

    if (ImPlot::BeginPlot(guest.name.c_str(), ImVec2(-1, 0), ImPlotFlags_NoMouseText))
    {

        metricsHistory.getList([&](std::vector<GuestMetrics>& items) {
            
            MAX_SIZE = metricsHistory.capacity();
            HALF_SIZE = MAX_SIZE >> 1;
            
            ImPlot::SetupAxes(NULL, NULL, xAxisFlags, yAxisFlags);
            ImPlot::SetupAxisLimits(ImAxis_X1, 0, MAX_SIZE, ImGuiCond_Always);

            const static auto plotStat = [&](const char* titleID, double value, const char* tagNumberFormat = "%4.0f") {
                currentY = value;
                snprintf(tag, 32, tagNumberFormat, currentY);
                //ImPlot::SetupAxisLimits(ImAxis_Y1, metricsHistory.minimum() - padding, metricsHistory.maximum() + padding);
                ImPlot::PlotLine(titleID, &items[0], (int)metricsHistory.size(), 1.0, 0.0, ImPlotLegendFlags_NoMenus, (double)metricsHistory.offset(), sizeof(GuestMetrics));
                ImPlot::DragLineY(0, &currentY, AppColors::positive, 1.0f, ImPlotDragToolFlags_NoInputs);
                ImPlot::Annotation(HALF_SIZE, currentY, AppColors::positive, ImVec2(), true, tag);
            };

            
            plotStat("Latency (ms)", items[metricsHistory.lastIndex()].getMetrics().networkLatency);
            plotStat("Bitrate (mbps)", items[metricsHistory.lastIndex()].getMetrics().bitrate);
        });

        ImPlot::EndPlot();
    }
}
