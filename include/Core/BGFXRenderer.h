#ifndef BGFXRenderer_h__
#define BGFXRenderer_h__

#include <bgfx.h>
#include <bgfxplatform.h>

#include "Util/Rectangle.h"
#include "Core/Camera.h"
#include "Core/RenderQueue.h"
#include "Core/ShaderProgram.h"

namespace dd
{

class BGFXRenderer
{
public:
    GLFWwindow* Window() const { return m_Window; }
    Rectangle Resolution() const { return m_Resolution; }
    void SetResolution(const Rectangle& resolution) { m_Resolution = resolution; }
    bool Fullscreen() { return m_Fullscreen; }
    void SetFullscreen(bool fullscreen) { m_Fullscreen = fullscreen; }
    bool VSYNC() const { return m_VSYNC; }
    void SetVSYNC(bool vsync) { m_VSYNC = vsync; }
    //void SetViewport(const Rectangle& viewport) { m_Viewport = viewport; }
    //void SetScissor(const Rectangle& scissor) { m_Scissor = scissor; }
    const dd::Camera* Camera() const { return m_Camera; }
    void SetCamera(const dd::Camera* camera)
    {
        if (camera == nullptr) {
            m_Camera = m_DefaultCamera.get();
        } else {
            m_Camera = camera;
        }
    }

    void Initialize();
    void Draw(RenderQueueCollection& rq);

private:
    Rectangle m_Resolution = Rectangle(1280, 720);
    bool m_Fullscreen = false;
    bool m_VSYNC = false;
    std::unique_ptr<dd::Camera> m_DefaultCamera = nullptr;
    const dd::Camera* m_Camera = nullptr;

    GLFWwindow* m_Window = nullptr;

    void DrawForward();
    void DrawDeferred(RenderQueue &objects, RenderQueue &lights);
    void DrawScene(RenderQueue &objects, ShaderProgram &program);
    void DrawLightScene(RenderQueue &objects, ShaderProgram &program);
    void LoadShaders();
    void CreateBuffers();
    void debugKeys();

    ShaderProgram* m_spDeferred1;
    ShaderProgram* m_spDeferred2;
    ShaderProgram* m_spDeferred3;
    ShaderProgram* m_spForward;
    ShaderProgram* m_spScreen;
    //TODO: Shaders, CreateBuffers, Draw forward, drawdeferred, drawscene, drawlights.
};

}

#endif
