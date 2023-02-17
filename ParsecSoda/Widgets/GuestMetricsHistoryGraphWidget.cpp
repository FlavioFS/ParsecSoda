#include "GuestMetricsHistoryGraphWidget.h"

void GuestMetricsHistoryGraphWidget::render(GuestData guest, CircularMetrics& metricsHistory)
{
    static const ImVec4 latencyColor = AppColors::primary;
    static const ImVec4 bitrateColor = AppColors::primary;
    static const ImVec4 fastRTColor = AppColors::warning;
    static const ImVec4 slowRTColor = AppColors::negative;
    static const ImVec4 queuedFramesColor = AppColors::primary;
    static const ImVec4 encodeColor = ImVec4(0.47f, 0.80f, 0.00f, 1.00f);
    static const ImVec4 decodeColor = ImVec4(0.80f, 0.00f, 0.47f, 1.00f);

    static const int xAxisFlags = ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoDecorations;
    static const int yAxisFlags = ImPlotAxisFlags_Lock | ImPlotAxisFlags_NoGridLines | ImPlotAxisFlags_AutoFit;

    static const float margin = 1.0f;
    static const ImVec2 graphSize(300, 200);

    size_t MAX_SIZE;
    size_t HALF_SIZE;
    char tag[32];
    double currentValue;

    const auto setupAxes = [&](double currentValue) {
        ImPlot::SetupAxes(NULL, NULL, xAxisFlags, yAxisFlags);
        ImPlot::SetupAxisLimits(ImAxis_X1, 0, MAX_SIZE, ImGuiCond_Always);
        ImPlot::SetupAxisLimits(ImAxis_Y1, 0.5f*currentValue - margin, 1.5*currentValue + margin, ImGuiCond_Always);
        ImPlot::SetupLegend(ImPlotLocation_South, ImPlotLegendFlags_Outside);
    };

    const auto plotFloatStat = [&](const char* titleID, const vector<float>& values, ImVec4 color, bool mustSetupAxis = false, const char* tagNumberFormat = "%4.1f") {
        currentValue = values[metricsHistory.newestIndex()];
        snprintf(tag, 32, tagNumberFormat, (float)currentValue);
        if (mustSetupAxis) setupAxes(currentValue);
        const string label = string(titleID) + ": " + tag;
        ImPlot::PushStyleColor(ImPlotCol_Line, color);
        ImPlot::PlotLine(label.c_str(), &values[0], (int)metricsHistory.size(), 1.0, 0.0, ImPlotLegendFlags_NoMenus, (double)metricsHistory.offset(), sizeof(float));
        ImPlot::PopStyleColor();
    };

    const auto plotIntStat = [&](const char* titleID, const vector<uint32_t>& values, ImVec4 color, bool mustSetupAxis = false, const char* tagNumberFormat = "%3d") {
        currentValue = values[metricsHistory.newestIndex()];
        snprintf(tag, 32, tagNumberFormat, (uint32_t)currentValue);
        if (mustSetupAxis) setupAxes(currentValue);
        const string label = string(titleID) + ": " + tag;
        ImPlot::PushStyleColor(ImPlotCol_Line, color);
        ImPlot::PlotLine(label.c_str(), &values[0], (int)metricsHistory.size(), 1.0, 0.0, ImPlotLegendFlags_NoMenus, (double)metricsHistory.offset(), sizeof(uint32_t));
        ImPlot::PopStyleColor();
    };

    MAX_SIZE = metricsHistory.size();
    HALF_SIZE = MAX_SIZE >> 1;

    AppStyle::pushLabel();
    AppColors::pushLabel();

    // Latency
    if (ImPlot::BeginPlot("### latency", graphSize, ImPlotFlags_NoMouseText))
    {
        metricsHistory.getLatencies([&](const vector<float>& values) { plotFloatStat("Latency", values, latencyColor, true, "%4.0f ms"); });
        ImPlot::EndPlot();
    }

    ImGui::SameLine();

    // Bitrate
    if (ImPlot::BeginPlot("### bitrate", graphSize, ImPlotFlags_NoMouseText))
    {
        metricsHistory.getBitrates([&](const vector<float>& values) { plotFloatStat("Bitrate", values, bitrateColor, true, "%4.1f mbps"); });
        ImPlot::EndPlot();
    }


    // RTs
    if (ImPlot::BeginPlot("### RT/Q", graphSize, ImPlotFlags_NoMouseText))
    {
        metricsHistory.getFastRTs([&](const vector<uint32_t>& values) { plotIntStat("Fast RTs", values, fastRTColor, true); });
        metricsHistory.getSlowRTs([&](const vector<uint32_t>& values) { plotIntStat("Slow RTs", values, slowRTColor); });
        metricsHistory.getQueuedFrames([&](const vector<uint32_t>& values) { plotIntStat("Queued Frames", values, queuedFramesColor); });
        ImPlot::EndPlot();
    }

    ImGui::SameLine();

    // Encode + Decode
    if (ImPlot::BeginPlot("### encode/decode", graphSize, ImPlotFlags_NoMouseText))
    {
        metricsHistory.getEncodes([&](const vector<float>& values) { plotFloatStat("Encode", values, encodeColor, true, "%4.1f ms"); });
        metricsHistory.getDecodes([&](const vector<float>& values) { plotFloatStat("Decode", values, decodeColor, false, "%4.1f ms"); });
        ImPlot::EndPlot();
    }

    AppColors::pop();
    AppStyle::pop();
}
