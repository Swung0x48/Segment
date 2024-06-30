#include "SegmentGui.h"
#include <string>
#include <format>

void SegmentGui::update()
{
	ImGui::Begin("Segments", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing);
	{
		ImGui::Text(current_level_name_.c_str());

		ImGui::BeginTable("##Segments", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg);
		{
			for (size_t i = 0; i < state_.size(); ++i) {
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::Text("#%d", i + 1);
				ImGui::TableSetColumnIndex(1);
				const auto time = state_.segment(i);
				ImGui::Text("%.3fs", time);
				ImGui::TableSetColumnIndex(2);
				const auto time_to_compare = state_.segment_to_compare(i);
				if (time_to_compare < 0.f)
					ImGui::Text("----");
				else
					ImGui::Text("%.3fs", time_to_compare);
				ImGui::TableSetColumnIndex(3);
				if (time_to_compare < 0.f)
					ImGui::Text("----");
				else
					ImGui::Text("%+.3fs", time - time_to_compare);

				if (cursor_visible_ && i == state_.get_current_segment()) {
					if (time_to_compare < 0 || std::abs(time - time_to_compare) < 1e-7)
						ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, even_color);
					else if (time < time_to_compare)
						ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, lead_color);
					else
						ImGui::TableSetBgColor(ImGuiTableBgTarget_RowBg0, lag_color);
				}
			}
		}
		ImGui::EndTable();
	}
	ImGui::End();
}
