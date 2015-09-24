#ifndef GUI_MAINMENUOPTIONS_H__
#define GUI_MAINMENUOPTIONS_H__

#include "GUI/TextureFrame.h"
#include "GUI/Button.h"
#include "GUI/Slider.h"

namespace dd
{
namespace GUI
{

class MainMenuOptions : public TextureFrame
{
public:
	MainMenuOptions(Frame* parent, std::string name)
		: TextureFrame(parent, name)
	{
		Width = 409;
		Height = 1080;

		m_Title = new GUI::TextureFrame(this, "MainMenuOptionsTitle");
		m_Title->SetTexture("Textures/GUI/Menu/OptionsTitle.png");

		m_TitleSFX = new GUI::TextureFrame(this, "MainMenuOptionsSFXTitle");
		m_TitleSFX->SetTop(m_Title->Bottom() + 20);
		m_TitleSFX->SetTexture("Textures/GUI/Menu/OptionsSFXTitle.png");
		m_SFXSlider = new GUI::Slider(this, "MainMenuOptionsSFXSlider");
		m_SFXSlider->X = 66;
		m_SFXSlider->SetTop(m_TitleSFX->Bottom());
		m_SFXSlider->SetTexture("Textures/GUI/Menu/SliderBackground.png");
		m_SFXSlider->SetTextureReleased("Textures/GUI/Menu/SliderHandle.png");
		m_SFXSlider->CenterSlider();

		m_TitleBGM = new GUI::TextureFrame(this, "MainMenuOptionsBGMTitle");
		m_TitleBGM->SetTop(m_TitleSFX->Bottom());
		m_TitleBGM->SetTexture("Textures/GUI/Menu/OptionsBGMTitle.png");
	}

private:
	TextureFrame* m_Title;
	TextureFrame* m_TitleSFX;
	Slider* m_SFXSlider;
	TextureFrame* m_TitleBGM;
};

}
}
#endif
