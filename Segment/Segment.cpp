#include "Segment.h"
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
	if (this->counting)
		this->srTime += static_cast<double>(m_bml->GetTimeManager()->GetLastDeltaTime());
}

void Segment::OnStartLevel()
{
	this->_energy = m_bml->GetArrayByName("Energy");
	this->_currentLevel = m_bml->GetArrayByName("CurrentLevel");
	int currentLevel;
	this->_currentLevel->GetElementValue(0, 0, &currentLevel);
	std::stringstream ss;
	ss << "Starting level " << currentLevel << "...";
	m_bml->SendIngameMessage(ss.str().c_str());
	this->counting = false;
	this->srTime = 0;
}

void Segment::OnPreCheckpointReached()
{
	_energy->GetElementValue(0, 0, &(this->points));
	++this->segment;
	Print(m_bml);
}
