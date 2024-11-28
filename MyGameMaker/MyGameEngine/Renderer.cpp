#include "Renderer.h"

#include <iostream>
#include <string>
#include "Engine.h"
#include <imgui.h>

using namespace std;

void GLClearError() {
    while (glGetError() != GL_NO_ERROR); // glGetError() != GL_NO_ERROR
}

bool GLLogCall(const char* function, const char* file, int line)
{
    while (GLenum error = glGetError())
    {
        cout << "[OpenGL Error] (" << error << "): " << function <<
            " " << file << ": " << line << endl;
        return false;
    }
    return true;
}

void Renderer::Clear() const
{
    GLCall(glClear(GL_COLOR_BUFFER_BIT));
}

void Renderer::Start() const
{
    glewInit();
    LOG(LogType::LOG_INFO, "# Succesfully initialized glew!");
    LOG(LogType::LOG_APPINFO, "Glew Versio: %s", (char*)glewGetString(GLEW_VERSION));
    LOG(LogType::LOG_INFO, "# Setting OpenGL Attributes...");
    if (!GLEW_VERSION_3_0) throw exception("OpenGL 3.0 API is not available.");
    GLCall(glEnable(GL_DEPTH_TEST));
    GLCall(glClearColor(0.5, 0.5, 0.5, 1.0));
    GLCall(glEnable(GL_BLEND));
    GLCall(glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA));

    LOG(LogType::LOG_APPINFO, "Vendor: %s", (char*)glGetString(GL_VENDOR));
    LOG(LogType::LOG_APPINFO, "Renderer: %s", (char*)glGetString(GL_RENDERER));
    LOG(LogType::LOG_APPINFO, "OpenGL version supported: %s", (char*)glGetString(GL_VERSION));
    LOG(LogType::LOG_APPINFO, "GLSL version: %s", (char*)glGetString(GL_SHADING_LANGUAGE_VERSION));
    LOG(LogType::LOG_APPINFO, "ImGui version: %s", ImGui::GetVersion());
}

//void Renderer::Draw(const VertexArray& va, const IndexBuffer& ib, const Shader& shader) const {
//    shader.Bind();
//    //bind de vertex array
//    va.Bind();
//    //bind de index buffer
//    ib.Bind();
//
//    GLCall(glDrawElements(GL_TRIANGLES, ib.GetCount(), GL_UNSIGNED_INT, nullptr));
//}