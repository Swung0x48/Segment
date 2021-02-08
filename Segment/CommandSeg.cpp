#include "CommandSeg.h"

CommandSeg::CommandSeg(double segmentTime[][9], int* currentLevel): 
	_segmentTime(segmentTime),
	_currentLevel(currentLevel)
{
}

void CommandSeg::Execute(IBML* bml, const std::vector<std::string>& args) {
	if (args.size() > 1) {
		if (args[1] == "clear")
		{
			for (int i = 0; i < 9; i++) {
				_segmentTime[*(_currentLevel) - 1][i] = -1;
			}
			bml->SendIngameMessage("Record for this level has been cleared.");
		}
	}
}
