#pragma once
#include "PerLevelSegmentState.h"

class SegmentGui {
public:
	SegmentGui(PerLevelSegmentState& state, const int level) :
		state_(state), current_level_(level) {}

	void update();

	PerLevelSegmentState& state_;
	const int current_level_ = 0;
};
