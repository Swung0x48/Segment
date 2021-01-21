#include "Segment.h"
#include <iomanip>
#include <sstream>

IMod* BMLEntry(IBML* bml) {
	return new Segment(bml);
}

/*void Segment::Print(IBML* bml) const
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
}*/

void Segment::OnPreStartMenu() {
	_panel->SetVisible(false);
}

void Segment::OnLoadObject(CKSTRING filename, BOOL isMap, CKSTRING masterName, CK_CLASSID filterClass,
	BOOL addtoscene, BOOL reuseMeshes, BOOL reuseMaterials, BOOL dynamic,
	XObjectArray* objArray, CKObject* masterObj) {
	_gui = new BGui::Gui;
	_panel = _gui->AddPanel("M_Segment_Bg", VxColor(255, 168, 0, 200), TITLE_X_POS, PANEL_INIT_Y_POS, 0.35f, 0.03f);
	_panel->SetZOrder(0);
	//_panel->SetPosition(Vx2DVector(0.0f, PANEL_INIT_Y_POS + 1.0 * PANEL_Y_SHIFT));
	_panel->SetVisible(false);
	_title = _gui->AddTextLabel("M_Segment_Title", "Segments:", ExecuteBB::GAMEFONT_01, TITLE_X_POS, TITLE_Y_POS, 0.2f, 0.03f);
	_title->SetZOrder(10);
	_title->SetAlignment(ALIGN_LEFT);
	_title->SetVisible(false);

	char labelText[5] = "#1: ";
	for (int i = 1; i <= 9; i++) {
		labelText[1] = i + '0';
		_labels[i-1][0] = _gui->AddTextLabel("M_Segment_Seg", labelText, ExecuteBB::GAMEFONT_01, TITLE_X_POS, TITLE_Y_POS + TITLE_Y_SHIFT + (float) i * ITEM_Y_SHIFT, 0.2f, 0.03f);
		_labels[i-1][0]->SetAlignment(ALIGN_LEFT);
		_labels[i-1][0]->SetVisible(false);
		_labels[i-1][1] = _gui->AddTextLabel("M_Segment_Time", "----", ExecuteBB::GAMEFONT_01, TITLE_X_POS + TITLE_X_SHIFT + 0.0f * ITEM_X_SHIFT, TITLE_Y_POS + TITLE_Y_SHIFT + (float)i * ITEM_Y_SHIFT, 0.2f, 0.03f);
		_labels[i-1][1]->SetAlignment(ALIGN_CENTER);
		_labels[i-1][1]->SetVisible(false);
		_labels[i-1][2] = _gui->AddTextLabel("M_Segment_Score", "----", ExecuteBB::GAMEFONT_01, TITLE_X_POS + TITLE_X_SHIFT + 1.0f * ITEM_X_SHIFT, TITLE_Y_POS + TITLE_Y_SHIFT + (float)i * ITEM_Y_SHIFT, 0.2f, 0.03f);
		_labels[i-1][2]->SetAlignment(ALIGN_CENTER);
		_labels[i-1][2]->SetVisible(false);
	}

	if (!(strlen(filename) == 30 && filename[18] == 'L' && filename[19] == 'e' && filename[23] == '_'))
		return;
	char buffer[20];
	for (int i = 1; i <= 9; i++) {
		//std::stringstream ss;
		sprintf_s(buffer, "Sector_%02d", i);
		//ss << "Sector_" << std::setfill('0') << std::setw(2) << i;
		//auto group = m_bml->GetGroupByName(buffer);
		if (m_bml->GetGroupByName(buffer) == nullptr)
			break;

		_segmentCount = i;
	}

	//memset(_segmentTime, 0, sizeof(_segmentTime));

	for (int i = 0; i < 9; i++)
		_segmentTime[i] = 0.0f;
	//sprintf_s(buffer, "Sector count: %d", _segmentCount);
	//std::stringstream ss;
	//ss << "Sector count: " << _segmentCount;
	//m_bml->SendIngameMessage(buffer);

	this->srTime = 0;
}

void Segment::OnPreEndLevel()
{
	_segmentTime[segment] = srTime;
	segment++;
	_panel->SetVisible(false);
	//_energy->GetElementValue(0, 0, &(this->points));
	//this->segment = 0;
	//m_bml->SendIngameMessage("Finished!");
	//Print(m_bml);
	//m_bml->SendIngameMessage("----------");
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
	if (this->counting)
		this->srTime += static_cast<double>(m_bml->GetTimeManager()->GetLastDeltaTime());
	
	if (m_bml->IsIngame())
		assert(segment <= _segmentCount);

	char timeString[10];
	sprintf_s(timeString, "%2.3fs", srTime / 1000.0f);
	_labels[segment][1]->SetText(timeString);

	char deltaString[10];

	double currentTime = _segmentTime[segment];
	if (currentTime == 0.0f)
		_panel->SetColor(VxColor(255, 168, 0, 200));
	else {
		double delta = srTime - currentTime;
		sprintf_s(deltaString, "%+2.3fs", delta / 1000.0f);
		_labels[segment][2]->SetText(deltaString);
		
		if (delta < 0.0f)
			_panel->SetColor(VxColor(50, 205, 50, 200));
		else if (delta == 0.0f)
			_panel->SetColor(VxColor(255, 168, 0, 200));
		else
			_panel->SetColor(VxColor(220, 20, 60, 200));
	}

	_gui->Process();
}

void Segment::OnStartLevel()
{
	//this->_energy = m_bml->GetArrayByName("Energy");
	//this->_currentLevel = m_bml->GetArrayByName("CurrentLevel");
	//int currentLevel;
	//this->_currentLevel->GetElementValue(0, 0, &currentLevel);
	//m_bml->SendIngameMessage("----------");
	//std::stringstream ss;
	//ss << "Starting level " << currentLevel << "...";
	//m_bml->SendIngameMessage(ss.str().c_str());
	this->counting = false;
	this->srTime = 0;
	this->segment = 0;

	_title->SetVisible(true);
	_panel->SetVisible(true);
	_panel->SetPosition(Vx2DVector(0.0f, PANEL_INIT_Y_POS + (float)segment * PANEL_Y_SHIFT));
	for (int i = 0; i < _segmentCount; i++) {
		_labels[i][0]->SetVisible(true);
		_labels[i][1]->SetVisible(true);
		_labels[i][2]->SetVisible(true);
	}
	_labels[0][1]->SetText("00.000s");
	//_energy->GetElementValue(0, 0, &(this->points));
	//char buf[5];
	//sprintf_s(buf, "%04d", points);
	//_segments[0][2]->SetText(buf);
}

void Segment::OnPreCheckpointReached()
{
	//_energy->GetElementValue(0, 0, &(this->points));
	if (_segmentTime[segment] == 0.0f || srTime < _segmentTime[segment])
		_segmentTime[segment] = srTime;
	this->segment++;
	_panel->SetPosition(Vx2DVector(0.0f, PANEL_INIT_Y_POS + (float)segment * PANEL_Y_SHIFT));
	//Print(m_bml);
	srTime = 0;
}