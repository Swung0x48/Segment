#pragma once
#include "PerLevelSegmentState.h"

#include <imgui.h>

class SegmentGui {
public:
	SegmentGui(PerLevelSegmentState& state, const int level) :
		state_(state), current_level_(level) {}

	void update();
	void set_cursor_visible(bool visible) { cursor_visible_ = visible; }

	PerLevelSegmentState& state_;
	const int current_level_ = 0;
	bool cursor_visible_ = true;

	const static inline ImU32 lead_color = ImGui::GetColorU32(ImVec4(0.2f, 0.8f, 0.2f, 0.75f));
	const static inline ImU32 even_color = ImGui::GetColorU32(ImVec4(1.f, 0.66f, 0.f, 0.75f));
	const static inline ImU32 lag_color = ImGui::GetColorU32(ImVec4(0.85f, 0.08f, 0.25f, 0.75f));
};
