#include "Segment.h"
#include "CommandSeg.h"

IMod* BMLEntry(IBML* bml) {
	return new Segment(bml);
}

Segment::Segment(IBML* bml) : IMod(bml) {
	dutySlices_.push_back([&]() {
		if (srTime_ / 1000.0 <= 9999.999)
			sprintf(timeString_, "%.3fs", srTime_ / 1000.0);
		else
			strcpy(timeString_, "9999.999s");
	});
	dutySlices_.push_back([&]() {
		if (useNativeFontRendering_)
			labels_[segment_][1]->SetText(timeString_);
		else
			T_labels_[segment_][1]->SetText(timeString_);
	});
	dutySlices_.push_back([&]() {
		double currentTime = segmentTime_[currentLevel_ - 1][segment_];
		if (currentTime < 0.0) {
			panel_->SetColor(VxColor(EVEN_R, EVEN_G, EVEN_B, EVEN_A));
			return;
		}
		
		delta_ = srTime_ - currentTime;
		if (delta_ < 0.0)
			panel_->SetColor(VxColor(LEAD_R, LEAD_G, LEAD_B, LEAD_A));
		else if (delta_ == 0.0)
			panel_->SetColor(VxColor(EVEN_R, EVEN_G, EVEN_B, EVEN_A));
		else
			panel_->SetColor(VxColor(LAG_R, LAG_G, LAG_B, LAG_A));
		
	});
	dutySlices_.push_back([&]() {
		if (delta_ / 1000.0 <= 9999.999 && delta_ / 1000.0 >= -9999.999)
			sprintf(deltaString_, "%+.3fs", delta_ / 1000.0f);
		else if (delta_ / 1000.0 > 9999.999)
			strcpy(deltaString_, "+9999.999s");
		else
			strcpy(deltaString_, "-9999.999s");
	});
	dutySlices_.push_back([&]() {
		if (segmentTime_[currentLevel_ - 1][segment_] > 0.0) {
			if (useNativeFontRendering_)
				labels_[segment_][2]->SetText(deltaString_);
			else
				T_labels_[segment_][2]->SetText(deltaString_);
		}
	});
}

void Segment::RefreshConfig() {
	enabled_ = props_[0]->GetBoolean();
	skipEnabled_ = props_[1]->GetBoolean();
	skipStep_ = props_[2]->GetInteger();
	sscanf(props_[3]->GetString(), "%d,%d,%d,%d", &LEAD_R, &LEAD_G, &LEAD_B, &LEAD_A);
	sscanf(props_[4]->GetString(), "%d,%d,%d,%d", &EVEN_R, &EVEN_G, &EVEN_B, &EVEN_A);
	sscanf(props_[5]->GetString(), "%d,%d,%d,%d", &LAG_R, &LAG_G, &LAG_B, &LAG_A);
	useNativeFontRendering_ = props_[6]->GetBoolean();
	strcpy(TITLE_FONT, props_[7]->GetString());
	TITLE_FONT_SIZE = props_[8]->GetInteger();
	TITLE_FONT_WEIGHT = props_[9]->GetInteger();
	TITLE_ITALIC = props_[10]->GetBoolean();
	TITLE_UNDERLINE = props_[11]->GetBoolean();
	strcpy(ITEM_FONT, props_[12]->GetString());
	ITEM_FONT_SIZE = props_[13]->GetInteger();
	ITEM_FONT_WEIGHT = props_[14]->GetInteger();
	ITEM_ITALIC = props_[15]->GetBoolean();
	ITEM_UNDERLINE = props_[16]->GetBoolean();
}

