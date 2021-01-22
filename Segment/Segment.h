#pragma once
#include <BML/BMLAll.h>
constexpr int SEG_MAJOR_VER = 1;
constexpr int SEG_MINOR_VER = 0;
constexpr int SEG_PATCH_VER = 9;
constexpr char SEG_VERSION[] = { SEG_MAJOR_VER + '0', '.', SEG_MINOR_VER + '0', '.', SEG_PATCH_VER + '0' };

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

	static constexpr int BUF_SIZE = 50;

	double srTime = 0;
	int points;
	int segment = 0;
	bool counting;
	CKDataArray* _currentLevel;
	BGui::Gui* _gui = nullptr;
	BGui::Label* _title = nullptr;
	BGui::Panel* _panel = nullptr;
	int _segmentCount = 0;
	BGui::Label* _labels[9][3];
	double _segmentTime[9];
	IProperty* _props[1];
	bool _enabled = true;
public:
	Segment(IBML* bml): IMod(bml) {}
	virtual CKSTRING GetID() override { return "Segment"; }
	virtual CKSTRING GetVersion() override { return SEG_VERSION; }
	virtual CKSTRING GetName() override { return "Segment"; }
	virtual CKSTRING GetAuthor() override { return "Swung0x48"; }
	virtual CKSTRING GetDescription() override { return "A mod to display your gameplay performance splitted into each segment."; }
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
	virtual void OnPreCheckpointReached() override;
	virtual void OnLoadObject(CKSTRING filename, BOOL isMap, CKSTRING masterName, CK_CLASSID filterClass,
		BOOL addtoscene, BOOL reuseMeshes, BOOL reuseMaterials, BOOL dynamic,
		XObjectArray* objArray, CKObject* masterObj);
};

