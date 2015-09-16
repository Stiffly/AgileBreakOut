//
// Created by Administrator on 2015-09-16.
//

#ifndef DAYDREAM_TEXTUREFRAME_H
#define DAYDREAM_TEXTUREFRAME_H

#include "GUI/Frame.h"
#include "Core/Texture.h"

namespace dd
{

class TextureFrame : public Frame {
public:
    TextureFrame(Frame *parent, std::string name)
            : Frame(parent, name), m_Texture(nullptr), m_FadeTexture(nullptr),
              m_Color(glm::vec4(1.f, 1.f, 1.f, 1.f)), m_FadeDuration(0.f), m_CurrentFade(1.f) { }

    void Draw(std::shared_ptr<Renderer> renderer) override {
        if (m_Texture == nullptr)
            return;

        RenderQueue.Clear();

        // Main texture
        {
            SpriteJob job;
            job.TextureID = m_Texture->ResourceID;
            job.Texture = *m_Texture;
            job.Color = glm::vec4(m_Color.r, m_Color.g, m_Color.b, m_Color.a * m_CurrentFade);
            RenderQueue.Forward.Add(job);
        }

        // Texture while fading
        if (m_FadeTexture && m_CurrentFade < 1) {
            SpriteJob job;
            job.TextureID = m_FadeTexture->ResourceID;
            job.Texture = *m_FadeTexture;
            job.Color = glm::vec4(m_Color.r, m_Color.g, m_Color.b, m_Color.a);
            RenderQueue.Forward.Add(job);
        }

        renderer->SetCamera(nullptr);
        renderer->SetViewport(Rectangle(Left(), Top(), Width, Height));
        renderer->DrawFrame(RenderQueue);
    }

    Texture *Texture() const { return m_Texture; }

    void SetTexture(std::string resourceName) {
        m_Texture = ResourceManager->Load<::Texture>("Texture", resourceName);
    }

    void FadeToTexture(std::string resourceName, double duration) {
        m_FadeTexture = m_Texture;
        SetTexture(resourceName);
        m_FadeDuration = duration;
        m_CurrentFade = 0.f;
    }

    void Update(double dt) override {
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
    Texture *m_Texture;
    Texture *m_FadeTexture;
    glm::vec4 m_Color;
    float m_FadeDuration;
    float m_CurrentFade;

};

}

#endif //DAYDREAM_TEXTUREFRAME_H
