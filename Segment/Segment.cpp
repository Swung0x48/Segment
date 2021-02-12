#include "Segment.h"
#include "CommandSeg.h"

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
		if (_useNativeFontRendering)
			_labels[segment][1]->SetText(_timeString);
		else
			T_labels[segment][1]->SetText(_timeString);
	});
	_dutySlices.push_back([&]() {
		double currentTime = _segmentTime[_currentLevel - 1][segment];
		if (currentTime < 0.0) {
			_panel->SetColor(VxColor(EVEN_R, EVEN_G, EVEN_B, EVEN_A));
			return;
		}
		
		_delta = srTime - currentTime;
		if (_delta < 0.0)
			_panel->SetColor(VxColor(LEAD_R, LEAD_G, LEAD_B, LEAD_A));
		else if (_delta == 0.0)
			_panel->SetColor(VxColor(EVEN_R, EVEN_G, EVEN_B, EVEN_A));
		else
			_panel->SetColor(VxColor(LAG_R, LAG_G, LAG_B, LAG_A));
		
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
		if (_segmentTime[_currentLevel - 1][segment] > 0.0) {
			if (_useNativeFontRendering)
				_labels[segment][2]->SetText(_deltaString);
			else
				T_labels[segment][2]->SetText(_deltaString);
		}
	});
}

void Segment::RefreshConfig() {
	_enabled = _props[0]->GetBoolean();
	_skipEnabled = _props[1]->GetBoolean();
	_skipStep = _props[2]->GetInteger();
	sscanf(_props[3]->GetString(), "%d,%d,%d,%d", &LEAD_R, &LEAD_G, &LEAD_B, &LEAD_A);
	sscanf(_props[4]->GetString(), "%d,%d,%d,%d", &EVEN_R, &EVEN_G, &EVEN_B, &EVEN_A);
	sscanf(_props[5]->GetString(), "%d,%d,%d,%d", &LAG_R, &LAG_G, &LAG_B, &LAG_A);
	_useNativeFontRendering = _props[6]->GetBoolean();
	strcpy(TITLE_FONT, _props[7]->GetString());
	TITLE_FONT_SIZE = _props[8]->GetInteger();
	TITLE_FONT_WEIGHT = _props[9]->GetInteger();
	TITLE_ITALIC = _props[10]->GetBoolean();
	TITLE_UNDERLINE = _props[11]->GetBoolean();
	strcpy(ITEM_FONT, _props[12]->GetString());
	ITEM_FONT_SIZE = _props[13]->GetInteger();
	ITEM_FONT_WEIGHT = _props[14]->GetInteger();
	ITEM_ITALIC = _props[15]->GetBoolean();
	ITEM_UNDERLINE = _props[16]->GetBoolean();
}

