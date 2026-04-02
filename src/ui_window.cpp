#include <ui_window>
#include <ka_basic>
namespace Kawai
{
    void UIWindow::init(int w, int h, const std::string& title)
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(w, h, title.c_str(), nullptr, nullptr);
        if(!window)
            KA_ASSERT(window, "窗口创建失败!!!");
        glfwMakeContextCurrent(window);

        int load = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        if(!load)
        {
            glfwTerminate();
            KA_ASSERT(load, "函数加载失败!!!");
        }

        glViewport(0, 0, w, h);
        
        glfwSetWindowUserPointer(window, this);
    }

    void UIWindow::OnUpdate()
    {
        while(!glfwWindowShouldClose(window))
        {
            OnRender();
            glfwPollEvents();
        }
    }

    void UIWindow::OnRender()
    {
        glClearColor(color.r, color.g, color.b, color.a);
        glClear(GL_COLOR_BUFFER_BIT);
        glfwSwapBuffers(window);
    }

    
}