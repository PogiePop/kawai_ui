#include <ui_component>
#include <print>
#include <ui_window>

namespace Kawai
{
        
    void UIRect::render()const
    {   
        this->ui_shader->use();
        this->ui_shader->SetVec4("color", color);
        UIRender::GetInstance().DrawElements(GL_TRIANGLES, this->ui_mesh.vao, this->ui_mesh.indices.size());
    }

    void UIPanel::render()const
    {
        this->ui_shader->use();
        this->ui_shader->SetVec4("uColor", bgColor);
        this->ui_shader->SetFloat1("uRadius", ndc_radius);
        UIRender::GetInstance().DrawElements(GL_TRIANGLES, this->ui_mesh.vao, this->ui_mesh.indices.size());
        for(const auto& child : childrens)
            UIRender::GetInstance().DrawElements(GL_TRIANGLES, child->ui_mesh.vao, child->ui_mesh.indices.size());
    }

    void UIComponent::SetPriority(int order)
    {
        this->priority = order;
        m_Window->needSort = true;
    }

                
}