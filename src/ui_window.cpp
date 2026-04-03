#include <ui_window>
#include <ka_basic>
#include <algorithm>
namespace Kawai
{
    void UIWindow::init(int w, int h, const std::string &title)
    {
        glfwInit();
        glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
        glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
        glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

        window = glfwCreateWindow(w, h, title.c_str(), nullptr, nullptr);
        if (!window)
            KA_ASSERT(window, "窗口创建失败!!!");
        glfwMakeContextCurrent(window);

        int load = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
        if (!load)
        {
            glfwTerminate();
            KA_ASSERT(load, "函数加载失败!!!");
        }

        glViewport(0, 0, w, h);

        UIRender::GetInstance().init(w, h);
        UIRender::GetInstance().SetWindow(this);

        glfwSetWindowUserPointer(window, this);
    }

    void UIWindow::OnUpdate()
    {
        while (!glfwWindowShouldClose(window))
        {
            OnRender();
            glfwPollEvents();
        }
    }

    void UIWindow::OnRender()
    {
        glClearColor(color.r, color.g, color.b, color.a);
        glClear(GL_COLOR_BUFFER_BIT);
        //更新组件
        for (auto &comp : ui_components)
        {
            if(comp->childChange)
                UpdateComponent(comp);
        }

        //构建优先队列
        std::vector<UIComponent*>queue;
        for (auto &comp : ui_components)
            CollectComponent(comp, queue);
        
        //排序
        std::sort(queue.begin(), queue.end(), [](UIComponent* comp1, UIComponent* comp2)
        { return comp1->priority < comp2->priority; });

        //渲染
        for(const auto& comp : queue)
            comp->render();



        glfwSwapBuffers(window);
    }

    void UIWindow::CollectComponent(UIComponent* comp, std::vector<UIComponent*>& queue)
    {
        queue.push_back(comp);
        for(const auto& child : comp->childrens)
            CollectComponent(child, queue);
    }

    void UIWindow::UpdateComponent(UIComponent *comp)
    {
        for (auto &child : comp->childrens)
        {
            switch (child->GetComponentType())
            {
            case Rect:
                child->SetShader(this->defaultUIShader);
                break;
            case Panel:
                child->SetShader(this->radiusShader);
                break;
            default:
                break;
            }
            UpdateComponent(child);
        }
        comp->childChange = false;
    }

    UIWindow::~UIWindow()
    {
        for (auto &comp : ui_components)
        {
            delete comp;
        }
        ui_components.clear();
        delete defaultUIShader;
        if (this->window)
            glfwDestroyWindow(window);
    }

}