#ifndef GAME_GUI_STORYBOARDFRAME_H__
#define GAME_GUI_STORYBOARDFRAME_H__

#include "GUI/TextureFrame.h"

namespace dd
{
namespace GUI
{

class StoryboardFrame : public TextureFrame
{
public:
	struct Transition
	{
		double Time = 0.0;
		double Duration = 0.0;
		Rectangle Viewport;
		std::string Texture;
	};

	StoryboardFrame(Frame* parent, std::string name)
		: TextureFrame(parent, name) 
	{

		m_TextureFrame = new TextureFrame(this, "StoryboardFrameHelper");

		//m_TextureFrame->DisableScissor();
	}

	void AddTransition(Transition& transition)
	{
		m_Timeline.push_back(transition);
	}

	void Play()
	{
		m_Playing = true;
	}

	void Skip()
	{
		m_Playing = false;
		OnComplete();
	}

	void Update(double dt)
	{
		if (Hidden()) {
			return;
		}

		if (m_Playing) {
			m_Time += dt;

			if (m_NextIndex < m_Timeline.size()) {
				if (m_Time >= m_Timeline.at(m_NextIndex).Time) {
					m_CurrentIndex = m_NextIndex;
					Transition& t = m_Timeline.at(m_CurrentIndex);

					m_TextureFrame->FadeToTexture(t.Texture, t.Duration);
					//m_TextureFrame->SetTexture(t.Texture);
					m_TextureFrame->SizeToTexture();

					if (m_StartRect == Rectangle()) {
						m_StartRect = Rectangle(0, 0, m_TextureFrame->Width, m_TextureFrame->Height);
					}
					else {
						m_StartRect = m_TargetRect;
					}

					double wScaleParent = Width / (double)m_TextureFrame->Width;
					double wScale = m_TextureFrame->Width / (double)t.Viewport.Width;
					LOG_DEBUG("wScale: %f", wScale);
					double hScaleParent = Height / (double)m_TextureFrame->Height;
					double hScale = m_TextureFrame->Height / (double)t.Viewport.Height;
					LOG_DEBUG("hScale: %f", hScale);
					m_TargetRect.X = -t.Viewport.X * wScale * wScaleParent;
					m_TargetRect.Y = -t.Viewport.Y * hScale * hScaleParent;
					m_TargetRect.Width = m_TextureFrame->Width * wScale * wScaleParent;
					m_TargetRect.Height = m_TextureFrame->Height * hScale * hScaleParent;

					m_NextIndex++;
				}
			} else {
				OnComplete();
			}
		}

		Transition& t = m_Timeline.at(m_CurrentIndex);
		double progress = glm::min((m_Time - t.Time)/t.Duration, 1.0);
		m_TextureFrame->X = (int)easeSine(progress, m_StartRect.X, (m_TargetRect.X - m_StartRect.X), 1.0);
		m_TextureFrame->Y = (int)easeSine(progress, m_StartRect.Y, (m_TargetRect.Y - m_StartRect.Y), 1.0);
		m_TextureFrame->Width = (int)easeSine(progress, m_StartRect.Width, (m_TargetRect.Width - m_StartRect.Width), 1.0);
		m_TextureFrame->Height = (int)easeSine(progress, m_StartRect.Height, (m_TargetRect.Height - m_StartRect.Height), 1.0);
	}

	virtual void OnComplete() { }

protected:
	TextureFrame* m_TextureFrame = nullptr;

private:
	bool m_Playing = false;
	double m_Time = 0;
	int m_CurrentIndex = 0;
	int m_NextIndex = 0;
	std::vector<Transition> m_Timeline;
	Rectangle m_StartRect;
	Rectangle m_TargetRect;

	double easeSine(double t, double b, double c, double d) 
	{
		return -c / 2 * (glm::cos(glm::pi<double>()*t / d) - 1) + b;
	}

	float easeExpo(float t,float b , float c, float d) {
		if (t == 0) return b;
		if (t == d) return b + c;
		if ((t /= d / 2) < 1) return c / 2 * pow(2, 10 * (t - 1)) + b;
		return c / 2 * (-pow(2, -10 * --t) + 2) + b;
	}
};

}
}

#endif