void Segment::InitGui()
{
	_gui = new BGui::Gui;
	_panel = _gui->AddPanel("M_Segment_Highlight", VxColor(EVEN_R, EVEN_G, EVEN_B, EVEN_A), TITLE_X_POS, PANEL_INIT_Y_POS, PANEL_WIDTH, PANEL_HEIGHT);
	_panel->SetZOrder(0);
	_panel->SetVisible(false);
	_background = _gui->AddPanel("M_Segment_Background", VxColor(0, 0, 0, 50), TITLE_X_POS, TITLE_Y_POS, PANEL_WIDTH, PANEL_HEIGHT);
	_background->SetZOrder(-1);
	_background->SetVisible(false);

	if (_useNativeFontRendering)
	{
		_title = _gui->AddTextLabel("M_Segment_Title", "Segments:", ExecuteBB::GAMEFONT_01, TITLE_X_POS, TITLE_Y_POS, 0.2f, 0.03f);
		_title->SetZOrder(10);
		_title->SetAlignment(ALIGN_LEFT);
		_title->SetVisible(false);
	} else {
		T_title = _gui->AddText("M_Segment_Title", "Segments:", TITLE_X_POS, TITLE_Y_POS, 0.2f, 0.03f);
		T_title->SetZOrder(10);
		T_title->SetFont(TITLE_FONT, TITLE_FONT_SIZE, TITLE_FONT_WEIGHT, TITLE_ITALIC, TITLE_UNDERLINE);
		T_title->SetAlignment(CKSPRITETEXT_LEFT);
		T_title->SetVisible(false);
	}
	
	char labelText[5] = "#1: ";
	for (int i = 1; i <= 9; i++) {
		labelText[1] = i + '0';
		if (_useNativeFontRendering) {
			_labels[i - 1][0] = _gui->AddTextLabel("M_Segment_Seg", labelText, ExecuteBB::GAMEFONT_01, TITLE_X_POS, TITLE_Y_POS + TITLE_Y_SHIFT + (float)i * ITEM_Y_SHIFT, 0.2f, 0.03f);
			_labels[i - 1][0]->SetAlignment(ALIGN_LEFT);
			_labels[i - 1][0]->SetVisible(false);
			_labels[i - 1][1] = _gui->AddTextLabel("M_Segment_Time", "----", ExecuteBB::GAMEFONT_01, TITLE_X_POS + TITLE_X_SHIFT + 0.0f * ITEM_X_SHIFT, TITLE_Y_POS + TITLE_Y_SHIFT + (float)i * ITEM_Y_SHIFT, 0.2f, 0.03f);
			_labels[i - 1][1]->SetAlignment(ALIGN_CENTER);
			_labels[i - 1][1]->SetVisible(false);
			_labels[i - 1][2] = _gui->AddTextLabel("M_Segment_Score", "----", ExecuteBB::GAMEFONT_01, TITLE_X_POS + TITLE_X_SHIFT + 1.0f * ITEM_X_SHIFT, TITLE_Y_POS + TITLE_Y_SHIFT + (float)i * ITEM_Y_SHIFT, 0.2f, 0.03f);
			_labels[i - 1][2]->SetAlignment(ALIGN_CENTER);
			_labels[i - 1][2]->SetVisible(false);
		} else {
			T_labels[i - 1][0] = _gui->AddText("M_Segment_Seg", labelText, TITLE_X_POS, TITLE_Y_POS + TITLE_Y_SHIFT + (float)i * ITEM_Y_SHIFT, 0.2f, 0.03f);
			T_labels[i - 1][0]->SetAlignment(CKSPRITETEXT_LEFT);
			T_labels[i - 1][0]->SetFont(ITEM_FONT, ITEM_FONT_SIZE, ITEM_FONT_WEIGHT, ITEM_ITALIC, ITEM_UNDERLINE);
			T_labels[i - 1][0]->SetVisible(false);
			T_labels[i - 1][1] = _gui->AddText("M_Segment_Time", "----", TITLE_X_POS + TITLE_X_SHIFT + 0.0f * ITEM_X_SHIFT, TITLE_Y_POS + TITLE_Y_SHIFT + (float)i * ITEM_Y_SHIFT, 0.2f, 0.03f);
			T_labels[i - 1][1]->SetAlignment(CKSPRITETEXT_CENTER);
			T_labels[i - 1][1]->SetFont(ITEM_FONT, ITEM_FONT_SIZE, ITEM_FONT_WEIGHT, ITEM_ITALIC, ITEM_UNDERLINE);
			T_labels[i - 1][1]->SetVisible(false);
			T_labels[i - 1][2] = _gui->AddText("M_Segment_Score", "----", TITLE_X_POS + TITLE_X_SHIFT + 1.0f * ITEM_X_SHIFT, TITLE_Y_POS + TITLE_Y_SHIFT + (float)i * ITEM_Y_SHIFT, 0.2f, 0.03f);
			T_labels[i - 1][2]->SetAlignment(CKSPRITETEXT_CENTER);
			T_labels[i - 1][2]->SetFont(ITEM_FONT, ITEM_FONT_SIZE, ITEM_FONT_WEIGHT, ITEM_ITALIC, ITEM_UNDERLINE);
			T_labels[i - 1][2]->SetVisible(false);
		}
	}
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
	_props[6] = GetConfig()->GetProperty("Tweaking", "NativeFontRendering");
	_props[6]->SetComment("");
	_props[6]->SetDefaultBoolean(true);

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
	_props[7] = GetConfig()->GetProperty("Customize", "Title_Font");
	_props[7]->SetComment("");
	_props[7]->SetDefaultString("Bank Gothic");
	_props[8] = GetConfig()->GetProperty("Customize", "Title_Font_Size");
	_props[8]->SetComment("");
	_props[8]->SetDefaultInteger(20);
	_props[9] = GetConfig()->GetProperty("Customize", "Title_Font_Weight");
	_props[9]->SetComment("");
	_props[9]->SetDefaultInteger(500);
	_props[10] = GetConfig()->GetProperty("Customize", "Title_Italic");
	_props[10]->SetComment("");
	_props[10]->SetDefaultBoolean(false);
	_props[11] = GetConfig()->GetProperty("Customize", "Title_Underline");
	_props[11]->SetComment("");
	_props[11]->SetDefaultBoolean(false);
	_props[12] = GetConfig()->GetProperty("Customize", "Item_Font");
	_props[12]->SetComment("");
	_props[12]->SetDefaultString("Bank Gothic");
	_props[13] = GetConfig()->GetProperty("Customize", "Item_Font_Size");
	_props[13]->SetComment("");
	_props[13]->SetDefaultInteger(20);
	_props[14] = GetConfig()->GetProperty("Customize", "Item_Font_Weight");
	_props[14]->SetComment("");
	_props[14]->SetDefaultInteger(500);
	_props[15] = GetConfig()->GetProperty("Customize", "Item_Italic");
	_props[15]->SetComment("");
	_props[15]->SetDefaultBoolean(false);
	_props[16] = GetConfig()->GetProperty("Customize", "Item_Underline");
	_props[16]->SetComment("");
	_props[16]->SetDefaultBoolean(false);

	GetConfig()->SetCategoryComment("Record", "Record Management");
	char buffer[BUF_SIZE];
	for (int i = 1; i <= 13; i++)
	{
		sprintf(buffer, "Level_%02d", i);
		_props[16 + i] = GetConfig()->GetProperty("Record", buffer);
		_props[16 + i]->SetComment("");
		_props[16 + i]->SetDefaultString("");
	}
	
	RefreshConfig();
	InitGui();
	m_bml->RegisterCommand(new CommandSeg(this));
}

