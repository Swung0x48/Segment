#include "CommandSeg.h"


CommandSeg::CommandSeg(Segment* segment): segment_(segment)
{}

void CommandSeg::Execute(IBML* bml, const std::vector<std::string>& args) {
	if (args.size() > 1) {
		if (args[1] == "clear")
		{
			if (bml->IsIngame()) {
				segment_->ClearRecord();
				bml->SendIngameMessage("Record for this level has been cleared.");
			}
		}
	}
}