void Segment::InitGui()
{
	gui_ = new BGui::Gui;
	panel_ = gui_->AddPanel("M_Segment_Highlight", VxColor(EVEN_R, EVEN_G, EVEN_B, EVEN_A), TITLE_X_POS, PANEL_INIT_Y_POS, PANEL_WIDTH, PANEL_HEIGHT);
	panel_->SetZOrder(0);
	panel_->SetVisible(false);
	background_ = gui_->AddPanel("M_Segment_Background", VxColor(0, 0, 0, 50), TITLE_X_POS, TITLE_Y_POS, PANEL_WIDTH, PANEL_HEIGHT);
	background_->SetZOrder(-1);
	background_->SetVisible(false);

	if (useNativeFontRendering_)
	{
		title_ = gui_->AddTextLabel("M_Segment_Title", "Segments:", ExecuteBB::GAMEFONT_01, TITLE_X_POS, TITLE_Y_POS, 0.2f, 0.03f);
		title_->SetZOrder(10);
		title_->SetAlignment(ALIGN_LEFT);
		title_->SetVisible(false);
	} else {
		T_title_ = gui_->AddText("M_Segment_Title", "Segments:", TITLE_X_POS, TITLE_Y_POS, 0.2f, 0.03f);
		T_title_->SetZOrder(10);
		T_title_->SetFont(TITLE_FONT, TITLE_FONT_SIZE, TITLE_FONT_WEIGHT, TITLE_ITALIC, TITLE_UNDERLINE);
		T_title_->SetAlignment(CKSPRITETEXT_LEFT);
		T_title_->SetVisible(false);
	}
	
	char labelText[5] = "#1: ";
	for (int i = 1; i <= 9; i++) {
		labelText[1] = i + '0';
		if (useNativeFontRendering_) {
			labels_[i - 1][0] = gui_->AddTextLabel("M_Segment_Seg", labelText, ExecuteBB::GAMEFONT_01, TITLE_X_POS, TITLE_Y_POS + TITLE_Y_SHIFT + (float)i * ITEM_Y_SHIFT, 0.2f, 0.03f);
			labels_[i - 1][0]->SetAlignment(ALIGN_LEFT);
			labels_[i - 1][0]->SetVisible(false);
			labels_[i - 1][1] = gui_->AddTextLabel("M_Segment_Time", "----", ExecuteBB::GAMEFONT_01, TITLE_X_POS + TITLE_X_SHIFT + 0.0f * ITEM_X_SHIFT, TITLE_Y_POS + TITLE_Y_SHIFT + (float)i * ITEM_Y_SHIFT, 0.2f, 0.03f);
			labels_[i - 1][1]->SetAlignment(ALIGN_CENTER);
			labels_[i - 1][1]->SetVisible(false);
			labels_[i - 1][2] = gui_->AddTextLabel("M_Segment_Score", "----", ExecuteBB::GAMEFONT_01, TITLE_X_POS + TITLE_X_SHIFT + 1.0f * ITEM_X_SHIFT, TITLE_Y_POS + TITLE_Y_SHIFT + (float)i * ITEM_Y_SHIFT, 0.2f, 0.03f);
			labels_[i - 1][2]->SetAlignment(ALIGN_CENTER);
			labels_[i - 1][2]->SetVisible(false);
		} else {
			T_labels_[i - 1][0] = gui_->AddText("M_Segment_Seg", labelText, TITLE_X_POS, TITLE_Y_POS + TITLE_Y_SHIFT + (float)i * ITEM_Y_SHIFT, 0.2f, 0.03f);
			T_labels_[i - 1][0]->SetAlignment(CKSPRITETEXT_LEFT);
			T_labels_[i - 1][0]->SetFont(ITEM_FONT, ITEM_FONT_SIZE, ITEM_FONT_WEIGHT, ITEM_ITALIC, ITEM_UNDERLINE);
			T_labels_[i - 1][0]->SetVisible(false);
			T_labels_[i - 1][1] = gui_->AddText("M_Segment_Time", "----", TITLE_X_POS + TITLE_X_SHIFT + 0.0f * ITEM_X_SHIFT, TITLE_Y_POS + TITLE_Y_SHIFT + (float)i * ITEM_Y_SHIFT, 0.2f, 0.03f);
			T_labels_[i - 1][1]->SetAlignment(CKSPRITETEXT_CENTER);
			T_labels_[i - 1][1]->SetFont(ITEM_FONT, ITEM_FONT_SIZE, ITEM_FONT_WEIGHT, ITEM_ITALIC, ITEM_UNDERLINE);
			T_labels_[i - 1][1]->SetVisible(false);
			T_labels_[i - 1][2] = gui_->AddText("M_Segment_Score", "----", TITLE_X_POS + TITLE_X_SHIFT + 1.0f * ITEM_X_SHIFT, TITLE_Y_POS + TITLE_Y_SHIFT + (float)i * ITEM_Y_SHIFT, 0.2f, 0.03f);
			T_labels_[i - 1][2]->SetAlignment(CKSPRITETEXT_CENTER);
			T_labels_[i - 1][2]->SetFont(ITEM_FONT, ITEM_FONT_SIZE, ITEM_FONT_WEIGHT, ITEM_ITALIC, ITEM_UNDERLINE);
			T_labels_[i - 1][2]->SetVisible(false);
		}
	}
}

