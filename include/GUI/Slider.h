#ifndef GUI_SLIDER_H__
#define GUI_SLIDER_H__

#include "GUI/TextureFrame.h"
#include "GUI/Button.h"
#include "GUI/EButtonPress.h"
#include "GUI/EButtonRelease.h"
#include "GUI/ESliderUpdate.h"
#include "Core/EMouseMove.h"
#include "Sound/EPlaySound.h"

namespace dd
{
namespace GUI
{

class Slider : public TextureFrame
{
public:
	Slider(Frame* parent, std::string name)
		: TextureFrame(parent, name)
	{
		m_SliderButton = new GUI::Button(this, "SliderButton");
		m_SliderButton->DisableScissor();

		EVENT_SUBSCRIBE_MEMBER(m_EButtonPress, &Slider::OnButtonPress);
		EVENT_SUBSCRIBE_MEMBER(m_EButtonRelease, &Slider::OnButtonRelease);
		EVENT_SUBSCRIBE_MEMBER(m_EMouseMove, &Slider::OnMouseMove);
	}

	float Percentage() const { return m_Percentage; }
	void SetPercentage(const float percentage)
	{
		m_Percentage = percentage;
		m_SliderButton->X = percentage * (Width - m_SliderButton->Width);
	}

	void SetTextureHover(std::string resourceName)
	{
		m_SliderButton->SetTextureHover(resourceName);
	}
	void SetTextureReleased(std::string resourceName)
	{
		m_SliderButton->SetTextureReleased(resourceName);
	}
	void SetTexturePressed(std::string resourceName)
	{
		m_SliderButton->SetTexturePressed(resourceName);
	}

	void AlignVertically()
	{
		m_SliderButton->Y = (Height / 2) - (m_SliderButton->Height / 2);
	}

	~Slider()
	{
		if (m_SliderButton != nullptr) {
			delete m_SliderButton;
			m_SliderButton = nullptr;
		}
	}

	virtual void Update(double dt) override
	{

	}

private:
	Button* m_SliderButton = nullptr;
	bool m_IsGrabbed = false;
	float m_Percentage = 0.f;

	EventRelay<Frame, Events::ButtonPress> m_EButtonPress;
	EventRelay<Frame, Events::ButtonRelease> m_EButtonRelease;
	EventRelay<Frame, Events::MouseMove> m_EMouseMove;

	virtual bool OnButtonPress(const Events::ButtonPress& event)
	{
		if (event.Button == m_SliderButton) {
			m_IsGrabbed = true;
		}
		else {
			Events::PlaySound e;
			e.FilePath = "Sounds/GUI/click-n.wav";
			EventBroker->Publish(e);
		}

		return true;
	}

	virtual bool OnButtonRelease(const Events::ButtonRelease& event)
	{
		if (event.Button == m_SliderButton) {
			m_IsGrabbed = false;
		}

		return true;
	}

	virtual bool OnMouseMove(const Events::MouseMove& event)
	{
		if (m_IsGrabbed) {
			// Set horizontal position
			m_SliderButton->X += event.DeltaX;
			if (m_SliderButton->Left() < Left()) {
				m_SliderButton->SetLeft(Left());
			} else if (m_SliderButton->Right() > Right()) {
				m_SliderButton->SetRight(Right());
			}

			// Update percentage
			float newPercentage = m_SliderButton->X / (float)(Width - m_SliderButton->Width);
			Events::SliderUpdate e;
			e.FrameName = Name();
			e.Slider = this;
			e.Percentage = newPercentage;
			e.DeltaPercentage = newPercentage - m_Percentage;
			EventBroker->Publish(e);
			m_Percentage = newPercentage;
		}

		return true;
	}
};

}
}
#endif

