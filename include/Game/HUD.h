#ifndef GUI_HUD_H__
#define GUI_HUD_H__

#include "GUI/Frame.h"
#include "GUI/TextureFrame.h"
#include "GUI/NumberFrame.h"
#include "Core/EKeyUp.h"
#include "Game/EScoreEvent.h"
#include "Game/FPSCounter.h"

namespace dd
{
namespace GUI
{

class HUD : public Frame
{
public:
	HUD(Frame* parent, std::string name)
		: Frame(parent, name)
	{
		Width = parent->Width;
		Height = parent->Height;

		m_LevelIndicator = new GUI::TextureFrame(this, "HUDLevelIndicator");
		m_LevelIndicator->SetTexture("Textures/GUI/HUD/LevelIndicatorBG.png");

		m_A1 = new GUI::NumberFrame(m_LevelIndicator, "HUDScoreNumberFrameawdawdwa");
		m_A1->X = 40;
		m_A1->SetNumber(1);
		m_A2 = new GUI::NumberFrame(m_LevelIndicator, "HUDScoreNumberFrameawdawdwa");
		m_A2->X = 60;
		m_A2->SetNumber(1);
		//m_A3 = new GUI::NumberFrame(m_LevelIndicator, "HUDScoreNumberFrameawdawdwa");
		//m_A3->X = 80;
		//m_A3->SetNumber(123);

		m_ScoreIndicator = new GUI::TextureFrame(this, "HUDScoreIndicator");
		m_ScoreIndicator->SetTexture("Textures/GUI/HUD/ScoreIndicatorBG.png");
		m_ScoreIndicator->SetRight(Right());
		m_ScoreNumberFrame = new GUI::NumberFrame(m_ScoreIndicator, "HUDScoreNumberFrame");
		m_ScoreNumberFrame->X = 15;
		m_ScoreNumberFrame->Y = 21;

		//m_FPSCounter = new GUI::FPSCounter(this, "FPSCounter");

		EVENT_SUBSCRIBE_MEMBER(m_EScore, &HUD::OnScore);
	}

private:
	TextureFrame* m_LevelIndicator = nullptr;
	TextureFrame* m_ScoreIndicator = nullptr;
	NumberFrame* m_A1 = nullptr;
	NumberFrame* m_A2 = nullptr;
	//NumberFrame* m_A3 = nullptr;
	NumberFrame* m_ScoreNumberFrame = nullptr;
	FPSCounter* m_FPSCounter = nullptr;

	EventRelay<Frame, Events::ScoreEvent> m_EScore;

	bool OnScore(const Events::ScoreEvent& event)
	{
		m_ScoreNumberFrame->SetNumber(m_ScoreNumberFrame->Number() + event.Score);
	}
};

}
}
#endif

