#pragma once
#include <BML/BMLAll.h>
#include <imgui.h>
#include <vector>
#include <unordered_map>
#include <memory>
#include <sstream>
#include <fstream>
#include <filesystem>

#include "PerLevelSegmentState.h"
#include "SegmentGui.h"

#include <picojson.h>

#define m_bml m_BML

constexpr int SEG_MAJOR_VER = 2;
constexpr int SEG_MINOR_VER = 0;
constexpr int SEG_PATCH_VER = 1;
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
	const static inline std::string RECORD_SAVE_PATH = "../ModLoader/Configs/SegmentRecords.json";

	struct session {
		session(const int current_level, const int sector_count):
			state(sector_count), gui(state, current_level) {}

		session(const std::string_view current_level_name, const int sector_count) :
			state(sector_count), gui(state, current_level_name) {}

		PerLevelSegmentState state;
		SegmentGui gui;
	};

	std::unordered_map<std::string, std::shared_ptr<session>> sessions_;
	std::shared_ptr<session> session_;

	enum class serialize_from_t {
		Current,
		Target
	};

	picojson::value serialize_sessions_to_pico(serialize_from_t serialize_from = serialize_from_t::Target) const {
		picojson::array records;
		for (const auto& [path, session] : sessions_) {
			picojson::object map_obj;

			picojson::object level_obj;
			picojson::array arr;
			const auto& state = session->state;
			switch (serialize_from) {
			case serialize_from_t::Target:
				for (size_t i = 0; i < state.size(); ++i) {
					arr.emplace_back(state.segment_target(i));
				}
				break;
			case serialize_from_t::Current:
				for (size_t i = 0; i < state.size(); ++i) {
					arr.emplace_back(state.segment(i));
				}
				break;
			default:
				assert(false);
			}
			
			level_obj.emplace("name", session->gui.current_level_name_);
			level_obj.emplace("segments", arr);
			map_obj.emplace(path, level_obj);

			records.emplace_back(map_obj);
		}
		picojson::value v(records);
		return v;
	}

	void save_pico_to_file(const picojson::value& v) const {
		//std::string path = (std::filesystem::current_path() / RECORD_SAVE_PATH).lexically_normal().string();
		std::ofstream fs(RECORD_SAVE_PATH, std::ios::trunc);
		std::string str = v.serialize();
		fs << str;
		fs.close();
	}

	bool load_sessions_from_file() {
		picojson::value v;
		std::ifstream fs(RECORD_SAVE_PATH);
		fs >> v;
		std::string err = picojson::get_last_error();
		if (!err.empty()) {
			GetLogger()->Warn("Error loading sessions from file.");
			GetLogger()->Warn(err.c_str());

			sessions_.clear();
			return false;
		}

		if (!v.is<picojson::array>()) {
			GetLogger()->Error("Outermost array in illegal form.");

			sessions_.clear();
			return false;
		}
		picojson::array records = v.get<picojson::array>();

		for (const auto& record : records) {
			if (!record.is<picojson::object>()) {
				GetLogger()->Error("Record object in illegal form.");

				sessions_.clear();
				return false;
			}
			picojson::object map_obj = record.get<picojson::object>();

			for (picojson::value::object::const_iterator i = map_obj.begin();
				i != map_obj.end();
				++i) {
				if (!i->second.is<picojson::object>()) {
					GetLogger()->Error("Level object in illegal form.");

					sessions_.clear();
					return false;
				}
				picojson::object obj = i->second.get<picojson::object>();

				if (!obj.contains("name") || !obj["name"].is<std::string>()) {
					GetLogger()->Error("Level object does not have \'name\' field.");

					sessions_.clear();
					return false;
				}
				std::string name = obj["name"].get<std::string>();

				if (!obj.contains("segments") || !obj["segments"].is<picojson::array>()) {
					GetLogger()->Error("Level object does not have \'segments\' field.");

					sessions_.clear();
					return false;
				}
				const auto& arr = obj["segments"].get<picojson::array>();

				sessions_[i->first] = std::make_shared<session>(name, arr.size());
				auto& state = sessions_[i->first]->state;
				for (size_t i = 0; i < state.size(); ++i) {
					state.segment_target(i) = arr[i].get<double>();
				}
			}
		}

		return true;
	}

	bool is_custom_map(const std::string_view filename)
	{
		return filename.substr(0, 11) != "3D Entities";
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

	std::vector<std::function<void()>> queued_tasks_;
};

