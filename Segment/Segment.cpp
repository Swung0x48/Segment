#include "Segment.h"

IMod* BMLEntry(IBML* bml) {
	return new Segment(bml);
}

Segment::Segment(IBML* bml) : IMod(bml) {
	_dutySlices.push_back([&]() {
		if (srTime / 1000.0 <= 9999.999)
			sprintf(_timeString, "%.3fs", srTime / 1000.0);
		else
			strcpy(_timeString, "9999.999s");
	});
	_dutySlices.push_back([&]() {
		_labels[segment][1]->SetText(_timeString);
	});
	_dutySlices.push_back([&]() {
		double currentTime = _segmentTime[segment];
		_delta = srTime - currentTime;
		if (currentTime < 0.0)
			_panel->SetColor(VxColor(EVEN_R, EVEN_G, EVEN_B, EVEN_A));
		else {
			if (_delta < 0.0)
				_panel->SetColor(VxColor(LEAD_R, LEAD_G, LEAD_B, LEAD_A));
			else if (_delta == 0.0)
				_panel->SetColor(VxColor(EVEN_R, EVEN_G, EVEN_B, EVEN_A));
			else
				_panel->SetColor(VxColor(LAG_R, LAG_G, LAG_B, LAG_A));
		}
	});
	_dutySlices.push_back([&]() {
		if (_delta / 1000.0 <= 9999.999 && _delta / 1000.0 >= -9999.999)
			sprintf(_deltaString, "%+.3fs", _delta / 1000.0f);
		else if (_delta / 1000.0 > 9999.999)
			strcpy(_deltaString, "+9999.999s");
		else
			strcpy(_deltaString, "-9999.999s");
	});
	_dutySlices.push_back([&]() {
		if (_segmentTime[segment] > 0.0)
			_labels[segment][2]->SetText(_deltaString);
	});
}

void Segment::OnLoad() {
	GetConfig()->SetCategoryComment("General", "General Settings");
	_props[0] = GetConfig()->GetProperty("General", "Enabled?");
	_props[0]->SetComment("Enable Segment");
	_props[0]->SetDefaultBoolean(true);

	GetConfig()->SetCategoryComment("Tweaking", "Performance Tweaking");
	_props[1] = GetConfig()->GetProperty("Tweaking", "SkipFrames?");
	_props[1]->SetComment("Enable frame skipping feature to optimize performance");
	_props[1]->SetDefaultBoolean(false);
	_props[2] = GetConfig()->GetProperty("Tweaking", "Skip_Step");
	_props[2]->SetComment("Skip GUI process every (Skip_Step):1 frame");
	_props[2]->SetDefaultInteger(240);

	GetConfig()->SetCategoryComment("Customize", "Appearance Customize");
	_props[3] = GetConfig()->GetProperty("Customize", "Lead_Color");
	_props[3]->SetComment("Leading highlight color, in r,g,b,a format. Default: 50,205,50,200");
	_props[3]->SetDefaultString("50,205,50,200");
	_props[4] = GetConfig()->GetProperty("Customize", "Even_Color");
	_props[4]->SetComment("Leading or default highlight color, in r,g,b,a format. Default: 255,168,0,200");
	_props[4]->SetDefaultString("255,168,0,200");
	_props[5] = GetConfig()->GetProperty("Customize", "Lag_Color");
	_props[5]->SetComment("Lagging highlight color, in r,g,b,a format. Default: 220,20,60,200");
	_props[5]->SetDefaultString("220,20,60,200");

	_enabled = _props[0]->GetBoolean();
	_skipEnabled = _props[1]->GetBoolean();
	_skipStep = _props[2]->GetInteger();
	sscanf(_props[3]->GetString(), "%d,%d,%d,%d", &LEAD_R, &LEAD_G, &LEAD_B, &LEAD_A);
	sscanf(_props[4]->GetString(), "%d,%d,%d,%d", &EVEN_R, &EVEN_G, &EVEN_B, &EVEN_A);
	sscanf(_props[5]->GetString(), "%d,%d,%d,%d", &LAG_R, &LAG_G, &LAG_B, &LAG_A);

	_gui = new BGui::Gui;
	_panel = _gui->AddPanel("M_Segment_Bg", VxColor(255, 168, 0, 200), TITLE_X_POS, PANEL_INIT_Y_POS, 0.35f, 0.03f);
	_panel->SetZOrder(0);
	_panel->SetVisible(false);

	_title = _gui->AddTextLabel("M_Segment_Title", "Segments:", ExecuteBB::GAMEFONT_01, TITLE_X_POS, TITLE_Y_POS, 0.2f, 0.03f);
	_title->SetZOrder(10);
	_title->SetAlignment(ALIGN_LEFT);
	_title->SetVisible(false);

	char labelText[5] = "#1: ";
	for (int i = 1; i <= 9; i++) {
		labelText[1] = i + '0';
		_labels[i - 1][0] = _gui->AddTextLabel("M_Segment_Seg", labelText, ExecuteBB::GAMEFONT_01, TITLE_X_POS, TITLE_Y_POS + TITLE_Y_SHIFT + (float)i * ITEM_Y_SHIFT, 0.2f, 0.03f);
		_labels[i - 1][0]->SetAlignment(ALIGN_LEFT);
		_labels[i - 1][0]->SetVisible(false);
		_labels[i - 1][1] = _gui->AddTextLabel("M_Segment_Time", "----", ExecuteBB::GAMEFONT_01, TITLE_X_POS + TITLE_X_SHIFT + 0.0f * ITEM_X_SHIFT, TITLE_Y_POS + TITLE_Y_SHIFT + (float)i * ITEM_Y_SHIFT, 0.2f, 0.03f);
		_labels[i - 1][1]->SetAlignment(ALIGN_CENTER);
		_labels[i - 1][1]->SetVisible(false);
		_labels[i - 1][2] = _gui->AddTextLabel("M_Segment_Score", "----", ExecuteBB::GAMEFONT_01, TITLE_X_POS + TITLE_X_SHIFT + 1.0f * ITEM_X_SHIFT, TITLE_Y_POS + TITLE_Y_SHIFT + (float)i * ITEM_Y_SHIFT, 0.2f, 0.03f);
		_labels[i - 1][2]->SetAlignment(ALIGN_CENTER);
		_labels[i - 1][2]->SetVisible(false);
	}
}

