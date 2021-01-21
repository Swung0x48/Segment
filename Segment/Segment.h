#pragma once
#include <BML/BMLAll.h>
constexpr int SEG_MAJOR_VER = 1;
constexpr int SEG_MINOR_VER = 0;
constexpr int SEG_PATCH_VER = 2;
constexpr char SEG_VERSION[] = { SEG_MAJOR_VER + '0', '.', SEG_MINOR_VER + '0', '.', SEG_PATCH_VER + '0' };

extern "C" {
	__declspec(dllexport) IMod* BMLEntry(IBML* bml);
}

class Segment: public IMod
{
private:
	const float TITLE_X_POS = 0.0f; // Title position (Segments)
	const float TITLE_Y_POS = 0.05f;
	const float PANEL_INIT_Y_POS = 0.085f; // Panel position (highlight background)

	const float TITLE_Y_SHIFT = 0.01f; // Between title & first row
	const float ITEM_Y_SHIFT = 0.03f; // Between rows
	const float TITLE_X_SHIFT = 0.02f; // Between heading (#n) & first col
	const float ITEM_X_SHIFT = 0.14f; // Between cols
	const float PANEL_Y_SHIFT = 0.03f; // Panel movement

	double srTime = 0;
	int points;
	int segment = 0;
	bool counting;
	//CKDataArray* _energy;
	CKDataArray* _currentLevel;
	BGui::Gui* _gui = nullptr;
	BGui::Label* _title = nullptr;
	BGui::Panel* _panel = nullptr;
	int _segmentCount = 0;
	BGui::Label* _labels[9][3];
	double _segmentTime[9];
	void Print(IBML* bml) const;
public:
	Segment(IBML* bml): IMod(bml) {}
	virtual CKSTRING GetID() override { return "Segment"; }
	virtual CKSTRING GetVersion() override { return SEG_VERSION; }
	virtual CKSTRING GetName() override { return "Segment"; }
	virtual CKSTRING GetAuthor() override { return "Swung0x48"; }
	virtual CKSTRING GetDescription() override { return "A mod to display your gameplay performance splitted into each segment."; }
	DECLARE_BML_VERSION;

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

