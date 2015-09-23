#ifndef GUI_BUTTON_H__
#define GUI_BUTTON_H__

#include "GUI/TextureFrame.h"
#include "Core/EMouseMove.h"
#include "Core/EMousePress.h"
#include "Core/EMouseRelease.h"

namespace dd
{
namespace GUI
{

class Button : public TextureFrame
{
public:
	Button(Frame* parent, std::string name)
		: TextureFrame(parent, name)
	{
		EVENT_SUBSCRIBE_MEMBER(m_EMouseMove, &Button::OnMouseMove);
		EVENT_SUBSCRIBE_MEMBER(m_EMousePress, &Button::OnMousePress);
		EVENT_SUBSCRIBE_MEMBER(m_EMouseRelease, &Button::OnMouseRelease);
	}

	std::string TextureHover;
	std::string TexturePressed;
	std::string TextureReleased;

	void Draw(RenderQueueCollection& rq) override
	{
		if (m_Texture == nullptr && !TextureReleased.empty()) {
			SetTexture(TextureReleased);
		}

		TextureFrame::Draw(rq);
	}

protected:
	bool m_MouseIsOver = false;
	bool m_IsDown = false;

	virtual bool OnMouseMove(const Events::MouseMove& event)
	{
		bool isOver = Rectangle::Intersects(AbsoluteRectangle(), Rectangle(event.X, event.Y, 1, 1));
		if (isOver && !m_MouseIsOver) { // Enter
			LOG_INFO("%s mouse enter", m_Name.c_str());
			if (!m_IsDown) {
				SetTexture(TextureHover);
			}
		} else if (!isOver && m_MouseIsOver) { // Leave
			LOG_INFO("%s mouse left", m_Name.c_str());
			if (!m_IsDown) {
				SetTexture(TextureReleased);
			}
		}
		m_MouseIsOver = isOver;

		return true;
	}
	virtual bool OnMousePress(const Events::MousePress& event)
	{
		if (!Rectangle::Intersects(AbsoluteRectangle(), Rectangle(event.X, event.Y, 1, 1))) {
			return false;
		}

		if (!TexturePressed.empty()) {
			SetTexture(TexturePressed);
		}

		LOG_INFO("%s mouse pressed", m_Name.c_str());

		m_IsDown = true;

		return true;
	}
	virtual bool OnMouseRelease(const Events::MouseRelease& event)
	{
		bool isOver = Rectangle::Intersects(AbsoluteRectangle(), Rectangle(event.X, event.Y, 1, 1));
		if (!isOver && !m_IsDown) {
			return false;
		}

		if (!TextureReleased.empty()) {
			SetTexture(TextureReleased);
		}

		LOG_INFO("%s mouse released", m_Name.c_str());

		m_IsDown = false;

		return true;
	}

private:
	EventRelay<Frame, Events::MouseMove> m_EMouseMove;
	EventRelay<Frame, Events::MousePress> m_EMousePress;
	EventRelay<Frame, Events::MouseRelease> m_EMouseRelease;
};

}
}
#endif
