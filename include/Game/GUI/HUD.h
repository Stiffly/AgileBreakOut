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

		m_ScoreBackground = new GUI::TextureFrame(this, "HUDScoreBackgroundFrame");
		m_ScoreBackground->SetTexture("Textures/GUI/Numbers/Background.png");
		m_ScoreNumberFrame = new GUI::NumberFrame(this, "HUDScoreNumberFrame");
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

		updateScore();
	}



private:
	TextureFrame* m_ScoreBackground = nullptr;
	NumberFrame* m_ScoreNumberFrame = nullptr;
	FPSCounter* m_FPSCounter = nullptr;
	TextureFrame* m_GameOverFrame = nullptr;
	TextureFrame* m_ClusterClearFrame = nullptr;
	Slider* m_KrakenAttackSlider = nullptr;

	EventRelay<Frame, Events::ScoreEvent> m_EScore;
	EventRelay<Frame, Events::GameOver> m_EGameOver;
	EventRelay<Frame, Events::ClusterClear> m_ClusterClear;
	EventRelay<Frame, Events::KrakenAttack> m_KrakenAttack;

	void updateScore()
	{
		m_ScoreNumberFrame->SetLeft(Width / 2.f - m_ScoreNumberFrame->Width / 2.f);
		m_ScoreBackground->SetLeft(m_ScoreNumberFrame->Left() - 20);
		m_ScoreBackground->Width = m_ScoreNumberFrame->Width + 40;
		m_ScoreBackground->Height = m_ScoreNumberFrame->Height + 30;
	}

	bool OnScore(const Events::ScoreEvent& event)
	{
		m_ScoreNumberFrame->SetNumber(m_ScoreNumberFrame->Number() + event.Score);
		updateScore();
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

