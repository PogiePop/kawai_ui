#include <ui_component>
#include <print>
#include <ui_window>
#include <ka_time>
#include <glm/gtc/matrix_transform.hpp>
#include <ui_window>
#include <event/event>
#include <event/event_dispatch>
#include <event/event_ui>
#include <event/event_mouse>

namespace Kawai
{

    void UIRect::render()
    {
        glm::vec2 worldPos = GetWorldPos();
        this->ui_shader->use();
        this->ui_shader->SetVec4("color", color);
        glm::mat4 pro = glm::ortho(0.0f, (float)GetScreenWidth(), 0.0f, (float)GetScreenHeight());
        this->ui_shader->SetMat4("pro", pro);
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(worldPos, 0.0f));
        this->ui_shader->SetMat4("model", model);
        UIRender::GetInstance().DrawElements(GL_TRIANGLES, this->ui_mesh.vao, this->ui_mesh.indices.size());
    }

    void UIPanel::render()
    {
        glm::vec2 worldPos = GetWorldPos();
        this->ui_shader->use();
        this->ui_shader->SetVec4("uColor", bgColor);
        this->ui_shader->SetFloat1("uRadius", ndc_radius);
        glm::mat4 pro = glm::ortho(0.0f, (float)GetScreenWidth(), 0.0f, (float)GetScreenHeight());
        this->ui_shader->SetMat4("pro", pro);
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(worldPos, 0.0f));
        this->ui_shader->SetMat4("model", model);
        UIRender::GetInstance().DrawElements(GL_TRIANGLES, this->ui_mesh.vao, this->ui_mesh.indices.size());
    }

    void UIComponent::SetPriority(int order)
    {
        this->priority = order;
        m_Window->needSort = true;
    }

    void UIButton::update(float deltatime)
    {
        // 默认为common state
        glm::vec4 target_color = commonColor;
        float target_scale = scaleNormal;

        // 根据当前button状态进行改变
        if (state == Click)
        {
            target_color = clickColor;
            target_scale = scaleClick;
        }
        else if (state == Hover)
        {
            target_color = hoverColor;
            target_scale = scaleHover;
        }

        float smoothSpeed = 10.0f;

        float t = 1.0f - exp(-smoothSpeed * deltatime);

        currentColor = glm::mix(currentColor, target_color, t);
        scale = glm::mix(scale, target_scale, t);
    }

    void UIButton::render()
    {
        glm::vec2 worldPos = GetWorldPos();
        update(Time::GetInstance().deltatime);
        this->ui_shader->use();
        this->ui_shader->SetVec4("color", currentColor);
        this->ui_shader->SetFloat1("radius", ndc_radius);
        this->ui_shader->SetVec4("shadowColor", shadowColor);
        this->ui_shader->SetVec2("shadowOffset", shadowOffset);
        this->ui_shader->SetFloat1("shadowBlur", shadowBlur);
        glm::mat4 pro = glm::ortho(0.0f, (float)GetScreenWidth(), 0.0f, (float)GetScreenHeight());
        this->ui_shader->SetMat4("pro", pro);
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(worldPos, 0.0f));
        this->ui_shader->SetMat4("model", model);
        UIRender::GetInstance().DrawElements(GL_TRIANGLES, this->ui_mesh.vao, this->ui_mesh.indices.size());
    }

    template <std::derived_from<UIComponent> T>
    void UIComponent::AddChildComponent(T *child)
    {
        if (!this->m_Window)
            return;
        child->InitMesh();
        child->parent = this;
        this->childrens.push_back(child);
        childChange = true;
        if (this->m_Window)
            child->AttachToWindow(this->m_Window);
        this->m_Window->AddComponentCount();
    }

    void UIComponent::AttachToWindow(UIWindow *window)
    {
        this->m_Window = window;
        this->number = this->m_Window->componentNum;
        for (auto &child : childrens)
            child->AttachToWindow(window);
    }

    bool UIRect::OnEvent(Event &e)
    {
        EventDispatch dispatcher(e);
        dispatcher.Dispatch<MouseMoveEvent>([this](MouseMoveEvent &e)
                                            {
            //判断是否在控件内
            bool inside = IsPointInside((float)e.x, (float)e.y);
            if(inside && !focused)
            {
                focused = true;
                UIFocusEvent fe;
                //如果设置了聚焦函数
                //if(focus)(*focus)(fe);
            }
            else if(!inside && focused)
            {
                focused = false;
                UIBlurEvent be;
                //如果设置了失焦函数
                //if(blur)(*blur)(be);
            }
            return inside; });
        return e.handle;
    }

    bool UIPanel::OnEvent(Event &e)
    {
        EventDispatch dispatcher(e);
        dispatcher.Dispatch<MouseMoveEvent>([this](MouseMoveEvent &e)
                                            {
            //判断是否在控件内
            bool inside = IsPointInside((float)e.x, (float)e.y);
            if(inside && !focused)
            {
                focused = true;
                UIFocusEvent fe;
                //如果设置了聚焦函数
                //if(focus)(*focus)(fe);
            }
            else if(!inside && focused)
            {
                focused = false;
                UIBlurEvent be;
                //如果设置了失焦函数
                //if(blur)(*blur)(be);
            }
            return inside; });
        return e.handle;
    }

    bool UIButton::OnEvent(Event &e)
    {
        EventDispatch dispatcher(e);
        dispatcher.Dispatch<MouseMoveEvent>([this](MouseMoveEvent &e)
                                            {
            //判断是否在控件内
            bool inside = IsPointInside((float)e.x, (float)e.y);
            if(inside && !focused)
            {
                focused = true;
                state = Hover;
                UIFocusEvent fe;
                //如果设置了聚焦函数
                if(focus)(*focus)(fe);
            }
            else if(!inside && focused)
            {
                focused = false;
                state = Common;
                UIBlurEvent be;
                //如果设置了失焦函数
                if(blur)(*blur)(be);
            }
            return inside; });
        return e.handle;
    }

    template void UIComponent::AddChildComponent<UIRect>(UIRect *);
    template void UIComponent::AddChildComponent<UIPanel>(UIPanel *);
    template void UIComponent::AddChildComponent<UIButton>(UIButton *);
}