void Segment::OnLoad() {
	GetConfig()->SetCategoryComment("General", "General Settings");
	props_[0] = GetConfig()->GetProperty("General", "Enabled?");
	props_[0]->SetComment("Enable Segment");
	props_[0]->SetDefaultBoolean(true);

	GetConfig()->SetCategoryComment("Tweaking", "Performance Tweaking");
	props_[1] = GetConfig()->GetProperty("Tweaking", "SkipFrames?");
	props_[1]->SetComment("Enable frame skipping feature to optimize performance");
	props_[1]->SetDefaultBoolean(false);
	props_[2] = GetConfig()->GetProperty("Tweaking", "Skip_Step");
	props_[2]->SetComment("Skip GUI process every (Skip_Step):1 frame");
	props_[2]->SetDefaultInteger(240);
	props_[6] = GetConfig()->GetProperty("Tweaking", "NativeFontRendering");
	props_[6]->SetComment("");
	props_[6]->SetDefaultBoolean(true);

	GetConfig()->SetCategoryComment("Customize", "Appearance Customize");
	props_[3] = GetConfig()->GetProperty("Customize", "Lead_Color");
	props_[3]->SetComment("Leading highlight color, in r,g,b,a format. Default: 50,205,50,200");
	props_[3]->SetDefaultString("50,205,50,200");
	props_[4] = GetConfig()->GetProperty("Customize", "Even_Color");
	props_[4]->SetComment("Leading or default highlight color, in r,g,b,a format. Default: 255,168,0,200");
	props_[4]->SetDefaultString("255,168,0,200");
	props_[5] = GetConfig()->GetProperty("Customize", "Lag_Color");
	props_[5]->SetComment("Lagging highlight color, in r,g,b,a format. Default: 220,20,60,200");
	props_[5]->SetDefaultString("220,20,60,200");
	props_[7] = GetConfig()->GetProperty("Customize", "Title_Font");
	props_[7]->SetComment("");
	props_[7]->SetDefaultString("Bank Gothic");
	props_[8] = GetConfig()->GetProperty("Customize", "Title_Font_Size");
	props_[8]->SetComment("");
	props_[8]->SetDefaultInteger(20);
	props_[9] = GetConfig()->GetProperty("Customize", "Title_Font_Weight");
	props_[9]->SetComment("");
	props_[9]->SetDefaultInteger(500);
	props_[10] = GetConfig()->GetProperty("Customize", "Title_Italic");
	props_[10]->SetComment("");
	props_[10]->SetDefaultBoolean(false);
	props_[11] = GetConfig()->GetProperty("Customize", "Title_Underline");
	props_[11]->SetComment("");
	props_[11]->SetDefaultBoolean(false);
	props_[12] = GetConfig()->GetProperty("Customize", "Item_Font");
	props_[12]->SetComment("");
	props_[12]->SetDefaultString("Bank Gothic");
	props_[13] = GetConfig()->GetProperty("Customize", "Item_Font_Size");
	props_[13]->SetComment("");
	props_[13]->SetDefaultInteger(20);
	props_[14] = GetConfig()->GetProperty("Customize", "Item_Font_Weight");
	props_[14]->SetComment("");
	props_[14]->SetDefaultInteger(500);
	props_[15] = GetConfig()->GetProperty("Customize", "Item_Italic");
	props_[15]->SetComment("");
	props_[15]->SetDefaultBoolean(false);
	props_[16] = GetConfig()->GetProperty("Customize", "Item_Underline");
	props_[16]->SetComment("");
	props_[16]->SetDefaultBoolean(false);

	GetConfig()->SetCategoryComment("Record", "Record Management");
	char buffer[BUF_SIZE];
	for (int i = 1; i <= 13; i++)
	{
		sprintf(buffer, "Level_%02d", i);
		props_[16 + i] = GetConfig()->GetProperty("Record", buffer);
		props_[16 + i]->SetComment("");
		props_[16 + i]->SetDefaultString("");
	}
	
	RefreshConfig();
	InitGui();
	m_bml->RegisterCommand(new CommandSeg(this));
}

