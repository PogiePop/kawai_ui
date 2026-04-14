#include "ui_window"
#include "ui_window"
#include <ui_window>
#include <ka_basic>
#include <algorithm>
#include <ka_time>
#include <event/event>
#include <event/event_window>
#include <event/event_mouse>
#include <event/event_ui>
#include <queue>
#include <Windows.h>

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

            //UI resize;
            UISizeEvent uise(width, height);
            OnEvent(uise);
        };

        m_CursorCall = [this](GLFWwindow *, double x, double y)
        {
            MouseMoveEvent mme(x, y);
            OnEvent(mme);
        };

        m_MSPressCall = [this](GLFWwindow* , int btn, int action, int mods)
        {
            MouseButtonPressEvent mspe(btn);
            OnEvent(mspe);
        };

        m_MSReleaseCall = [this](GLFWwindow* , int btn, int action, int mods)
        {
            MouseButtonReleaseEvent msre(btn);
            OnEvent(msre);
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
               // std::println("{}X{}", x, y);
            UIWindow* self = (UIWindow*)glfwGetWindowUserPointer(window);
            if(self)
            {
                if(self->m_CursorCall)self->m_CursorCall(window, x, y);
            } });
        glfwSetMouseButtonCallback(window, [](GLFWwindow* window, int button, int action, int mods){
            UIWindow* self = (UIWindow*)glfwGetWindowUserPointer(window);
            if(self)
            {
                if(action == GLFW_PRESS && self->m_MSPressCall)self->m_MSPressCall(window, button, action, mods);
                if(action == GLFW_RELEASE && self->m_MSReleaseCall)self->m_MSReleaseCall(window, button, action, mods);
            }
        });

        glfwSetWindowContentScaleCallback(window,
            [](GLFWwindow* window, float xscale, float yscale)
            {
                std::println("DPI scale: {}, {}", xscale, yscale);
            });
    }

    // 完全抛弃 GLFW 回调的脏数据！自己算！
    double getRealMousePos(GLFWwindow* window, double& outX, double& outY)
    {
        // 1. 获取鼠标在【整个屏幕】上的坐标
        double screenMouseX, screenMouseY;
        glfwGetCursorPos(window, &screenMouseX, &screenMouseY);

        // 2. 获取窗口【在屏幕上的左上角坐标】
        int winPosX, winPosY;
        glfwGetWindowPos(window, &winPosX, &winPosY);

        // 3. 自己算：客户区坐标 = 屏幕坐标 - 窗口坐标
        // 这是 100% 正确、永远不漂移、不受 GLFW BUG 影响
        outX = screenMouseX - winPosX;
        outY = screenMouseY - winPosY;

        return true;
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

   
    void UIWindow::OnRender()
    {
        glClearColor(color.r, color.g, color.b, color.a);
        glClear(GL_COLOR_BUFFER_BIT);
        

        

        // 构建队列
        if (needSort || uiNumChange)
        {
            ui_renderQ.clear();
            ui_eventQ.clear();
            // 构建渲染队列
            std::queue<UIComponent *> ui_queue;
            for (auto &comp : ui_components)
                ui_queue.push(comp);

            while (!ui_queue.empty())
            {
                auto q = ui_queue.front();
                ui_queue.pop();
                ui_renderQ.push_back(q);
                for (auto &child : q->childrens)
                    ui_queue.push(child);
            }

            //首先按照priority排序，然后按照添加顺序
            std::sort(ui_renderQ.begin(), ui_renderQ.end(), [](UIComponent* a, UIComponent* b){
                if(a->priority != b->priority)
                    return a->priority < b->priority;
                return a->number < b->number;
            });

            ui_eventQ.assign(ui_renderQ.rbegin(), ui_renderQ.rend());

            needSort = false;
            uiNumChange = false;
        }

        // 更新组件
        for (auto& comp : ui_renderQ)
        {
            if (comp->childChange)
            {
                //ApplyLayout();
                UpdateComponent();
                break;
            }
        }

        if (needResize)
        {
            //std::println("resize ui");
            for (auto& comp : ui_renderQ)comp->UpdateComponentSize();
            //ApplyLayout();
            needResize = false;
        }

        for(auto& r : ui_renderQ)
        {
            r->render();
            r->renderID = instanceID++;
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

    void UIWindow::UpdateComponentShader(UIComponent* comp)
    {
        switch (comp->GetComponentType())
        {
        case Rect:
            comp->SetShader(this->defaultUIShader);
            break;
        case Panel:
            comp->SetShader(this->radiusShader);
            break;
        case Button:
            comp->SetShader(this->btnShader);
            break;
        case Text:
        {
            ((UIText*)comp)->SetFontShader(this->textShader);
            comp->SetShader(this->defaultUIShader);
        }
        break;
        default:
            break;
        }
    }

    void UIWindow::UpdateComponent()
    {
        for (auto& comp : ui_renderQ)
        {
            UpdateComponentShader(comp);
            comp->childChange = false;
        }
    }

    void UIWindow::OnEvent(Event &e)
    {
        // 处理ui控件onevent
        for (auto &comp : ui_eventQ)
            if (comp->OnEvent(e))
            {
                //std::println("事件被{}处理, type = {}", comp->renderID, comp->GetTypeName());
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

    void UIWindow::SetLayout(UILayout* layout)
    {
        m_Layout = layout;
    }

    void UIWindow::ApplyLayout()
    {
        if (!m_Layout)return;
        std::println("apply()");
        int w, h;
        glfwGetWindowSize(window, &w, &h);
        m_Layout->ApplyLayout(ui_components, w, h);
    }

    glm::vec2 UIWindow::GetNativeMonitorVideoSize()
    {
        auto monitor = glfwGetPrimaryMonitor();
        int x, y, ww, hh;
        glfwGetMonitorWorkarea(monitor, &x, &y, &ww, &hh);
        return {  ww, hh };
    }



}