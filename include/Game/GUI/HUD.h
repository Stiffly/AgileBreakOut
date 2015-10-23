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

		m_ScoreBackMid = new GUI::TextureFrame(this, "HUDScoreBackgroundFrame");
		m_ScoreBackMid->SetTexture("Textures/GUI/Numbers/ScoreBackgroundMid.png");

		m_ScoreBackLeft = new GUI::TextureFrame(this, "HUDScoreBackgroundLeftFrame");
		m_ScoreBackLeft->SetTexture("Textures/GUI/Numbers/ScoreBackgroundLeft.png");

		m_ScoreBackRight = new GUI::TextureFrame(this, "HUDScoreBackgroundRightFrame");
		m_ScoreBackRight->SetTexture("Textures/GUI/Numbers/ScoreBackgroundRight.png");
		m_ScoreNumberFrame = new GUI::NumberFrame(this, "HUDScoreNumberFrame");
		//m_ScoreNumberFrame->X = 15;
		m_ScoreNumberFrame->Y = 18;

		m_FPSCounter = new GUI::FPSCounter(this, "FPSCounter");

		m_KrakenAttackSlider = new Slider(this, "KrakenSlider");
		m_KrakenAttackSlider->SetTexture("Textures/GUI/KrakenSlider/KrakenArmSliderBackground.png");
		m_KrakenAttackSlider->SetTextureReleased("Textures/GUI/KrakenSlider/SpaceSpam.png");
		m_KrakenAttackSlider->SetTexturePressed("Textures/GUI/KrakenSlider/SpaceHit.png");

		m_KrakenAttackSlider->SetLeft(Width / 2 - m_KrakenAttackSlider->Width / 2);
		m_KrakenAttackSlider->Y = 800;
		m_KrakenAttackSlider->Hide();

		EVENT_SUBSCRIBE_MEMBER(m_EScore, &HUD::OnScore);
		EVENT_SUBSCRIBE_MEMBER(m_EGameOver, &HUD::OnGameOver);
		EVENT_SUBSCRIBE_MEMBER(m_ClusterClear, &HUD::OnClusterClear);
		EVENT_SUBSCRIBE_MEMBER(m_KrakenAttack, &HUD::OnKrakenAttack);
		EVENT_SUBSCRIBE_MEMBER(m_KrakenAttackEnd, &HUD::OnKrakenAttackEnd);

		updateScore();
	}



private:
	TextureFrame* m_ScoreBackMid = nullptr;
	TextureFrame* m_ScoreBackLeft = nullptr;
	TextureFrame* m_ScoreBackRight = nullptr;
	NumberFrame* m_ScoreNumberFrame = nullptr;
	FPSCounter* m_FPSCounter = nullptr;
	TextureFrame* m_GameOverFrame = nullptr;
	TextureFrame* m_ClusterClearFrame = nullptr;
	Slider* m_KrakenAttackSlider = nullptr;

	EventRelay<Frame, Events::ScoreEvent> m_EScore;
	EventRelay<Frame, Events::GameOver> m_EGameOver;
	EventRelay<Frame, Events::ClusterClear> m_ClusterClear;
	EventRelay<Frame, Events::KrakenAttack> m_KrakenAttack;
	EventRelay<Frame, Events::KrakenAttackEnd> m_KrakenAttackEnd;

	void updateScore()
	{
		m_ScoreNumberFrame->SetLeft(Width / 2.f - m_ScoreNumberFrame->Width / 2.f);

		m_ScoreBackMid->Width = m_ScoreNumberFrame->Width;
		m_ScoreBackMid->Height = m_ScoreNumberFrame->Height + 35;
		m_ScoreBackMid->SetLeft(m_ScoreNumberFrame->Left() + 4);

		//m_ScoreBackLeft->Width = 40;
		m_ScoreBackLeft->Height = m_ScoreBackMid->Height;
		m_ScoreBackLeft->SetRight(m_ScoreBackMid->Left());

		//m_ScoreBackRight->Width = 40;
		m_ScoreBackRight->Height = m_ScoreBackMid->Height;
		m_ScoreBackRight->SetLeft(m_ScoreBackMid->Right());
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

	bool OnKrakenAttackEnd(const Events::KrakenAttackEnd& event)
	{
		m_KrakenAttackSlider->Hide();

		return true;
	}
};

}
}
#endif

