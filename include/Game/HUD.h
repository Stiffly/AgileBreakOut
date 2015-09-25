#ifndef GUI_HUD_H__
#define GUI_HUD_H__

#include "GUI/Frame.h"
#include "GUI/TextureFrame.h"

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

		m_ScoreIndicator = new GUI::TextureFrame(this, "HUDScoreIndicator");
		m_ScoreIndicator->SetTexture("Textures/GUI/HUD/ScoreIndicatorBG.png");
		m_ScoreIndicator->SetRight(Right());
	}

private:
	TextureFrame* m_LevelIndicator = nullptr;
	TextureFrame* m_ScoreIndicator = nullptr;
};

}
}
#endif

