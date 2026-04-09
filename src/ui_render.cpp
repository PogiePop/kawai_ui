#include <ui_render>
#include <ui_window>

namespace Kawai
{
    void UIRender::init(int width, int height)
    {
        this->screenWidth = width;
        this->screenHeight = height;
        aspect = screenWidth / screenHeight;

        // 开启透明混合
        glEnable(GL_BLEND);
        // src * src_alpha + other * (1 - src_alpha)
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    }

    void UIRender::SetWindow(UIWindow *window)
    {
        this->m_Window = window;
    }

    void UIRender::DrawArrays(GLenum mode, GLuint vao, size_t vertexCount)
    {
        glBindVertexArray(vao);
        glDrawArrays(mode, 0, vertexCount);
        glBindVertexArray(0);
    }

    void UIRender::DrawElements(GLenum mode, GLuint vao, size_t indexCount)
    {
        glBindVertexArray(vao);
        glDrawElements(mode, indexCount, GL_UNSIGNED_INT, nullptr);
        glBindVertexArray(0);
    }

    
}