#include "menu.h"

void Menu::DrawMethodInspector()
{
    Helper::DrawMethodInspector();
}

void Menu::Draw()
{
    ImGui::SetNextWindowSize(
        ImVec2(760, 500),
        ImGuiCond_FirstUseEver
    );

    ImGui::Begin(
        "Mono Runtime Explorer",
        nullptr,
        ImGuiWindowFlags_NoCollapse |
        ImGuiWindowFlags_NoScrollbar |
        ImGuiWindowFlags_NoScrollWithMouse
    );

    ImVec2 windowSize =
        ImGui::GetContentRegionAvail();

    Helper::DrawSidebar(windowSize);

    ImGui::SameLine();

    ImGui::BeginChild(
        "##MainContent",
        ImVec2(0, windowSize.y),
        false
    );

    ImGui::Dummy(ImVec2(0, 12));

    Helper::DrawCurrentTab();

    ImGui::EndChild();
    ImGui::End();

    DrawMethodInspector();
}
