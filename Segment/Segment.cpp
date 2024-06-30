#include "Segment.h"
#include "CommandSeg.h"
#include <iostream>

IMod* BMLEntry(IBML* bml) {
	return new Segment(bml);
}

Segment::Segment(IBML* bml) : IMod(bml) {
}

void Segment::OnLoad() {
	m_bml->RegisterCommand(new CommandSeg(this));

	load_sessions_from_file();
}

void Segment::OnModifyConfig(CKSTRING category, CKSTRING key, IProperty* prop) {
}

void Segment::OnPreStartMenu() {
	for (auto& t : queued_tasks_) {
		t();
	}
	queued_tasks_.clear();
}

void Segment::OnLoadObject(CKSTRING filename, BOOL isMap, CKSTRING masterName, CK_CLASSID filterClass,
													 BOOL addtoscene, BOOL reuseMeshes, BOOL reuseMaterials, BOOL dynamic,
													 XObjectArray* objArray, CKObject* masterObj) {
	if (!isMap)
		return;
	const int current_level = get_current_level();
	const int sector_count = get_sector_count();

	// TODO: S/L states
	//state_ = std::make_unique<PerLevelSegmentState>(sector_count);
	//gui_ = std::make_unique<SegmentGui>(*state_, get_current_level());
	
	auto* man = m_bml->GetPathManager();

	if (sessions_.find(filename) == sessions_.end()) {
		if (!is_custom_map(filename))
			sessions_[filename] = std::make_shared<session>(get_current_level(), get_sector_count());
		else {
			CKPathSplitter splitter(const_cast<char*>(filename));
			std::string name = std::format("\"{}\"",splitter.GetName());
			sessions_[filename] = std::make_shared<session>(name, get_sector_count());
		}
	}
	session_ = sessions_[filename];
	session_->gui.set_cursor_visible(true);
}

void Segment::OnPreExitLevel()
{
	save_pico_to_file(serialize_sessions_to_pico());
	queued_tasks_.emplace_back([s = session_]() {
		s->state.update_target_figures();
	});
}

void Segment::OnCheatEnabled(bool enable)
{
}

void Segment::OnGameOver()
{
	session_->gui.set_cursor_visible(false);
}

void Segment::OnPreEndLevel()
{
	session_->gui.set_cursor_visible(false);
	session_->state.enable_counting(false);
	session_->state.change_segment(get_current_sector() + 1);
	//session_->state.update_target_figures();
	save_pico_to_file(serialize_sessions_to_pico(serialize_from_t::Current));
}

void Segment::OnCounterActive()
{
	session_->state.enable_counting(true);
}

void Segment::OnCounterInactive()
{	
	session_->state.enable_counting(false);
}

void Segment::OnPauseLevel()
{
	session_->state.enable_counting(false);
}

void Segment::OnUnpauseLevel()
{
	session_->state.enable_counting(true);
}

void Segment::OnProcess()
{
	if (!m_bml->IsIngame()) return;

	session_->state.update(m_bml->GetTimeManager()->GetLastDeltaTime() / 1000.);
	session_->gui.update();
}

void Segment::OnStartLevel()
{
	session_->state.reset();
	session_->gui.set_cursor_visible(true);
}

void Segment::OnPostCheckpointReached()
{
	session_->state.change_segment(get_current_sector());
}