void Segment::OnModifyConfig(CKSTRING category, CKSTRING key, IProperty* prop) {
	_enabled = _props[0]->GetBoolean();
	_skipEnabled = _props[1]->GetBoolean();
	_skipStep = _props[2]->GetInteger();
	sscanf(_props[3]->GetString(), "%d,%d,%d,%d", &LEAD_R, &LEAD_G, &LEAD_B, &LEAD_A);
	sscanf(_props[4]->GetString(), "%d,%d,%d,%d", &EVEN_R, &EVEN_G, &EVEN_B, &EVEN_A);
	sscanf(_props[5]->GetString(), "%d,%d,%d,%d", &LAG_R, &LAG_G, &LAG_B, &LAG_A);

	if (prop == _props[0] && m_bml->IsIngame()) {
		_title->SetVisible(_enabled);
		_panel->SetVisible(_enabled);
		for (int i = 0; i < _segmentCount; i++) {
			_labels[i][0]->SetVisible(_enabled);
			_labels[i][1]->SetVisible(_enabled);
			_labels[i][2]->SetVisible(_enabled);
		}
	}
}

void Segment::OnPreStartMenu() {
	if (_panel != nullptr)
		_panel->SetVisible(false);
}

void Segment::OnLoadObject(CKSTRING filename, BOOL isMap, CKSTRING masterName, CK_CLASSID filterClass,
	BOOL addtoscene, BOOL reuseMeshes, BOOL reuseMaterials, BOOL dynamic,
	XObjectArray* objArray, CKObject* masterObj) {
	if (!isMap)
		return;

	char buffer[20];
	for (int i = 1; i <= 9; i++) {
		sprintf_s(buffer, "Sector_%02d", i);
		if (m_bml->GetGroupByName(buffer) == nullptr)
			break;

		_segmentCount = i;
	}

	for (double& i : _segmentTime)
		i = -1.0;

	this->srTime = 0;
	_title->SetVisible(_enabled);
	_panel->SetVisible(_enabled);
	_panel->SetPosition(Vx2DVector(0.0f, PANEL_INIT_Y_POS + static_cast<float>(segment) * PANEL_Y_SHIFT));
	for (int i = 0; i < _segmentCount; i++) {
		_labels[i][0]->SetVisible(_enabled);
		_labels[i][1]->SetVisible(_enabled);
		_labels[i][2]->SetVisible(_enabled);
	}
}

void Segment::OnPreEndLevel()
{
	_segmentTime[segment] = srTime;
	segment++;
	_panel->SetVisible(false);
	this->counting = false;
	_gui->Process();
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
	_loopCount++;

	if (this->counting)
		this->srTime += static_cast<double>(m_bml->GetTimeManager()->GetLastDeltaTime());
	

	if (_enabled) {
		if (m_bml->IsIngame()) {
			_dutySlices[_loopCount % _dutySlices.size()]();
			
			//_gui->Process();
			if (_loopCount % _skipStep != 0 || !_skipEnabled) {
				_title->Process();
				for (int i = 0; i < _segmentCount; i++) {
					_labels[i][0]->Process();
					_labels[i][1]->Process();
					_labels[i][2]->Process();
				}
			}
		}
	}
}

void Segment::OnStartLevel()
{
	this->counting = false;
	this->srTime = 0;
	this->segment = 0;
	_panel->SetPosition(Vx2DVector(0.0f, PANEL_INIT_Y_POS + static_cast<float>(segment) * PANEL_Y_SHIFT));
}

void Segment::OnPreCheckpointReached()
{
	if (_segmentTime[segment] < 0.0 || srTime < _segmentTime[segment])
		_segmentTime[segment] = srTime;

	this->segment++;
	_panel->SetPosition(Vx2DVector(0.0f, PANEL_INIT_Y_POS + static_cast<float>(segment) * PANEL_Y_SHIFT));

	srTime = 0;
}