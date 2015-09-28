#ifndef GUI_MAINMENUMAIN_H__
#define GUI_MAINMENUMAIN_H__

#include "GUI/TextureFrame.h"
#include "GUI/Button.h"
#include "Game/EGameStart.h"
#include "Game/HUD.h"

namespace dd
{
namespace GUI
{

class MainMenuMain : public TextureFrame
{
public:
	MainMenuMain(Frame* parent, std::string name)
		: TextureFrame(parent, name)
	{
		Width = 409;
		Height = 543;

		m_Title = new GUI::TextureFrame(this, "MainMenuMainTitle");
		m_Title->SetTexture("Textures/GUI/Menu/MainTitle.png");

		m_SquidoutButton = new GUI::Button(this, "MainMenuMainSquidoutButton");
		m_SquidoutButton->SetTop(m_Title->Bottom());
		m_SquidoutButton->SetTextureReleased("Textures/GUI/Menu/MainSquidout.png");
		m_SquidoutButton->SetTextureHover("Textures/GUI/Menu/MainSquidoutHover.png");
		m_SquidoutButton->SetTexturePressed("Textures/GUI/Menu/MainSquidoutClick.png");
		m_SquidoutButton->SizeToTexture();

		m_TimeAttackButton = new GUI::Button(this, "MainMenuMainTimeAttackButton");
		m_TimeAttackButton->SetTop(m_SquidoutButton->Bottom());
		m_TimeAttackButton->SetTextureReleased("Textures/GUI/Menu/MainTimeAttack.png");
		m_TimeAttackButton->SetTextureHover("Textures/GUI/Menu/MainTimeAttackHover.png");
		m_TimeAttackButton->SetTexturePressed("Textures/GUI/Menu/MainTimeAttackClick.png");
		m_TimeAttackButton->SizeToTexture();

		m_OptionsButton = new GUI::Button(this, "MainMenuMainOptionsButton");
		m_OptionsButton->SetTop(m_TimeAttackButton->Bottom());
		m_OptionsButton->SetTextureReleased("Textures/GUI/Menu/MainOptions.png");
		m_OptionsButton->SetTextureHover("Textures/GUI/Menu/MainOptionsHover.png");
		m_OptionsButton->SetTexturePressed("Textures/GUI/Menu/MainOptionsClick.png");
		m_OptionsButton->SizeToTexture();

		EVENT_SUBSCRIBE_MEMBER(m_EButtonRelease, &MainMenuMain::OnButtonRelease);
	}

private:
	TextureFrame* m_Title;
	Button* m_SquidoutButton;
	Button* m_TimeAttackButton;
	Button* m_OptionsButton;

	EventRelay<Frame, Events::ButtonRelease> m_EButtonRelease;
	virtual bool OnButtonRelease(const Events::ButtonRelease& event)
	{
		if (event.Button == m_SquidoutButton) {
			Events::GameStart e;
			EventBroker->Publish(e);
			m_Parent->Hide();
			auto hud = new GUI::HUD(BaseFrame, "HUD");
		}

		return true;
	}
};

}
}
#endif