void Segment::OnModifyConfig(CKSTRING category, CKSTRING key, IProperty* prop) {
	RefreshConfig();
	if (m_bml->IsIngame())
		LoadRecordFromConfig();

	if (prop == props_[0] && m_bml->IsIngame()) {
		if (useNativeFontRendering_)
			title_->SetVisible(enabled_);
		else
			T_title_->SetVisible(enabled_);
		panel_->SetVisible(enabled_);
		background_->SetVisible(enabled_);
		for (int i = 0; i < segmentCount_; i++) {
			if (useNativeFontRendering_) {
				labels_[i][0]->SetVisible(enabled_);
				labels_[i][1]->SetVisible(enabled_);
				labels_[i][2]->SetVisible(enabled_);
			} else {
				T_labels_[i][0]->SetVisible(enabled_);
				T_labels_[i][1]->SetVisible(enabled_);
				T_labels_[i][2]->SetVisible(enabled_);
			}
		}
	} 
	
	if (prop == props_[6]) {
		delete title_;
		delete T_title_;
		title_ = nullptr;
		T_title_ = nullptr;
		delete panel_;
		delete background_;

		if (useNativeFontRendering_) {
			for (int j = 0; j < 9; j++) {
				for (int k = 0; k < 3; k++) {
					if (k >= 1)
						strcpy(text[j][k - 1], T_labels_[j][k]->GetText());
					delete T_labels_[j][k];
				}
			}
		} else {
			for (int j = 0; j < 9; j++) {
				for (int k = 0; k < 3; k++) {
					if (k >= 1)
						strcpy(text[j][k - 1], labels_[j][k]->GetText());
					delete labels_[j][k];
				}
			}
		}
		delete gui_;
		InitGui();
		if (useNativeFontRendering_) {
			for (int j = 0; j < 9; j++) {
				for (int k = 1; k < 3; k++) {
					labels_[j][k]->SetText(text[j][k - 1]);
				}
			}
		}
		else {
			for (int j = 0; j < 9; j++) {
				for (int k = 1; k < 3; k++) {
					T_labels_[j][k]->SetText(text[j][k - 1]);
				}
			}
		}
		panel_->SetVisible(enabled_ && m_bml->IsIngame());
		panel_->SetPosition(Vx2DVector(0.0f, PANEL_INIT_Y_POS + static_cast<float>(segment_) * PANEL_Y_SHIFT));
		background_->SetVisible(enabled_ && m_bml->IsIngame());
		background_->SetSize(Vx2DVector(PANEL_WIDTH, (float)segmentCount_ * PANEL_HEIGHT + PANEL_INIT_HEIGHT));
		for (int i = 0; i < segmentCount_; i++)
		{
			if (useNativeFontRendering_) {
				title_->SetVisible(enabled_ && m_bml->IsIngame());
				for (auto* j: labels_[i])
				{
					j->SetVisible(enabled_ && m_bml->IsIngame());
				}
			} else {
				T_title_->SetVisible(enabled_ && m_bml->IsIngame());
				for (auto* j : T_labels_[i])
				{
					j->SetVisible(enabled_ && m_bml->IsIngame());
				}
			}
		}
	}

	for (int i = 7; i <= 11; i++) {
		if (useNativeFontRendering_)
			break;
		if (prop == props_[i]) {
			T_title_->SetFont(TITLE_FONT, TITLE_FONT_SIZE, TITLE_FONT_WEIGHT, TITLE_ITALIC, TITLE_UNDERLINE);
			break;
		}
	}

	for (int i = 12; i <= 16; i++) {
		if (useNativeFontRendering_)
			break;
		if (prop == props_[i]) {
			for (auto& j: T_labels_) {
				for (auto& k : j)
					k->SetFont(ITEM_FONT, ITEM_FONT_SIZE, ITEM_FONT_WEIGHT, ITEM_ITALIC, ITEM_UNDERLINE);
			}
			break;
		}
	}
}

