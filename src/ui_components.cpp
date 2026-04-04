#include <ui_component>
#include <print>
#include <ui_window>
#include <glm/gtc/matrix_transform.hpp>

namespace Kawai
{
        
    void UIRect::render()const
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

    void UIPanel::render()const
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
        for(const auto& child : childrens)
            UIRender::GetInstance().DrawElements(GL_TRIANGLES, child->ui_mesh.vao, child->ui_mesh.indices.size());
    }

    void UIComponent::SetPriority(int order)
    {
        this->priority = order;
        m_Window->needSort = true;
    }

                
}