#pragma once
#include <BML/BMLAll.h>
class CommandSeg: public ICommand
{
public:
	CommandSeg(double segmentTime[][9], int* currentLevel);
	virtual std::string GetName() override { return "segment"; };
	virtual std::string GetAlias() override { return "seg"; };
	virtual std::string GetDescription() override { return "Command for segment."; };
	virtual bool IsCheat() override { return false; };

	virtual void Execute(IBML* bml, const std::vector<std::string>& args) override;
	virtual const std::vector<std::string> GetTabCompletion(IBML* bml, const std::vector<std::string>& args) override {
		return args.size() == 2 ? std::vector<std::string>{ "clear" } : std::vector<std::string>{};
	};
private:
	double (*_segmentTime)[9];
	int* _currentLevel;
};