void Segment::OnModifyConfig(CKSTRING category, CKSTRING key, IProperty* prop) {
	RefreshConfig();
	if (m_bml->IsIngame())
		LoadRecordFromConfig();

	if (prop == _props[0] && m_bml->IsIngame()) {
		if (_useNativeFontRendering)
			_title->SetVisible(_enabled);
		else
			T_title->SetVisible(_enabled);
		_panel->SetVisible(_enabled);
		_background->SetVisible(_enabled);
		for (int i = 0; i < _segmentCount; i++) {
			if (_useNativeFontRendering) {
				_labels[i][0]->SetVisible(_enabled);
				_labels[i][1]->SetVisible(_enabled);
				_labels[i][2]->SetVisible(_enabled);
			} else {
				T_labels[i][0]->SetVisible(_enabled);
				T_labels[i][1]->SetVisible(_enabled);
				T_labels[i][2]->SetVisible(_enabled);
			}
		}
	} 
	
	if (prop == _props[6]) {
		delete _title;
		delete T_title;
		_title = nullptr;
		T_title = nullptr;
		delete _panel;
		delete _background;

		if (_useNativeFontRendering) {
			for (int j = 0; j < 9; j++) {
				for (int k = 0; k < 3; k++) {
					if (k >= 1)
						strcpy(text[j][k - 1], T_labels[j][k]->GetText());
					delete T_labels[j][k];
				}
			}
		} else {
			for (int j = 0; j < 9; j++) {
				for (int k = 0; k < 3; k++) {
					if (k >= 1)
						strcpy(text[j][k - 1], _labels[j][k]->GetText());
					delete _labels[j][k];
				}
			}
		}
		delete _gui;
		InitGui();
		if (_useNativeFontRendering) {
			for (int j = 0; j < 9; j++) {
				for (int k = 1; k < 3; k++) {
					_labels[j][k]->SetText(text[j][k - 1]);
				}
			}
		}
		else {
			for (int j = 0; j < 9; j++) {
				for (int k = 1; k < 3; k++) {
					T_labels[j][k]->SetText(text[j][k - 1]);
				}
			}
		}
		_panel->SetVisible(_enabled && m_bml->IsIngame());
		_panel->SetPosition(Vx2DVector(0.0f, PANEL_INIT_Y_POS + static_cast<float>(segment) * PANEL_Y_SHIFT));
		_background->SetVisible(_enabled && m_bml->IsIngame());
		_background->SetSize(Vx2DVector(PANEL_WIDTH, (float)_segmentCount * PANEL_HEIGHT + PANEL_INIT_HEIGHT));
		for (int i = 0; i < _segmentCount; i++)
		{
			if (_useNativeFontRendering) {
				_title->SetVisible(_enabled && m_bml->IsIngame());
				for (auto* j: _labels[i])
				{
					j->SetVisible(_enabled && m_bml->IsIngame());
				}
			} else {
				T_title->SetVisible(_enabled && m_bml->IsIngame());
				for (auto* j : T_labels[i])
				{
					j->SetVisible(_enabled && m_bml->IsIngame());
				}
			}
		}
	}

	for (int i = 7; i <= 11; i++) {
		if (_useNativeFontRendering)
			break;
		if (prop == _props[i]) {
			T_title->SetFont(TITLE_FONT, TITLE_FONT_SIZE, TITLE_FONT_WEIGHT, TITLE_ITALIC, TITLE_UNDERLINE);
			break;
		}
	}

	for (int i = 12; i <= 16; i++) {
		if (_useNativeFontRendering)
			break;
		if (prop == _props[i]) {
			for (auto& j: T_labels) {
				for (auto& k : j)
					k->SetFont(ITEM_FONT, ITEM_FONT_SIZE, ITEM_FONT_WEIGHT, ITEM_ITALIC, ITEM_UNDERLINE);
			}
			break;
		}
	}
}

