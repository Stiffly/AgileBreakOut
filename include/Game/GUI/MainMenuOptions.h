#ifndef GUI_MAINMENUOPTIONS_H__
#define GUI_MAINMENUOPTIONS_H__

#include "GUI/TextureFrame.h"
#include "GUI/Button.h"
#include "GUI/Slider.h"
#include "GUI/ESliderUpdate.h"
#include "Sound/EMasterVolume.h"

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
		m_SFXSlider->SetTop(m_TitleSFX->Bottom() + 10);
		m_SFXSlider->SetTexture("Textures/GUI/Menu/SliderBackground.png");
		m_SFXSlider->SetTextureReleased("Textures/GUI/Menu/SliderHandle.png");
		m_SFXSlider->AlignVertically();
		m_SFXSlider->SetPercentage(1.f);

		m_TitleBGM = new GUI::TextureFrame(this, "MainMenuOptionsBGMTitle");
		m_TitleBGM->SetTop(m_SFXSlider->Bottom() + 20);
		m_TitleBGM->SetTexture("Textures/GUI/Menu/OptionsBGMTitle.png");
		m_BGMSlider = new GUI::Slider(this, "MainMenuOptionsBGMSlider");
		m_BGMSlider->X = 66;
		m_BGMSlider->SetTop(m_TitleBGM->Bottom() + 10);
		m_BGMSlider->SetTexture("Textures/GUI/Menu/SliderBackground.png");
		m_BGMSlider->SetTextureReleased("Textures/GUI/Menu/SliderHandle.png");
		m_BGMSlider->AlignVertically();
		m_BGMSlider->SetPercentage(1.f);

		EVENT_SUBSCRIBE_MEMBER(m_ESliderUpdate, &MainMenuOptions::OnSliderUpdate);
	}

private:
	TextureFrame* m_Title = nullptr;
	TextureFrame* m_TitleSFX = nullptr;
	Slider* m_SFXSlider = nullptr;
	Slider* m_BGMSlider = nullptr;
	TextureFrame* m_TitleBGM = nullptr;

	EventRelay<Frame, Events::SliderUpdate> m_ESliderUpdate;
	bool OnSliderUpdate(const Events::SliderUpdate& event)
	{
		if (event.Slider == m_SFXSlider) {
			Events::MasterVolume e;
			e.IsAmbient = false;
			e.Gain = event.Percentage;
			EventBroker->Publish(e);
		}

		if (event.Slider == m_BGMSlider) {
			Events::MasterVolume e;
			e.IsAmbient = true;
			e.Gain = event.Percentage;
			EventBroker->Publish(e);
		}

		return true;
	}
};

}
}
#endif
