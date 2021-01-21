#include "Segment.h"
#include <iomanip>
#include <sstream>

IMod* BMLEntry(IBML* bml) {
	return new Segment(bml);
}

void Segment::Print(IBML* bml) const
{
	std::stringstream ss;
	if (segment != 0) {
		ss << "#" << this->segment << ":";
		bml->SendIngameMessage(ss.str().c_str()); ss.str("");
	}
	
	ss << "Points: " << this->points;
	bml->SendIngameMessage(ss.str().c_str()); ss.str("");
	ss << "Time: " << this->srTime / 1000.0 << "s";
	bml->SendIngameMessage(ss.str().c_str()); ss.str("");
}

void Segment::OnLoadObject(CKSTRING filename, BOOL isMap, CKSTRING masterName, CK_CLASSID filterClass,
	BOOL addtoscene, BOOL reuseMeshes, BOOL reuseMaterials, BOOL dynamic,
	XObjectArray* objArray, CKObject* masterObj) {
	_gui = new BGui::Gui;
	
	_title = _gui->AddTextLabel("M_Segment_Title", "Segments:", ExecuteBB::GAMEFONT_01, TITLE_X_POS, TITLE_Y_POS, 0.2f, 0.03f);
	_title->SetAlignment(ALIGN_LEFT);
	_title->SetVisible(false);

	for (int i = 1; i <= 9; i++) {
		//BGui::Label* label[3];
		_segments[i-1][0] = _gui->AddTextLabel("M_Segment_Seq", "#1:", ExecuteBB::GAMEFONT_01, TITLE_X_POS, TITLE_Y_POS + TITLE_Y_SHIFT + (float) i * ITEM_Y_SHIFT, 0.2f, 0.03f);
		_segments[i-1][0]->SetAlignment(ALIGN_LEFT);
		_segments[i-1][0]->SetVisible(false);
		_segments[i-1][1] = _gui->AddTextLabel("M_Segment_Time", "00.000s", ExecuteBB::GAMEFONT_01, TITLE_X_POS + TITLE_X_SHIFT + 0.0f * ITEM_X_SHIFT, TITLE_Y_POS + TITLE_Y_SHIFT + (float)i * ITEM_Y_SHIFT, 0.2f, 0.03f);
		_segments[i-1][1]->SetAlignment(ALIGN_CENTER);
		_segments[i-1][1]->SetVisible(false);
		_segments[i-1][2] = _gui->AddTextLabel("M_Segment_Score", "1234", ExecuteBB::GAMEFONT_01, TITLE_X_POS + TITLE_X_SHIFT + 1.0f * ITEM_X_SHIFT, TITLE_Y_POS + TITLE_Y_SHIFT + (float)i * ITEM_Y_SHIFT, 0.2f, 0.03f);
		_segments[i-1][2]->SetAlignment(ALIGN_CENTER);
		_segments[i-1][2]->SetVisible(false);
	}

	if (!(strlen(filename) == 30 && filename[18] == 'L' && filename[19] == 'e' && filename[23] == '_'))
		return;
	for (int i = 1; i <= 9; i++) {
		std::stringstream ss;
		ss << "Sector_" << std::setfill('0') << std::setw(2) << i;
		auto group = m_bml->GetGroupByName(ss.str().c_str());
		if (m_bml->GetGroupByName(ss.str().c_str()) == nullptr)
			break;

		_segmentCount = i;
	}

	std::stringstream ss;
	ss << "Sector count: " << _segmentCount;
	m_bml->SendIngameMessage(ss.str().c_str());
}

void Segment::OnPreEndLevel()
{
	_energy->GetElementValue(0, 0, &(this->points));
	this->segment = 0;
	m_bml->SendIngameMessage("Finished!");
	Print(m_bml);
	m_bml->SendIngameMessage("----------");
	this->srTime = 0;
	this->counting = false;
}

void Segment::OnCounterActive()
{
	this->counting = true;
}

void Segment::OnCounterInactive()
{	
	this->counting = false;
}

void Segment::OnPauseLevel()
{
	this->counting = false;
}

void Segment::OnUnpauseLevel()
{
	this->counting = true;
}

void Segment::OnProcess()
{
	_gui->Process();

	if (this->counting)
		this->srTime += static_cast<double>(m_bml->GetTimeManager()->GetLastDeltaTime());
}

void Segment::OnStartLevel()
{
	this->_energy = m_bml->GetArrayByName("Energy");
	this->_currentLevel = m_bml->GetArrayByName("CurrentLevel");
	int currentLevel;
	this->_currentLevel->GetElementValue(0, 0, &currentLevel);
	m_bml->SendIngameMessage("----------");
	std::stringstream ss;
	ss << "Starting level " << currentLevel << "...";
	m_bml->SendIngameMessage(ss.str().c_str());
	this->counting = false;
	this->srTime = 0;
	this->segment = 0;

	_title->SetVisible(true);
	for (int i = 0; i < _segmentCount; i++) {
		_segments[i][0]->SetVisible(true);
		_segments[i][1]->SetVisible(true);
		_segments[i][2]->SetVisible(true);
	}
}

void Segment::OnPreCheckpointReached()
{
	_energy->GetElementValue(0, 0, &(this->points));
	++this->segment;
	Print(m_bml);
}