void Segment::OnPreStartMenu() {
	if (_panel != nullptr)
		_panel->SetVisible(false);
	if (_background != nullptr)
		_background->SetVisible(false);
	if (_useNativeFontRendering)
		_title->SetVisible(false);
	else
		T_title->SetVisible(false);
	for (int i = 1; i <= 9; i++) {
		if (_useNativeFontRendering)
		{
			_labels[i - 1][0]->SetVisible(false);
			_labels[i - 1][1]->SetVisible(false);
			_labels[i - 1][2]->SetVisible(false);
		} else {
			T_labels[i - 1][0]->SetVisible(false);
			T_labels[i - 1][1]->SetVisible(false);
			T_labels[i - 1][2]->SetVisible(false);
		}
	}
}

void Segment::ClearRecord()
{
	for (int i = 1; i <= 9; i++) {
		_segmentTime[_currentLevel - 1][i - 1] = -1;

		if (_useNativeFontRendering) {
			_labels[i - 1][2]->SetText("----");
		} else {
			T_labels[i - 1][2]->SetText("----");
		}
	}
}

void Segment::LoadRecordFromConfig()
{
	std::vector<double> segmentTime = split(_props[16 + _currentLevel]->GetString(), ',');
	int validSegment = (segmentTime.size() < _segmentCount) ? segmentTime.size() : _segmentCount;
	for (int i = 0; i < 9; i++) {
		if (i < validSegment)
			_segmentTime[_currentLevel - 1][i] = segmentTime[i] * 1000.0;
		else
			_segmentTime[_currentLevel - 1][i] = -1.0;
	}
}

