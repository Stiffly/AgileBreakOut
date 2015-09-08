#ifndef BGFXRenderer_h__
#define BGFXRenderer_h__

#include <bgfx.h>
#include <bgfxplatform.h>

#include "Util/Rectangle.h"
#include "Core/Camera.h"
#include "Core/RenderQueue.h"

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

    void Initialize()
    {
        // Initialize GLFW
        if (!glfwInit()) {
            LOG_ERROR("GLFW: Initialization failed");
            exit(EXIT_FAILURE);
        }

        // Create a window
        GLFWmonitor* monitor = nullptr;
        if (m_Fullscreen) {
            monitor = glfwGetPrimaryMonitor();
        }
        glfwWindowHint(GLFW_SAMPLES, 8);
        m_Window = glfwCreateWindow(m_Resolution.Width, m_Resolution.Height, "daydream", monitor, nullptr);
        if (!m_Window) {
            LOG_ERROR("GLFW: Failed to create window");
            exit(EXIT_FAILURE);
        }
        //glfwMakeContextCurrent(m_Window);

        // Initialize GLEW
//        if (glewInit() != GLEW_OK) {
//            LOG_ERROR("GLEW: Initialization failed");
//            exit(EXIT_FAILURE);
//        }
        //LOG_ERROR("STUFF");

        // Initialize bgfx
        bgfx::glfwSetWindow(m_Window);
        bgfx::init();
        bgfx::reset(m_Resolution.Width, m_Resolution.Height, BGFX_RESET_NONE);
        bgfx::setDebug(BGFX_DEBUG_TEXT);
        bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);
    }

    void Draw(RenderQueueCollection& rq)
    {
        bgfx::touch(0);

        bgfx::dbgTextClear();
        bgfx::dbgTextPrintf(0, 1, 0x4f, "daydream");

        bgfx::frame();
    }

private:
    Rectangle m_Resolution = Rectangle(1280, 720);
    bool m_Fullscreen = false;
    bool m_VSYNC = false;
    std::unique_ptr<dd::Camera> m_DefaultCamera = nullptr;
    const dd::Camera* m_Camera = nullptr;

    GLFWwindow* m_Window = nullptr;
};

}

#endif
