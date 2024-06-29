#include "Segment.h"
#include "CommandSeg.h"

IMod* BMLEntry(IBML* bml) {
	return new Segment(bml);
}

Segment::Segment(IBML* bml) : IMod(bml) {
}

void Segment::OnLoad() {
	m_bml->RegisterCommand(new CommandSeg(this));
}

void Segment::OnModifyConfig(CKSTRING category, CKSTRING key, IProperty* prop) {
}

void Segment::OnPreStartMenu() {

}

void Segment::OnLoadObject(CKSTRING filename, BOOL isMap, CKSTRING masterName, CK_CLASSID filterClass,
													 BOOL addtoscene, BOOL reuseMeshes, BOOL reuseMaterials, BOOL dynamic,
													 XObjectArray* objArray, CKObject* masterObj) {
	if (!isMap)
		return;
	//const int current_level = get_current_level();
	const int sector_count = get_sector_count();

	// TODO: S/L states
	state_ = std::make_unique<PerLevelSegmentState>(sector_count);
	gui_ = std::make_unique<SegmentGui>(*state_, get_current_level());
}

void Segment::OnPreExitLevel()
{
}

void Segment::OnCheatEnabled(bool enable)
{
}

void Segment::OnGameOver()
{

}

void Segment::OnPreEndLevel()
{
}

void Segment::OnCounterActive()
{
	state_->enable_counting(true);
}

void Segment::OnCounterInactive()
{	
	state_->enable_counting(false);
}

void Segment::OnPauseLevel()
{
	state_->enable_counting(false);
}

void Segment::OnUnpauseLevel()
{
	state_->enable_counting(true);
}

void Segment::OnProcess()
{
	if (!m_bml->IsIngame()) return;

	state_->update(m_bml->GetTimeManager()->GetLastDeltaTime() / 1000.);
	gui_->update();
}

void Segment::OnStartLevel()
{
	state_->reset();
}

void Segment::OnPostCheckpointReached()
{
	state_->change_segment(get_current_sector());
}