void Segment::OnPreStartMenu() {
	if (panel_ != nullptr)
		panel_->SetVisible(false);
	if (background_ != nullptr)
		background_->SetVisible(false);
	if (useNativeFontRendering_)
		title_->SetVisible(false);
	else
		T_title_->SetVisible(false);
	for (int i = 1; i <= 9; i++) {
		if (useNativeFontRendering_)
		{
			labels_[i - 1][0]->SetVisible(false);
			labels_[i - 1][1]->SetVisible(false);
			labels_[i - 1][2]->SetVisible(false);
		} else {
			T_labels_[i - 1][0]->SetVisible(false);
			T_labels_[i - 1][1]->SetVisible(false);
			T_labels_[i - 1][2]->SetVisible(false);
		}
	}
}

void Segment::ClearRecord()
{
	for (int i = 1; i <= 9; i++) {
		segmentTime_[currentLevel_ - 1][i - 1] = -1;

		if (useNativeFontRendering_) {
			labels_[i - 1][2]->SetText("----");
		} else {
			T_labels_[i - 1][2]->SetText("----");
		}
	}
}

void Segment::LoadRecordFromConfig()
{
	std::vector<double> segmentTime = split(props_[16 + currentLevel_]->GetString(), ',');
	int validSegment = (segmentTime.size() < segmentCount_) ? segmentTime.size() : segmentCount_;
	for (int i = 0; i < 9; i++) {
		if (i < validSegment)
			segmentTime_[currentLevel_ - 1][i] = segmentTime[i] * 1000.0;
		else
			segmentTime_[currentLevel_ - 1][i] = -1.0;
	}
}

void Segment::OnLoadObject(CKSTRING filename, BOOL isMap, CKSTRING masterName, CK_CLASSID filterClass,
                           BOOL addtoscene, BOOL reuseMeshes, BOOL reuseMaterials, BOOL dynamic,
                           XObjectArray* objArray, CKObject* masterObj) {
	if (!isMap)
		return;

	m_bml->GetArrayByName("CurrentLevel")->GetElementValue(0, 0, &currentLevel_);
	char buffer[BUF_SIZE];
	for (int i = 1; i <= 9; i++) {
		sprintf_s(buffer, "Sector_%02d", i);
		if (m_bml->GetGroupByName(buffer) == nullptr)
			break;

		segmentCount_ = i;
	}
	
	for (double& i : segmentTime_[currentLevel_ - 1])
		i = -1.0;

	this->srTime_ = 0;
	if (useNativeFontRendering_)
		title_->SetVisible(enabled_);
	else
		T_title_->SetVisible(enabled_);
	panel_->SetVisible(enabled_);
	background_->SetVisible(enabled_);
	panel_->SetPosition(Vx2DVector(0.0f, PANEL_INIT_Y_POS + static_cast<float>(segment_) * PANEL_Y_SHIFT));
	for (int i = 0; i < segmentCount_; i++) {
		if (useNativeFontRendering_)
		{
			labels_[i][0]->SetVisible(enabled_);
			labels_[i][1]->SetVisible(enabled_);
			labels_[i][2]->SetVisible(enabled_);
		} else {
			T_labels_[i][0]->SetVisible(enabled_);
			T_labels_[i][1]->SetVisible(enabled_);
			T_labels_[i][2]->SetVisible(enabled_);
		}
	}
	// for (int i = 0; i < _segmentCount; i++)
	// 	_segmentTime[i] = -1.0;
	for (int i = 1; i <= 9; i++) {
		if (useNativeFontRendering_) {
			labels_[i - 1][1]->SetText("----");
			labels_[i - 1][2]->SetText("----");
		} else {
			T_labels_[i - 1][1]->SetText("----");
			T_labels_[i - 1][2]->SetText("----");
		}
	}
	
	background_->SetSize(Vx2DVector(PANEL_WIDTH, (float) segmentCount_ * PANEL_HEIGHT + PANEL_INIT_HEIGHT));

	if (!isCustomMap(filename)) {
		LoadRecordFromConfig();
	}
	for (int i = 0; i < segmentCount_; i++) {
		double time = -1.0 * segmentTime_[currentLevel_ - 1][i] / 1000.0;
		if (time > 0.0) continue;
		if (time <= 9999.999)
			sprintf(buffer, "%.3lfs", time);
		else
			strcpy(buffer, "-9999.999s");

		//sprintf(buffer, "%.3lfs", time);
		if (useNativeFontRendering_) {
			labels_[i][2]->SetText(buffer);
		}
		else {
			T_labels_[i][2]->SetText(buffer);
		}
	}
}

