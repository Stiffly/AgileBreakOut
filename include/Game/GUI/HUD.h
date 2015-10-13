#ifndef GUI_HUD_H__
#define GUI_HUD_H__

#include "GUI/Frame.h"
#include "GUI/TextureFrame.h"
#include "GUI/NumberFrame.h"
#include "Core/EKeyUp.h"
#include "Game/EScoreEvent.h"
#include "Game/GUI/FPSCounter.h"
#include "Game/EGameOver.h"
#include "Game/EClusterClear.h"
#include "Game/EKrakenAttack.h"
#include "GUI/Slider.h"

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

		//m_LevelIndicator = new GUI::TextureFrame(this, "HUDLevelIndicator");
		//m_LevelIndicator->SetTexture("Textures/GUI/HUD/LevelIndicatorBG.png");

		//m_AreaNumberFrame = new GUI::NumberFrame(m_LevelIndicator, "HUDScoreNumberFrameawdawdwa");
		//m_AreaNumberFrame->X = 132;
		//m_AreaNumberFrame->Y = 19;
		//m_AreaNumberFrame->SetNumber(6);
		//m_StageNumberFrame = new GUI::NumberFrame(m_LevelIndicator, "HUDScoreNumberFrameawdawdwa");
		//m_StageNumberFrame->X = 165;
		//m_StageNumberFrame->Y = 19;
		//m_StageNumberFrame->SetNumber(5);

		//m_ScoreIndicator = new GUI::TextureFrame(this, "HUDScoreIndicator");
		//m_ScoreIndicator->SetTexture("Textures/GUI/HUD/ScoreIndicatorBG.png");
		m_ScoreNumberFrame = new GUI::NumberFrame(this, "HUDScoreNumberFrame");
		m_ScoreNumberFrame->SetTexture("Textures/GUI/Numbers/Background.png");
		//m_ScoreNumberFrame->X = 15;
		m_ScoreNumberFrame->Y = 18;

		m_FPSCounter = new GUI::FPSCounter(this, "FPSCounter");

		m_KrakenAttackSlider = new Slider(this, "KrakenSlider");
		m_KrakenAttackSlider->SetTexture("Textures/GUI/Menu/SliderBackground.png");
		m_KrakenAttackSlider->SetTextureReleased("Textures/GUI/Menu/SliderHandle.png");
		m_KrakenAttackSlider->SetLeft(Width / 2 - m_KrakenAttackSlider->Width / 2);
		m_KrakenAttackSlider->Y = 800;
		m_KrakenAttackSlider->Hide();

		EVENT_SUBSCRIBE_MEMBER(m_EScore, &HUD::OnScore);
		EVENT_SUBSCRIBE_MEMBER(m_EGameOver, &HUD::OnGameOver);
		EVENT_SUBSCRIBE_MEMBER(m_ClusterClear, &HUD::OnClusterClear);
		EVENT_SUBSCRIBE_MEMBER(m_KrakenAttack, &HUD::OnKrakenAttack);
	}

	void Update(double dt) override
	{
		m_ScoreNumberFrame->SetLeft(Width / 2.f - m_ScoreNumberFrame->Width / 2.f);
	}

private:
	TextureFrame* m_LevelIndicator = nullptr;
	TextureFrame* m_ScoreIndicator = nullptr;
	NumberFrame* m_AreaNumberFrame = nullptr;
	NumberFrame* m_StageNumberFrame = nullptr;
	NumberFrame* m_ScoreNumberFrame = nullptr;
	FPSCounter* m_FPSCounter = nullptr;
	TextureFrame* m_GameOverFrame = nullptr;
	TextureFrame* m_ClusterClearFrame = nullptr;
	Slider* m_KrakenAttackSlider = nullptr;

	EventRelay<Frame, Events::ScoreEvent> m_EScore;
	EventRelay<Frame, Events::GameOver> m_EGameOver;
	EventRelay<Frame, Events::ClusterClear> m_ClusterClear;
	EventRelay<Frame, Events::KrakenAttack> m_KrakenAttack;

	bool OnScore(const Events::ScoreEvent& event)
	{
		m_ScoreNumberFrame->SetNumber(m_ScoreNumberFrame->Number() + event.Score);
		return true;
	}

	bool OnGameOver(const Events::GameOver& event)
	{
		m_GameOverFrame = new GUI::TextureFrame(this, "HUDGameOverFrame");
		m_GameOverFrame->SetTexture("Textures/GUI/HUD/GameOverScreen.png");
		return true;
	}

	bool OnClusterClear(const Events::ClusterClear& event)
	{
		m_ClusterClearFrame = new GUI::TextureFrame(this, "HUDGameOverFrame");
		m_ClusterClearFrame->SetTexture("Textures/GUI/HUD/WinScreen.png");
		return true;
	}

	bool OnKrakenAttack(const Events::KrakenAttack& event)
	{
		
		if (event.ChargeUpdate >= 1.f) {
			m_KrakenAttackSlider->Hide();
		} else {
			m_KrakenAttackSlider->Show();
			m_KrakenAttackSlider->SetPercentage(event.ChargeUpdate);
		}
		
		return true;
	}
};

}
}
#endif