void Segment::OnLoadObject(CKSTRING filename, BOOL isMap, CKSTRING masterName, CK_CLASSID filterClass,
                           BOOL addtoscene, BOOL reuseMeshes, BOOL reuseMaterials, BOOL dynamic,
                           XObjectArray* objArray, CKObject* masterObj) {
	if (!isMap)
		return;

	m_bml->GetArrayByName("CurrentLevel")->GetElementValue(0, 0, &_currentLevel);
	char buffer[BUF_SIZE];
	for (int i = 1; i <= 9; i++) {
		sprintf_s(buffer, "Sector_%02d", i);
		if (m_bml->GetGroupByName(buffer) == nullptr)
			break;

		_segmentCount = i;
	}
	
	for (double& i : _segmentTime[_currentLevel - 1])
		i = -1.0;
	
	if (!isCustomMap(filename)) {
		LoadRecordFromConfig();
	}
	for (int i = 0; i < _segmentCount; i++) {
		double time = -1 * _segmentTime[_currentLevel - 1][i] / 1000.0;
		if (time > 0.0) continue;
		if (time / 1000.0 <= 9999.999)
			sprintf(buffer, "%.3lfs", time);
		else
			strcpy(buffer, "-9999.999s");
		
		//sprintf(buffer, "%.3lfs", time);
		if (_useNativeFontRendering) {
			_labels[i][2]->SetText(buffer);
		} else {
			T_labels[i][2]->SetText(buffer);
		}
	}

	this->srTime = 0;
	if (_useNativeFontRendering)
		_title->SetVisible(_enabled);
	else
		T_title->SetVisible(_enabled);
	_panel->SetVisible(_enabled);
	_background->SetVisible(_enabled);
	_panel->SetPosition(Vx2DVector(0.0f, PANEL_INIT_Y_POS + static_cast<float>(segment) * PANEL_Y_SHIFT));
	for (int i = 0; i < _segmentCount; i++) {
		if (_useNativeFontRendering)
		{
			_labels[i][0]->SetVisible(_enabled);
			_labels[i][1]->SetVisible(_enabled);
			_labels[i][2]->SetVisible(_enabled);
		} else {
			T_labels[i][0]->SetVisible(_enabled);
			T_labels[i][1]->SetVisible(_enabled);
			T_labels[i][2]->SetVisible(_enabled);
		}
	}
	// for (int i = 0; i < _segmentCount; i++)
	// 	_segmentTime[i] = -1.0;
	/*for (int i = 1; i <= 9; i++) {
		if (_useNativeFontRendering) {
			_labels[i - 1][1]->SetText("----");
			_labels[i - 1][2]->SetText("----");
		} else {
			T_labels[i - 1][1]->SetText("----");
			T_labels[i - 1][2]->SetText("----");
		}
	}*/
	
	_background->SetSize(Vx2DVector(PANEL_WIDTH, (float) _segmentCount * PANEL_HEIGHT + PANEL_INIT_HEIGHT));
}

void Segment::OnPreExitLevel()
{
	std::string str = serialize();
	_props[16 + _currentLevel]->SetString(str.c_str());
}

void Segment::OnPreEndLevel()
{
	_segmentTime[_currentLevel - 1][segment] = srTime;
	segment++;
	_props[16 + _currentLevel]->SetString(serialize().c_str());
	
	_panel->SetVisible(false);
	this->counting = false;
	if (_useNativeFontRendering)
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

			if (_useNativeFontRendering) {
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
}

void Segment::OnStartLevel()
{
	if (m_bml->IsCheatEnabled()) {
		m_bml->SendIngameMessage("Cheat mode enabled. Segment will stop recording.");
		m_bml->SendIngameMessage("Disable cheat mode and restart this level to restore.");
	}

	this->counting = false;
	this->srTime = 0;
	this->segment = 0;

	_panel->SetVisible(_enabled);
	_panel->SetPosition(Vx2DVector(0.0f, PANEL_INIT_Y_POS + static_cast<float>(segment) * PANEL_Y_SHIFT));
}

void Segment::OnPreCheckpointReached()
{
	if (_segmentTime[_currentLevel - 1][segment] < 0.0 || srTime < _segmentTime[_currentLevel - 1][segment])
		if (!m_bml->IsCheatEnabled())
			_segmentTime[_currentLevel - 1][segment] = srTime;

	this->segment++;
	_panel->SetPosition(Vx2DVector(0.0f, PANEL_INIT_Y_POS + static_cast<float>(segment) * PANEL_Y_SHIFT));

	srTime = 0;
}