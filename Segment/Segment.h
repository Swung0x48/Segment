#pragma once
#include <BML/BMLAll.h>
#include <imgui.h>
#include <vector>
#include <unordered_map>
#include <memory>
#include <sstream>

#include "PerLevelSegmentState.h"
#include "SegmentGui.h"

#define m_bml m_BML

constexpr int SEG_MAJOR_VER = 2;
constexpr int SEG_MINOR_VER = 0;
constexpr int SEG_PATCH_VER = 0;
extern "C" {
	__declspec(dllexport) IMod* BMLEntry(IBML* bml);
}

class Segment: public IMod
{
public:
	typedef const char* CKSTRING;
	typedef CKBOOL BOOL;

	Segment(IBML* bml);
	virtual CKSTRING GetID() override { return "Segment"; }
	virtual CKSTRING GetVersion() override {
		return SEG_VERSION.c_str(); 
	}
	virtual CKSTRING GetName() override { return "Segment"; }
	virtual CKSTRING GetAuthor() override { return "Swung0x48"; }
	virtual CKSTRING GetDescription() override { return "A mod to display your gameplay performance split into each segment."; }
	DECLARE_BML_VERSION;

	virtual void OnLoad() override;
	virtual void OnModifyConfig(CKSTRING category, CKSTRING key, IProperty* prop) override;
	virtual void OnPreStartMenu() override;
	virtual void OnPreEndLevel() override;
	virtual void OnCounterActive() override;
	virtual void OnCounterInactive() override;
	virtual void OnPauseLevel() override;
	virtual void OnUnpauseLevel() override;
	virtual void OnProcess() override;
	virtual void OnStartLevel() override;
	virtual void OnPostCheckpointReached() override;
	virtual void OnLoadObject(CKSTRING filename, BOOL isMap, CKSTRING masterName, CK_CLASSID filterClass,
		BOOL addtoscene, BOOL reuseMeshes, BOOL reuseMaterials, BOOL dynamic,
		XObjectArray* objArray, CKObject* masterObj);
	virtual void OnPreExitLevel() override;
	virtual void OnCheatEnabled(bool enable) override;
	virtual void OnGameOver() override;
private:
	const std::string SEG_VERSION = std::format("{}.{}.{}", SEG_MAJOR_VER, SEG_MINOR_VER, SEG_PATCH_VER);
	std::unique_ptr<PerLevelSegmentState> state_;
	std::unique_ptr<SegmentGui> gui_;

	bool is_custom_map(const std::string_view filename)
	{
		return filename.substr(0, 18) == R"(..\ModLoader\Maps\)";
	}

	const int get_current_level() {
		int ret = 0;
		m_bml->GetArrayByName("CurrentLevel")->GetElementValue(0, 0, &ret);
		return ret;
	}

	const int get_current_sector() {
		int next_sector;
		CKDataArray* ingameparameter_array = m_bml->GetArrayByName("IngameParameter");
		ingameparameter_array->GetElementValue(0, 1, &next_sector);
		return next_sector - 1;
	}

	// TODO: more elegant way to retrieve sector groups
	const int get_sector_count() {
		std::string name;
		
		int sector_count = 0;
		for (int i = 1; i <= 9; i++) {
			if (i == 9)
				name = "Sector_9";
			else
				name = std::format("Sector_{:02d}", i);
			
			if (m_bml->GetGroupByName(name.c_str()) == nullptr)
				break;

			sector_count = i;
		}
		return sector_count;
	}

	std::vector<double> split(const std::string& s, const char delim = ' ') {
		std::vector<double> vec;
		std::istringstream iss(s);
		std::string temp;

		while (getline(iss, temp, delim)) {
			vec.push_back(stof(temp));
		}
		return vec;
	}

};

