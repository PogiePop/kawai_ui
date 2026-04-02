#include <ui_render>


namespace Kawai
{
    UIRender UIRender::render = UIRender();

    void UIRender::init(int width, int height)
    {
        aspect = screenWidth / screenHeight;

        render = *this;
        //开启透明混合
        glEnable(GL_BLEND);
        // src * src_alpha + other * (1 - src_alpha)
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
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