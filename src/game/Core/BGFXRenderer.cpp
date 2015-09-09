#include "PrecompiledHeader.h"
#include "Core/BGFXRenderer.h"

void dd::BGFXRenderer::Initialize() {

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
    //intiialize BGFX
    bgfx::glfwSetWindow(m_Window);

    bgfx::init();
    bgfx::reset(m_Resolution.Width, m_Resolution.Height, BGFX_RESET_NONE);

    //Enable debug text
    bgfx::setDebug(BGFX_DEBUG_TEXT);

    //Set view 0 clear state
    bgfx::setViewClear(0, BGFX_CLEAR_COLOR | BGFX_CLEAR_DEPTH, 0x303030ff, 1.0f, 0);

    std::stringstream ss;
    ss << bgfx::getRendererName(bgfx::getRendererType());
#ifdef DEBUG
    ss << " DEBUG";
#endif
    LOG_INFO(ss.str().c_str());
    glfwSetWindowTitle(m_Window, ss.str().c_str());
    //LoadShaders
    //CreateBuffers

}

void dd::BGFXRenderer::Draw(RenderQueueCollection& rq) {
    //TODO: Fix this shit.

    //Draw the different passes.
    DrawDeferred(rq.Deferred, rq.Lights);
    //DrawForward(); //For alpha things

    //Settings for the last combination of images.
    //Clear
    //bind textures
    //Draw array

    //Swapbuffer?

    //DEbugKeys.

//    bgfx::touch(0);
//    bgfx::dbgTextClear();
//    bgfx::dbgTextPrintf(0, 1, 0x4f, "DayDream");
//    bgfx::frame();
}

void dd::BGFXRenderer::DrawDeferred(RenderQueue &objects, RenderQueue &lights) {
    //Pass 1: Fill g-buffers

    //Make sure the settings are correct, like Backface culling and depthmask and shit.
    //Clear
    //Bind The first shaders with the remade shaderprogram class.
    //Draw the scene by calling DrawScene() and putting in objects and the shaderprogram used.

    //Pass 2: Lighting

    //Make sure the settings are correct, like backface culling and depthmask and shit.
    //clear
    //bind shaders with remade shaderProgram class.
    //Draw by calling DrawLightSpheres() and sending in the Renderqueue with lights.

    //Pass 3: Combine into final image.

    //Make sure settings are correct.
    //clear
    //Bind shaderprograms
    //Bind light and gbuffer textures to uniforms so the shader can use them.
}

void dd::BGFXRenderer::DrawScene(RenderQueue &objects, ShaderProgram &program) {
    //Init shaderprogram

    //Fix the camera matrises

    //Loop throught jobs and modeljobs, make sure the uniform/texture variables are set.
}

void dd::BGFXRenderer::DrawLightScene(RenderQueue &objects, ShaderProgram &program) {
    //Init shaderProgram

    //Bind textures

    //Fix matrises

    //Loop through the jobs and draw them with the right uniforms and shit.
}
void dd::BGFXRenderer::debugKeys() {

    //TODO: Insert keys to debug rendering.
}

void dd::BGFXRenderer::LoadShaders() {
    //Remake shaderprogram so the loadshader function can work with bgfx
}

void dd::BGFXRenderer::CreateBuffers() {
    //Quad and sphere for what?

    //Depth buffer

    //Generate G-Buffer(first pass) textures

    //Create first pass frame buffer

    //Generate Lighting(second pass) texture

    //Generate second pass framebuffer

    //Generate final texture

    //Generate third pass buffer
}

