#include "SegmentGui.h"
#include <string>
#include <format>
#include <imgui.h>

void SegmentGui::update()
{
	ImGui::Begin("Segment", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing);
	{
		ImGui::Text("Level %d", current_level_);

		int segment = 0;

		const ImU32 row_bg_color = ImGui::GetColorU32(ImVec4(1.f, 0.66f, 0.f, 0.75f));
		ImGui::BeginTable("##Segments", 2, ImGuiTableFlags_BordersV | ImGuiTableFlags_RowBg);
		{
			for (const auto& time : state_) {
				ImGui::TableNextRow();

				if (segment == state_.get_current_segment())
					ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, row_bg_color);

				ImGui::TableSetColumnIndex(0);
				ImGui::Text("#%d", segment + 1);
				ImGui::TableSetColumnIndex(1);
				ImGui::Text("%.3fs", time);

				++segment;
			}
		}
		ImGui::EndTable();
	}
	ImGui::End();
}
