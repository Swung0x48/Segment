#pragma once
#include <BML/BMLAll.h>
constexpr int SEG_MAJOR_VER = 0;
constexpr int SEG_MINOR_VER = 1;
constexpr int SEG_PATCH_VER = 0;
constexpr char SEG_VERSION[] = { SEG_MAJOR_VER + '0', '.', SEG_MINOR_VER + '0', '.', SEG_PATCH_VER + '0' };

extern "C" {
	__declspec(dllexport) IMod* BMLEntry(IBML* bml);
}

class Segment: public IMod
{
private:
	double srTime = 0;
	int points;
	int segment = 0;
	bool counting;
	CKDataArray* _energy;
	CKDataArray* _currentLevel;
	void Print(IBML* bml) const;

public:
	Segment(IBML* bml): IMod(bml) {}
	virtual CKSTRING GetID() override { return "Segment"; }
	virtual CKSTRING GetVersion() override { return SEG_VERSION; }
	virtual CKSTRING GetName() override { return "Segment"; }
	virtual CKSTRING GetAuthor() override { return "Swung0x48"; }
	virtual CKSTRING GetDescription() override { return "A mod to display your gameplay performance split into each segment."; }
	DECLARE_BML_VERSION;

	virtual void OnPreEndLevel() override;
	virtual void OnCounterActive() override;
	virtual void OnCounterInactive() override;
	virtual void OnPauseLevel() override;
	virtual void OnUnpauseLevel() override;
	virtual void OnProcess() override;
	virtual void OnStartLevel() override;
	virtual void OnPreCheckpointReached() override;
};

