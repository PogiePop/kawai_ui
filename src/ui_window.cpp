#include <ui_window>
#include <ka_basic>
#include <algorithm>
#include <ka_time>
#include <event/event>
#include <event/event_window>
#include <event/event_mouse>
#include <stack>
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

        // 初始化回调函数
        m_SizeCall = [this](GLFWwindow *, int width, int height)
        {
            // 事件处理
            WindowResizeEvent wre(width, height);
            OnEvent(wre);
        };

        m_CursorCall = [this](GLFWwindow *, double x, double y)
        {
            MouseMoveEvent mme(x, y);
            OnEvent(mme);
        };

        // 设置回调
        glfwSetFramebufferSizeCallback(window, [](GLFWwindow *window, int width, int height)
                                       {
            glViewport(0, 0, width, height);

            
            UIWindow* self = (UIWindow*)glfwGetWindowUserPointer(window);
            if(self)
            {
                UIRender::GetInstance().SetScW(width);
                UIRender::GetInstance().SetScH(height);
                self->needResize = true;        
                if(self->m_SizeCall)self->m_SizeCall(window, width, height);
            } });

        glfwSetCursorPosCallback(window, [](GLFWwindow *window, double x, double y)
                                 {
            UIWindow* self = (UIWindow*)glfwGetWindowUserPointer(window);
            if(self)
            {
                if(self->m_CursorCall)self->m_CursorCall(window, x, y);
            } });
    }

    void UIWindow::OnUpdate()
    {
        while (!glfwWindowShouldClose(window))
        {
            Time::GetInstance().UpdateTime();
            OnRender();
            glfwPollEvents();
        }
    }

    //TODO: 还有点问题
    void UIWindow::OnRender()
    {
        glClearColor(color.r, color.g, color.b, color.a);
        glClear(GL_COLOR_BUFFER_BIT);
        // 更新组件
        for (auto &comp : ui_components)
        {
            if (comp->childChange)
                UpdateComponent(comp);
        }

        if (needResize)
        {
            for (auto &comp : ui_components)
                comp->UpdateComponentSize();
            needResize = false;
        }

        //构建优先队列
        std::vector<UIComponent*>queue;
        for (auto &comp : ui_components)
            CollectComponent(comp, queue);

        // 构建事件队列
        if (needSort || uiNumChange)
        {
            ui_eventQ.clear();
            std::stack<std::pair<UIComponent *, bool>> st;

            // 根节点逆序入栈 → 后添加的根优先处理
            for (auto it = ui_components.rbegin(); it != ui_components.rend(); ++it)
            {
                st.push({*it, false});
            }

            while (!st.empty())
            {
                auto [comp, visited] = st.top();
                st.pop();

                if (!visited)
                {
                    // 父节点先压回去（等子节点处理完再来处理自己）
                    st.push({comp, true});

                    // 子节点逆序压栈 → 最右边的子节点最先弹出
                    for (auto it = comp->childrens.rbegin(); it != comp->childrens.rend(); ++it)
                    {
                        st.push({*it, false});
                    }
                }
                else
                {
                    // 所有子节点都已经入队了，才入队自己
                    ui_eventQ.push_back(comp);
                }
            }

            // 关键：这里绝对不要 reverse！
            // std::reverse(ui_eventQ.begin(), ui_eventQ.end());  // 删掉！

            // 稳定排序：优先级高的在前，同优先级保持“子在前，父在后”
            std::stable_sort(ui_eventQ.begin(), ui_eventQ.end(), [](UIComponent *a, UIComponent *b)
                             { return a->priority > b->priority; });

            needSort = false;
            uiNumChange = false;
        }

        //排序
        std::sort(queue.begin(), queue.end(), [](UIComponent* comp1, UIComponent* comp2)
        { return comp1->priority < comp2->priority; });

        // 渲染
       for(auto& comp : queue)
       {
            comp->renderID = instanceID++;
            comp->render();
       }
       instanceID = 0;
            

        glfwSwapBuffers(window);
    }

    void UIWindow::CollectComponent(UIComponent *comp, std::vector<UIComponent *> &queue)
    {
        queue.push_back(comp);
        for (const auto &child : comp->childrens)
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
            case Button:
                child->SetShader(this->btnShader);
                break;
            default:
                break;
            }
            UpdateComponent(child);
        }
        comp->childChange = false;
    }

    void UIWindow::OnEvent(Event &e)
    {
        // 处理ui控件onevent
        for (auto &comp : ui_eventQ)
            if (comp->OnEvent(e))
            {
                std::println("事件被{}处理, type = {}", comp->renderID, comp->GetTypeName());
                e.handle = true;
                break;
            }

        // ui没有处理完
        if (!e.handle)
        {
            if (e.GetEventType() == EventType::WindowResize)
            {
                WindowResizeEvent wre = (WindowResizeEvent &)e;
                std::println("window resize: ({}x{})", wre.w, wre.h);
            }
        }
        // e.handle = true;
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