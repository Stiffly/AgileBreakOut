#ifndef GUI_NUMBERFRAME_H__
#define GUI_NUMBERFRAME_H__

#include <unordered_map>

#include "GUI/TextureFrame.h"

namespace dd
{
namespace GUI
{

class NumberFrame : public TextureFrame
{
public:
	NumberFrame(Frame* parent, std::string name)
		: TextureFrame(parent, name)
	{
		m_CharacterTextures['0'] = "Textures/GUI/Numbers/N0.png";
		m_CharacterTextures['1'] = "Textures/GUI/Numbers/N1.png";
		m_CharacterTextures['2'] = "Textures/GUI/Numbers/N2.png";
		m_CharacterTextures['3'] = "Textures/GUI/Numbers/N3.png";
		m_CharacterTextures['4'] = "Textures/GUI/Numbers/N4.png";
		m_CharacterTextures['5'] = "Textures/GUI/Numbers/N5.png";
		m_CharacterTextures['6'] = "Textures/GUI/Numbers/N6.png";
		m_CharacterTextures['7'] = "Textures/GUI/Numbers/N7.png";
		m_CharacterTextures['8'] = "Textures/GUI/Numbers/N8.png";
		m_CharacterTextures['9'] = "Textures/GUI/Numbers/N9.png";

		SetNumber(0);
	}

	int Number() const { return m_Number; }
	void SetNumber(int number)
	{
		m_Number = number;

		// Clear number textures
		for (auto& frame : m_NumberFrames) {
			frame->SetTexture("");
		}

		Width = 0;
		Height = 0;

		std::string digits = std::to_string(number);

		int i = 0;
		for (char& c : digits) {
			// Create number frame if missing
			if (i >= m_NumberFrames.size()) {
				auto frame = new GUI::TextureFrame(this, "NumberFrameDigit");
				if (i > 0) {
					frame->SetLeft(m_NumberFrames[i - 1]->Right());
				}
				frame->DisableScissor();
				m_NumberFrames.push_back(frame);
			}

			// Update value
			std::string texture = "Textures/Core/ErrorTexture.png";
			if (m_CharacterTextures.find(c) != m_CharacterTextures.end()) {
				texture = m_CharacterTextures.at(c);
			}
			m_NumberFrames[i]->SetTexture(m_CharacterTextures.at(c));
			Width += m_NumberFrames[i]->Width;
			if (Height < m_NumberFrames[i]->Height) {
				Height = m_NumberFrames[i]->Height;
			}

			i++;
		}
	}

	void SetLeftAlign() { m_LeftAligned = true; }
	void SetRightAlign() { m_LeftAligned = false; }

	virtual void Update(double dt) override
	{

	}

private:
	int m_Number = 0;
	bool m_LeftAligned = true;

	std::unordered_map<char, std::string> m_CharacterTextures;
	std::deque<TextureFrame*> m_NumberFrames;
};

}
}
#endif

