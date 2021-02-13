#pragma once
#include <BML/BMLAll.h>
#include <sstream>
constexpr int SEG_MAJOR_VER = 1;
constexpr int SEG_MINOR_VER = 1;
constexpr int SEG_PATCH_VER = 4;

extern "C" {
	__declspec(dllexport) IMod* BMLEntry(IBML* bml);
}

class Segment: public IMod
{
private:
	static constexpr float TITLE_X_POS = 0.0f; // Title position (Segments)
	static constexpr float TITLE_Y_POS = 0.05f;
	static constexpr float PANEL_INIT_Y_POS = 0.085f; // Panel position (highlight background)

	static constexpr float TITLE_Y_SHIFT = 0.01f; // Between title & first row
	static constexpr float ITEM_Y_SHIFT = 0.03f; // Between rows
	static constexpr float TITLE_X_SHIFT = 0.02f; // Between heading (#n) & first col
	static constexpr float ITEM_X_SHIFT = 0.14f; // Between cols
	static constexpr float PANEL_Y_SHIFT = 0.03f; // Panel movement
	static constexpr float PANEL_WIDTH = 0.35f;
	static constexpr float PANEL_HEIGHT = 0.03f;
	float PANEL_INIT_HEIGHT = 0.0353f;

	static constexpr int BUF_SIZE = 50;

	char SEG_VERSION[20];

	int LEAD_R = 50;
	int LEAD_G = 205;
	int LEAD_B = 50;
	int LEAD_A = 200;
	int EVEN_R = 255;
	int EVEN_G = 168;
	int EVEN_B = 0;
	int EVEN_A = 200;
	int LAG_R = 220;
	int LAG_G = 20;
	int LAG_B = 60;
	int LAG_A = 200;
	bool useNativeFontRendering_ = true;
	char TITLE_FONT[BUF_SIZE] = "Bank Gothic";
	int TITLE_FONT_SIZE = 20;
	int TITLE_FONT_WEIGHT = 500;
	bool TITLE_ITALIC = false;
	bool TITLE_UNDERLINE = false;
	char ITEM_FONT[BUF_SIZE] = "Bank Gothic";
	int ITEM_FONT_SIZE = 15;
	int ITEM_FONT_WEIGHT = 500;
	bool ITEM_ITALIC = false;
	bool ITEM_UNDERLINE = false;

	double srTime_ = 0;
	int points_;
	int segment_ = 0;
	bool counting_;
	BGui::Gui* gui_ = nullptr;
	BGui::Text* T_title_ = nullptr;
	BGui::Label* title_ = nullptr;
	BGui::Panel* panel_ = nullptr;
	BGui::Panel* background_ = nullptr;
	int segmentCount_ = 0;
	char text[9][2][BUF_SIZE];
	BGui::Text* T_labels_[9][3];
	BGui::Label* labels_[9][3];
	double segmentTime_[13][9];
	IProperty* props_[30];
	int currentLevel_;
	double delta_;
	char timeString_[BUF_SIZE];
	char deltaString_[BUF_SIZE];
	long long loopCount_ = 0;
	std::vector<std::function<void()>> dutySlices_;
	bool skipEnabled_ = false;
	int skipStep_ = 60;
	bool enabled_ = true;

	bool isCustomMap(CKSTRING filename)
	{
		return std::string(filename).substr(0, 18) == R"(..\ModLoader\Maps\)";
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

	std::string serialize()
	{
		bool isFirst = true;
		std::stringstream ss;
		for (int i = 0; i < segmentCount_; i++)
		{
			ss << (isFirst ? "" : ",") << segmentTime_[currentLevel_ - 1][i] / 1000.0;
			isFirst = false;
		}
		std::string ret = ss.str();
		return ret;
	}
public:
	Segment(IBML* bml);
	virtual CKSTRING GetID() override { return "Segment"; }
	virtual CKSTRING GetVersion() override { 
		sprintf(SEG_VERSION, "%d.%d.%d", SEG_MAJOR_VER, SEG_MINOR_VER, SEG_PATCH_VER);
		return SEG_VERSION; 
	}
	virtual CKSTRING GetName() override { return "Segment"; }
	virtual CKSTRING GetAuthor() override { return "Swung0x48"; }
	virtual CKSTRING GetDescription() override { return "A mod to display your gameplay performance split into each segment."; }
	DECLARE_BML_VERSION;

	void RefreshConfig();
	void InitGui();

	void ClearRecord();

	virtual void OnLoad() override;
	virtual void OnModifyConfig(CKSTRING category, CKSTRING key, IProperty* prop) override;
	virtual void OnPreStartMenu() override;
	void LoadRecordFromConfig();
	virtual void OnPreEndLevel() override;
	virtual void OnCounterActive() override;
	virtual void OnCounterInactive() override;
	virtual void OnPauseLevel() override;
	virtual void OnUnpauseLevel() override;
	virtual void OnProcess() override;
	virtual void OnStartLevel() override;
	virtual void OnPreCheckpointReached() override;
	virtual void OnLoadObject(CKSTRING filename, BOOL isMap, CKSTRING masterName, CK_CLASSID filterClass,
		BOOL addtoscene, BOOL reuseMeshes, BOOL reuseMaterials, BOOL dynamic,
		XObjectArray* objArray, CKObject* masterObj);
	virtual void OnPreExitLevel() override;
	virtual void OnCheatEnabled(bool enable) override;
};

