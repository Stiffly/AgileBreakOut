#ifndef GUI_TextureFrame_h__
#define GUI_TextureFrame_h__

#include "GUI/Frame.h"
#include "Core/Texture.h"

namespace dd
{
namespace GUI
{

class TextureFrame : public Frame
{
public:
	TextureFrame(Frame* parent, std::string name)
		: Frame(parent, name) { }

	void Draw(RenderQueueCollection& rq) override
	{
		if (m_Texture == nullptr)
			return;

		// Main texture
		{
			FrameJob job;
			job.Scissor = m_Parent->AbsoluteRectangle();
			job.Viewport = Rectangle(Left(), Top(), Width, Height);
			job.TextureID = m_Texture->ResourceID;
			job.DiffuseTexture = *m_Texture;
			job.Color = glm::vec4(m_Color.r, m_Color.g, m_Color.b, m_Color.a * m_CurrentFade);
			rq.GUI.Add(job);
		}

		// Texture while fading
		if (m_FadeTexture && m_CurrentFade < 1) {
			FrameJob job;
			job.Scissor = m_Parent->AbsoluteRectangle();
			job.Viewport = Rectangle(Left(), Top(), Width, Height);
			job.TextureID = m_FadeTexture->ResourceID;
			job.DiffuseTexture = *m_FadeTexture;
			job.Color = glm::vec4(m_Color.r, m_Color.g, m_Color.b, m_Color.a);
			rq.GUI.Add(job);
		}
	}

	dd::Texture* Texture() const { return m_Texture; }

	void SetTexture(std::string resourceName)
	{
		m_Texture = ResourceManager::Load<dd::Texture>(resourceName);
		if (m_Texture == nullptr) {
			m_Texture = ResourceManager::Load<dd::Texture>("Textures/Core/ErrorTexture.png");
		}
	}

	void FadeToTexture(std::string resourceName, double duration)
	{
		m_FadeTexture = m_Texture;
		SetTexture(resourceName);
		m_FadeDuration = duration;
		m_CurrentFade = 0.f;
	}

	void Update(double dt) override
	{
		if (m_CurrentFade < 1) {
			m_CurrentFade += dt / m_FadeDuration;
			if (m_CurrentFade > 1) {
				m_FadeTexture = nullptr;
				m_CurrentFade = 1;
				m_FadeDuration = 0;
			}
		}
	}

	glm::vec4 Color() const { return m_Color; }
	void SetColor(glm::vec4 val) { m_Color = val; }

protected:
	dd::Texture* m_Texture = nullptr;
	dd::Texture* m_FadeTexture = nullptr;
	glm::vec4 m_Color = glm::vec4(1.f, 1.f, 1.f, 1.f);
	float m_FadeDuration = 0.f;
	float m_CurrentFade = 1.f;

};

}
}

#endif // GUI_TextureFrame_h__