void Segment::OnPreExitLevel()
{
	std::string str = serialize();
	props_[16 + currentLevel_]->SetString(str.c_str());
}

void Segment::OnCheatEnabled(bool enable)
{
	if (enable) {
		m_bml->SendIngameMessage("Cheat mode enabled. Segment will stop recording.");
		m_bml->SendIngameMessage("Disable cheat mode to restore.");
	} else {
		m_bml->SendIngameMessage("Cheat mode disabled. Segment will now start recording.");
	}
}

void Segment::OnPreEndLevel()
{
	segmentTime_[currentLevel_ - 1][segment_] = srTime_;
	segment_++;
	props_[16 + currentLevel_]->SetString(serialize().c_str());
	
	panel_->SetVisible(false);
	this->counting_ = false;
	if (useNativeFontRendering_)
		gui_->Process();
}

void Segment::OnCounterActive()
{
	this->counting_ = true;
}

void Segment::OnCounterInactive()
{	
	this->counting_ = false;
}

void Segment::OnPauseLevel()
{
	this->counting_ = false;
}

void Segment::OnUnpauseLevel()
{
	this->counting_ = true;
}

void Segment::OnProcess()
{
	loopCount_++;

	if (this->counting_)
		this->srTime_ += static_cast<double>(m_bml->GetTimeManager()->GetLastDeltaTime());
	

	if (enabled_) {
		if (m_bml->IsIngame()) {
			dutySlices_[loopCount_ % dutySlices_.size()]();

			if (useNativeFontRendering_) {
				if (loopCount_ % skipStep_ != 0 || !skipEnabled_) {
					title_->Process();
					for (int i = 0; i < segmentCount_; i++) {
						labels_[i][0]->Process();
						labels_[i][1]->Process();
						labels_[i][2]->Process();
					}
				}
			}
		}
	}
}

void Segment::OnStartLevel()
{
	this->counting_ = false;
	this->srTime_ = 0;
	this->segment_ = 0;

	panel_->SetVisible(enabled_);
	panel_->SetPosition(Vx2DVector(0.0f, PANEL_INIT_Y_POS + static_cast<float>(segment_) * PANEL_Y_SHIFT));
}

void Segment::OnPreCheckpointReached()
{
	for (auto& dutySlice : dutySlices_)
		dutySlice(); // Refreshes last segment on checkpoint reached. Excluding delta cell.(aka. second column)

	if (segmentTime_[currentLevel_ - 1][segment_] < 0.0 || srTime_ < segmentTime_[currentLevel_ - 1][segment_])
		if (!m_bml->IsCheatEnabled())
			segmentTime_[currentLevel_ - 1][segment_] = srTime_;

	this->segment_++;
	panel_->SetPosition(Vx2DVector(0.0f, PANEL_INIT_Y_POS + static_cast<float>(segment_) * PANEL_Y_SHIFT));

	srTime_ = 0;
}