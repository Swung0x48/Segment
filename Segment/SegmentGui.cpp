#include "SegmentGui.h"
#include <string>
#include <format>

void SegmentGui::update()
{
	/*ImGuiStyle style;
	ImGui::StyleColorsLight(&style);
	ImVec4 col = style.Colors[ImGuiCol_WindowBg];
	col.w = 0.5f;
	ImGui::ShowDemoWindow();*/
	ImGui::PushStyleColor(ImGuiCol_WindowBg, bg_color);

	ImGui::Begin("Segments", nullptr, ImGuiWindowFlags_AlwaysAutoResize | ImGuiWindowFlags_NoFocusOnAppearing);
	{
		ImGui::Text(current_level_name_.c_str());

		if (ImGui::BeginTable("##Segments", 4, ImGuiTableFlags_Borders | ImGuiTableFlags_RowBg))
		{
			ImGui::TableSetupColumn("#Segment", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("Current", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("Target", ImGuiTableColumnFlags_WidthStretch);
			ImGui::TableSetupColumn("Delta", ImGuiTableColumnFlags_WidthStretch);

			ImGui::TableHeadersRow();
			for (size_t i = 0; i < state_.size(); ++i) {
				ImGui::TableNextRow();

				ImGui::TableSetColumnIndex(0);
				ImGui::Text("#%d", i + 1);
				ImGui::TableSetColumnIndex(1);
				const auto time = state_.segment(i);
				ImGui::Text("%.3fs", time);
				ImGui::TableSetColumnIndex(2);
				auto& time_to_compare = state_.segment_target(i);
				ImGui::PushItemWidth(-1);
				ImGui::DragFloat(std::format("##seg{}", i).c_str(), &time_to_compare, 
					0.1f, 0.0f, 1e6f, 
					(time_to_compare >= 0.f) ? "%.3fs" : "----");
				/*if (time_to_compare < 0.f)
					ImGui::Text("----");
				else
					ImGui::Text("%.3fs", time_to_compare);*/
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

			ImGui::EndTable();
		}
		
		if (ImGui::TreeNode("History Settings")) {
			if (ImGui::Button("Clear History"))
				state_.clear_history();
			ImGui::Checkbox("Update History", &state_.is_saving_);
			ImGui::TreePop();
		}
	}
	ImGui::End();
	ImGui::PopStyleColor();
}
