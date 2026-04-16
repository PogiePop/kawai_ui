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
#include <xutility>
#include <event/event_keyboard>

namespace Kawai
{

    bool UIComponent::IsPointInside(float x, float y)
    {
        glm::vec2 worldPos = GetWorldPos() * ui_scale;
        int screenH = GetScreenHeight();

        // 关键：把 GLFW 鼠标 Y 翻转成 UI 坐标系 Y
        float uiMouseY = screenH - y;
        return (x > worldPos.x  && x < worldPos.x + w * ui_scale) &&
            (uiMouseY > worldPos.y && uiMouseY < worldPos.y + h * ui_scale);
    }

    void UIComponent::InitMesh()
    {
        auto mode = UIWindow::GetNativeMonitorVideoSize();
        ui_scale = GetScreenWidth() / mode.x;
        ui_mesh = CreateUIRectMesh(0, 0, w * ui_scale, h * ui_scale);
    }

    void UIComponent::TransformToNDCP()
    {
        glm::vec2 point = toNDC(glm::vec2{ x, y });
        float width = toNDCSizeHorizontal(w);
        float height = toNDCSizeVertical(h);
        this->ndc_x = point.x;
        this->ndc_y = point.y;
        this->ndc_w = width;
        this->ndc_h = height;
    }

    void UIRect::render()
    {
        glm::vec2 worldPos = GetWorldPos();
        this->ui_shader->use();
        this->ui_shader->SetVec4("color", color);
        glm::mat4 pro = glm::ortho(0.0f, (float)GetScreenWidth(), 0.0f, (float)GetScreenHeight());
        this->ui_shader->SetMat4("pro", pro);
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(worldPos * ui_scale, 0.0f));
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
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(worldPos*ui_scale, 0.0f));
        this->ui_shader->SetMat4("model", model);
        UIRender::GetInstance().DrawElements(GL_TRIANGLES, this->ui_mesh.vao, this->ui_mesh.indices.size());
    }

    void UIComponent::SetPriority(int order)
    {
        this->priority = order;
        m_Window->needSort = true;
    }

    glm::vec2 UIComponent::toNDC(const glm::vec2& p)
    {
        glm::vec2 res;
        res.x = p.x / GetScreenWidth() * 2 - 1;
        res.y = p.y / GetScreenHeight() * 2 - 1;
        return res;
    }

    float UIComponent::toNDCSizeHorizontal(float x)
    {
        return x / (float)GetScreenWidth() * 2.0f;
    }

    float UIComponent::toNDCSizeVertical(float y)
    {
        return y / (float)GetScreenHeight() * 2.0f;
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
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(worldPos * ui_scale, 0.0f));
        //std::println("{}X{}", worldPos.x * ui_scale, worldPos.y * ui_scale);
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

    void UIComponent::UpdateComponentSize()
    {
        TransformToNDCP();
        ModifyUIRectMesh(this->ui_mesh, x, y, w, h);
        for (auto& child : childrens)
            child->UpdateComponentSize();
    }

    void UIComponent::AttachToWindow(UIWindow *window)
    {
        if (!window)return;
        this->m_Window = window;
        this->number = this->m_Window->componentNum;
        for (auto &child : childrens)
            child->AttachToWindow(window);
    }

    glm::vec2 UIComponent::GetWorldPos() const
    {
        glm::vec2 worldPos = { x, y };

        // 递归向上累加所有父节点坐标
        const UIComponent* p = this->parent;
        while (p != nullptr)
        {
            worldPos.x += p->x;
            worldPos.y += p->y;
            p = p->parent;
        }

        return worldPos;
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
        dispatcher.Dispatch<UISizeEvent>([&, this](UISizeEvent& e) {
            auto mode = UIWindow::GetNativeMonitorVideoSize();
            this->ui_scale = (float)GetScreenWidth() / mode.x;
            ModifyUIRectMesh(ui_mesh, 0, 0, w * ui_scale, h * ui_scale);
            //UISizeEvent不会拦截
            return false;
            });
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
        dispatcher.Dispatch<UISizeEvent>([&, this](UISizeEvent& e) {
            auto mode = UIWindow::GetNativeMonitorVideoSize();
            this->ui_scale = (float)GetScreenWidth() / mode.x;
            ModifyUIRectMesh(ui_mesh, 0, 0, w * ui_scale, h * ui_scale);
            //UISizeEvent不会拦截
            return false;
            });
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
                std::println("hover!!");
                //如果设置了聚焦函数
                if(focus)focus(fe);
            }
            else if(!inside && focused)
            {
                focused = false;
                state = Common;
                UIBlurEvent be;
                std::println("blur!!");
                //如果设置了失焦函数
                if(blur)blur(be);
            }
            return inside; });

        dispatcher.Dispatch<UISizeEvent>([&, this](UISizeEvent& e) {
            auto mode = UIWindow::GetNativeMonitorVideoSize();
            this->ui_scale = (float)GetScreenWidth() / mode.x;
            ModifyUIRectMesh(ui_mesh, 0, 0, w * ui_scale, h * ui_scale);
            //UISizeEvent不会拦截
            return false;
            });

        dispatcher.Dispatch<MouseButtonPressEvent>([this](MouseButtonPressEvent &e)
                                                   {
            if(focused)
            {
                state = Click;
                if(click)click();
                std::println("click!!");
            }
            return focused; });

        dispatcher.Dispatch<MouseButtonReleaseEvent>([this](MouseButtonReleaseEvent &e)
                                                     {
            if(focused)
            {
                state = Hover;
                std::println("Release!!");
            }
            return focused; });

        return e.handle;
    }

    bool UIText::OnEvent(Event& e)
    {
        EventDispatch dispatcher(e);
        dispatcher.Dispatch<UISizeEvent>([&, this](UISizeEvent& e) {
            auto mode = UIWindow::GetNativeMonitorVideoSize();
            this->ui_scale = (float)GetScreenWidth() / mode.x;
            ModifyUIRectMesh(ui_mesh, 0, 0, w * ui_scale, h * ui_scale);
            m_ContentSize = { 2 * (float)w * ui_scale / 3, 2 * (float)h * ui_scale / 3 };
            UpdateInnerSpacing();
            CalculateFitFontSize();
            float scale = (float)m_FitFontSize / m_Font->GetFontSize();
            SplitTextIntoLines(scale);
            //std::println("当前字体大小:{}", m_FitFontSize);
            //UISizeEvent不会拦截
            return false;
            });
        return false;
    }

    void UIText::render()
    {
        if (!visible || m_Text.empty() || !m_Font) return;

        glm::vec2 world = GetWorldPos();
        float baseScale = (float)m_FitFontSize / this->m_Font->GetFontSize();
        float contentBottom = world.y + m_bottomSpacing;
        auto totalH = GetTotalLineHeight(baseScale);
        float startY = CalculateAlignY(totalH);
        /*if(number == 3)
        std::println("startY:{}", startY);*/
        glm::mat4 proj = glm::ortho(
            0.0f, (float)GetScreenWidth(),
            0.0f, (float)GetScreenHeight()
        );
        ////绘制边框
        ui_shader->use();
        ui_shader->SetMat4("model", glm::translate(glm::mat4(1.0f), { world.x * ui_scale, world.y * ui_scale, 0.0f }));
        ui_shader->SetMat4("pro", proj);
        ui_shader->SetVec4("color", color_black);

        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        
        UIRender::GetInstance().DrawElements(
            GL_TRIANGLES,
            ui_mesh.vao,
            ui_mesh.indices.size()
        );
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);

        m_FontShader->use();
        m_FontShader->SetMat4("pro", proj);
        m_FontShader->SetVec4("textColor", color);
 
        //std::println("bottomSpacing={}, totalH={}, contentY={}", m_bottomSpacing, totalH, m_ContentSize.y);
        for (int i = 0; i < m_lines.size(); i++)
        {
            auto& line = m_lines[i];
            auto lineMetrics = GetLineMetrics(line, baseScale);
            float lineW = lineMetrics.x;
            float lineH = GetLineHeight(baseScale);

            float penX = world.x * ui_scale + CalculateAlignX(lineW);
            float penY = world.y * ui_scale + startY - i * (lineH + m_lineSpacing * ui_scale);
            DrawLine(penX, penY, line, baseScale);
        }
    }

    void UIText::InitMesh()
    {
        m_FontMesh = CreateUnitQuad();
        auto mode = UIWindow::GetNativeMonitorVideoSize();
        ui_scale = GetScreenWidth() / mode.x;
        ui_mesh = CreateUIRectMesh(0, 0, w * ui_scale, h * ui_scale);
        m_ContentSize = { 2 * (float)w * ui_scale / 3, 2 * (float)h * ui_scale / 3 };
        UpdateInnerSpacing();
        CalculateFitFontSize();
        float scale = (float)m_FitFontSize / m_Font->GetFontSize();

        SplitTextIntoLines(scale); // ✅ 最终一次

        //std::println("当前字体大小:{}", m_FitFontSize);
    }

    void UIText::CalculateFitFontSize()
    {
        for (uint32_t sz = m_FontSize; sz > 0; sz--)
        {
            float scale = (float)sz / this->m_Font->GetFontSize();
            SplitTextIntoLines(scale);
            float totalH = GetTotalLineHeight(scale);
            float totalW = 0.0f;
            for (auto& line : m_lines)
                totalW = std::max(totalW, GetLineMetrics(line, scale).x);
            if (totalH <= m_ContentSize.y && totalW <= m_ContentSize.x)
            {
                m_FitFontSize = sz;
                return;
            }
        }
    }

    void UIText::UpdateInnerSpacing()
    {
        // 总可用区域 = 组件大小
        float totalW = w * ui_scale;
        float totalH = h * ui_scale;

        // 内容区域（文本实际占用的宽高，已经计算好）
        float contentW = m_ContentSize.x;
        float contentH = m_ContentSize.y;

        // 剩余空间
        float freeW = totalW - contentW;
        float freeH = totalH - contentH;



        switch (m_Align)
        {
            // ==================== 顶部 ====================
        case Align::TOP_LEFT:
            m_leftSpacing = 0.0f;
            m_rightSpacing = freeW;
            m_topSpacing = 0.0f;
            m_bottomSpacing = freeH;
            break;

        case Align::TOP_CENTER:
            m_leftSpacing = freeW * 0.5f;
            m_rightSpacing = freeW * 0.5f;
            m_topSpacing = 0.0f;
            m_bottomSpacing = freeH;
            break;

        case Align::TOP_RIGHT:
            m_leftSpacing = freeW;
            m_rightSpacing = 0.0f;
            m_topSpacing = 0.0f;
            m_bottomSpacing = freeH;
            break;

            // ==================== 中间 ====================
        case Align::MIDDLE_LEFT:
            m_leftSpacing = 0.0f;
            m_rightSpacing = freeW;
            m_topSpacing = freeH * 0.5f;
            m_bottomSpacing = freeH * 0.5f;
            break;

        case Align::CENTER:
            m_leftSpacing = freeW * 0.5f;
            m_rightSpacing = freeW * 0.5f;
            m_topSpacing = freeH * 0.5f;
            m_bottomSpacing = freeH * 0.5f;
            break;

        case Align::MIDDLE_RIGHT:
            m_leftSpacing = freeW;
            m_rightSpacing = 0.0f;
            m_topSpacing = freeH * 0.5f;
            m_bottomSpacing = freeH * 0.5f;
            break;

            // ==================== 底部 ====================
        case Align::BOTTOM_LEFT:
            m_leftSpacing = 0.0f;
            m_rightSpacing = freeW;
            m_topSpacing = freeH;
            m_bottomSpacing = 0.0f;
            break;

        case Align::BOTTOM_CENTER:
            m_leftSpacing = freeW * 0.5f;
            m_rightSpacing = freeW * 0.5f;
            m_topSpacing = freeH;
            m_bottomSpacing = 0.0f;
            break;

        case Align::BOTTOM_RIGHT:
            m_leftSpacing = freeW;
            m_rightSpacing = 0.0f;
            m_topSpacing = freeH;
            m_bottomSpacing = 0.0f;
            break;

        default:
            // 默认居中
            m_leftSpacing = m_rightSpacing = freeW * 0.5f;
            m_topSpacing = m_bottomSpacing = freeH * 0.5f;
            break;
        }
    }

    glm::vec2 UIText::GetLineMetrics(const std::string& line, float scale)
    {
        if (!m_Font || line.empty()) return { 0, 0 };

        float penX = 0.0f;
        float firstBearingX = 0;
        float maxBearingY = 0;

        for (size_t i = 0; i < line.size(); i++)
        {
            auto ch = m_Font->GetCharacter(line[i]);
            if (ch.empty()) continue;

            // 记录第一个字符的 bearing.x（左边突出）
            if (i == 0) firstBearingX = ch.bearing.x * scale;

            // 记录整行最高的 bearing.y
            maxBearingY = std::max(maxBearingY, ch.bearing.y * scale);

            // 累加步进（advance 不含任何 bearing）
            penX += (ch.advance >> 6) * scale;
            if (i != line.size() - 1) penX += m_letterSpacing;
        }

        // ✅ 核心：advance 不包含 bearing.x，必须手动补偿突出
        float width = penX + firstBearingX;
        float height = maxBearingY;

        return { width, height };
    }

    float UIText::GetLineHeight(float scale)
    {
        if (!m_Font) return 0.0f;

        // FreeType 行高（baseline -> baseline）
        float lineHeight = m_Font->GetFontFace()->size->metrics.height / 64.0f;

        return lineHeight * scale;
    }

    float UIText::GetTotalLineHeight(float scale)
    {
        if (!m_Font) return 0.0f;

        size_t lineCount = m_lines.size();
        if (lineCount == 0) return 0.0f;

        float lineHeight = GetLineHeight(scale);

        return lineCount * lineHeight + (lineCount - 1) * m_lineSpacing * scale;
    }

    void UIText::SplitTextIntoLines(float scale)
    {
        if (!m_Font || m_Text.empty())return;
        m_lines.clear();
        std::string line = "";
        float currentWidth = 0.0f;
        for (auto c : m_Text)
        {
            Character ch = m_Font->GetCharacter(c);
            if (ch.empty())continue;
            float wx = (ch.advance >> 6) * scale;
            if (currentWidth + wx <= m_ContentSize.x)
            {
                currentWidth += wx;
                line += c;
            }
            else
            {
                m_lines.push_back(line);
                currentWidth = wx;
                line = c;
            }
        }

        if (!line.empty())
            m_lines.push_back(line);

    }


    // 水平对齐（自动包含 leftSpacing）
    float UIText::CalculateAlignX(float lineW)
    {
        float contentX = m_leftSpacing;

        switch (m_Align)
        {
        case Align::TOP_LEFT:
        case Align::MIDDLE_LEFT:
        case Align::BOTTOM_LEFT:
            return contentX;

        case Align::TOP_CENTER:
        case Align::CENTER:
        case Align::BOTTOM_CENTER:
            return contentX + (m_ContentSize.x - lineW) * 0.5f;

        case Align::TOP_RIGHT:
        case Align::MIDDLE_RIGHT:
        case Align::BOTTOM_RIGHT:
            return contentX + m_ContentSize.x - lineW;

        default: return contentX;
        }
    }

    float UIText::CalculateAlignY(float totalH)
    {
        float contentTop = m_bottomSpacing;

        switch (m_Align)
        {
            // 底部对齐：文字贴着内容区域底部
        case Align::BOTTOM_LEFT:
        case Align::BOTTOM_CENTER:
        case Align::BOTTOM_RIGHT:
            return contentTop + totalH;

            // 居中：底部 + (内容高度 - 文本总高度)/2
        case Align::MIDDLE_LEFT:
        case Align::CENTER:
        case Align::MIDDLE_RIGHT:
            return contentTop + (m_ContentSize.y - totalH) * 0.5f + +totalH;

            // 顶部对齐：底部 + (内容高度 - 文本总高度)
        case Align::TOP_LEFT:
        case Align::TOP_CENTER:
        case Align::TOP_RIGHT:
            return contentTop + m_ContentSize.y;

        default:
            return contentTop;
        }
    }

    void UIText::DrawLine(float x, float y, const std::string& line, float scale)
    {
        float penX = x;
     
        for (int i = 0; i < line.size(); i++)
        {
            char c = line[i];
            auto ch = m_Font->GetCharacter(c);
            if (!ch.tex_ID) continue;
            //std::println("{}", y);


            float xpos = penX + ch.bearing.x * scale;
            float ypos = y - ch.size.y * scale;
            float w = ch.size.x * scale;
            float h = ch.size.y * scale;

            glm::mat4 model = glm::mat4(1.0f);
            model = glm::translate(model, { xpos, ypos, 0 });
            model = glm::scale(model, { w, h, 1 });

            m_FontShader->SetMat4("model", model);
            glBindTexture(GL_TEXTURE_2D, ch.tex_ID);
            UIRender::GetInstance().DrawElements(
                GL_TRIANGLES,
                m_FontMesh.vao,
                m_FontMesh.indices.size());

            penX += (ch.advance >> 6) * scale;
            if (i != line.size() - 1) penX += m_letterSpacing * scale;
        }
    }

    void UITextBox::InitMesh()
    {
        auto mode = UIWindow::GetNativeMonitorVideoSize();
        ui_scale = GetScreenWidth() / mode.x;
        ui_mesh = CreateUIRectMesh(0, 0, w * ui_scale, h * ui_scale);
        m_FontMesh = CreateUnitQuad();
        m_CursorMesh = CreateUIRectMesh(0, 0, 2.0f * ui_scale, h * ui_scale);
    }

    void UITextBox::render()
    {

        
        glm::vec2 worldPos = GetWorldPos();
        this->ui_shader->use();
        this->ui_shader->SetVec4("color", glm::vec4(1.0f));
        glm::mat4 pro = glm::ortho(0.0f, (float)GetScreenWidth(), 0.0f, (float)GetScreenHeight());
        this->ui_shader->SetMat4("pro", pro);
        glm::mat4 model = glm::translate(glm::mat4(1.0f), glm::vec3(worldPos * ui_scale, 0.0f));
        this->ui_shader->SetMat4("model", model);
        UIRender::GetInstance().DrawElements(GL_TRIANGLES, this->ui_mesh.vao, this->ui_mesh.indices.size());

        if (!m_Font || text.empty() || !m_FontShader)return;
        this->m_FontShader->use();
        this->m_FontShader->SetVec4("textColor", color_red);
        //开启ui裁剪
        glEnable(GL_SCISSOR_TEST);
        glScissor(worldPos.x * ui_scale, worldPos.y * ui_scale, w * ui_scale, h * ui_scale);
        //绘制字符
        float penX = worldPos.x * ui_scale;
        //std::println("gly:{}, vis:{}", glyphPos.size(), visibaleGlyph.size());
        for (int i = 0; i < visibaleGlyph.size(); ++i)
        {
            auto ch = m_Font->GetCharacter(text[visibaleGlyph[i]]);
            float penY = (worldPos.y - (ch.size.y - ch.bearing.y)) * ui_scale;
            float x = penX + (glyphPos[visibaleGlyph[i]] + ch.bearing.x) * ui_scale;
            model = glm::mat4(1.0f);
            model = glm::translate(model, glm::vec3(x - viewOffsetX * ui_scale, penY, 0.0f));
            model = glm::scale(model, glm::vec3(ch.size.x*ui_scale, ch.size.y * ui_scale, 1.0f));
            this->m_FontShader->SetMat4("pro", pro);
            this->m_FontShader->SetMat4("model", model);
            glActiveTexture(GL_TEXTURE0);
            glBindTexture(GL_TEXTURE_2D, ch.tex_ID);
            this->m_FontShader->SetInt1("text", 0);
            UIRender::GetInstance().DrawElements(
                GL_TRIANGLES,
                m_FontMesh.vao,
                m_FontMesh.indices.size()
            );
           /* penX += (ch.advance >> 6) * ui_scale;*/
        }

        if (focused && showCursor)
        {
            // 更新闪烁计时器
            cursorBlink += Time::GetInstance().deltatime;


            // 闪烁周期 0.53 秒（显示 265ms，隐藏 265ms）
            const float blinkInterval = 0.53f;
            if (cursorBlink >= blinkInterval)
            {
                cursorBlink = 0.0f;
            }
            if (cursorBlink < blinkInterval * 0.5f) {
                float cx = GetCursorX();
                glm::vec2 wp = GetWorldPos();

                float cursorX = (wp.x + cx - viewOffsetX) * ui_scale;
                float cursorY = wp.y * ui_scale;

                cursorX = std::round(cursorX);
                cursorY = std::round(cursorY);

                this->m_CursorShader->use();
                this->m_CursorShader->SetVec4("color", { 1.0f, 1.0f, 1.0f, 1.0f });
                this->m_CursorShader->SetMat4("pro", pro);

                model = glm::translate(glm::mat4(1.0f),
                    glm::vec3(cursorX, cursorY, 0.0f));
                this->m_CursorShader->SetMat4("model", model);
                UIRender::GetInstance().DrawElements(GL_TRIANGLES,
                    m_CursorMesh.vao,
                    m_CursorMesh.indices.size()
                );
            }
        }

        glDisable(GL_SCISSOR_TEST);
    }

    bool UITextBox::OnEvent(Event& e)
    {
        EventDispatch dispatcher(e);
        //鼠标点击
        dispatcher.Dispatch<MouseButtonPressEvent>([this](MouseButtonPressEvent& e) {
            bool inside = IsPointInside(e.xPos, e.yPos);
            focused = inside;
            if (inside)
            {
                cursorBlink = 0.0f;  // ✅ 获得焦点时重置
            }
            return inside;
            });
       
        //输入字符
        dispatcher.Dispatch<KeyInputEvent>([this](KeyInputEvent& e) {
            if (focused)
            {
                if (isascii(e.button))
                {
                    text.insert(text.begin() + cursorIndex, (char)e.button);
                    ++cursorIndex;
                    RecalcGlyph(ui_scale);
                    UpdateView(w);
                    //AutoScrollToEnd();
                    
                    AutoScrollToCursor();
                    cursorBlink = 0.0f;  // ✅ 用户输入时重置
                   
                }
            }
            return focused;
            });

        dispatcher.Dispatch<KeyPressEvent>([this](KeyPressEvent& e) {
            if (focused)
            {
                if (e.button == GLFW_KEY_BACKSPACE)
                {
                    if (text.empty() || cursorIndex - 1 < 0)return focused;
                    text.erase(text.begin() + cursorIndex - 1);
                    cursorIndex--;
                    RecalcGlyph(ui_scale);
                    UpdateView(w);
                    //AutoScrollToEnd()
                    AutoScrollToCursor();
                    cursorBlink = 0.0f;  // ✅ 重置
                }

                // 左右方向键控制光标
                if (e.button == GLFW_KEY_LEFT)
                {
                    cursorIndex = glm::max(cursorIndex - 1, 0);
                    AutoScrollToCursor();
                    cursorBlink = 0.0f;  // ✅ 重置
                }
                if (e.button == GLFW_KEY_RIGHT)
                {
                    cursorIndex = glm::min(cursorIndex + 1, (int)text.size());
                    AutoScrollToCursor();
                }
            }
            return focused;
            });

        dispatcher.Dispatch<UISizeEvent>([this](UISizeEvent& e) {
            auto mode = UIWindow::GetNativeMonitorVideoSize();
            this->ui_scale = (float)GetScreenWidth() / mode.x;
            ModifyUIRectMesh(ui_mesh, 0, 0, w * ui_scale, h * ui_scale);
            ModifyUIRectMesh(m_CursorMesh, 0, 0, 2.0f * ui_scale, h * ui_scale);
            RecalcGlyph(ui_scale);
            UpdateView(w);
            //UISizeEvent不会拦截
            return false;
            });

        return e.handle;
    }


    void UITextBox::RecalcGlyph(float scale)
    {
        glyphPos.clear();
        float penX = 0.0f;
        for (int i = 0; i < text.size(); ++i)
        {
            auto ch = m_Font->GetCharacter(text[i]);
            //float x = penX + ch.bearing.x;
            glyphPos.push_back(penX);
            penX += (ch.advance >> 6);
        }
    }

    void UITextBox::UpdateView(float visibleWidth)
    {
        visibaleGlyph.clear();
        float viewLeft = viewOffsetX;
        float viewRight = viewLeft + visibleWidth;
        for (int i = 0; i < glyphPos.size(); ++i)
        {
            auto ch = m_Font->GetCharacter(text[i]);
            float x = glyphPos[i];
            float x1 = glyphPos[i] + ch.advance;
            if (x1 >= viewLeft && x <= viewRight)
            {
                visibaleGlyph.push_back(i);
            }
        }
    }

    void UITextBox::AutoScrollToEnd()
    {
        if (glyphPos.empty())
        {
            viewOffsetX = 0.0f;
            return;
        }
        auto ch = m_Font->GetCharacter(text[glyphPos.size() - 1]);
        float lastX = glyphPos.back();
        float visibleWidth = w * ui_scale;
        float offsetX = (lastX - visibleWidth) / ui_scale;
        if (offsetX < 0)
            offsetX = 0;
        viewOffsetX = offsetX;
    }

    void UITextBox::AutoScrollToCursor()
    {
        float cx = GetCursorX();
        float visibleW = w;

        if (cx > (viewOffsetX + w))
        {
            viewOffsetX = (cx - visibleW);
        }
        
        if (cx < viewOffsetX)
        {
            viewOffsetX = cx;
        }
        if (viewOffsetX < 0)
            viewOffsetX = 0;
        
        if (cursorIndex <= 0)
            viewOffsetX = 0;
    }

    float UITextBox::GetCursorX() const
    {
        if (cursorIndex <= 0 || glyphPos.empty())
            return 0.0f;

        auto ch = m_Font->GetCharacter(text[cursorIndex - 1]);
        return  glyphPos[cursorIndex - 1] + (ch.advance >> 6);
    }
    

    


    template void UIComponent::AddChildComponent<UIRect>(UIRect *);
    template void UIComponent::AddChildComponent<UIPanel>(UIPanel *);
    template void UIComponent::AddChildComponent<UIButton>(UIButton *);
    template void UIComponent::AddChildComponent<UIText>(UIText *);
    template void UIComponent::AddChildComponent<UITextBox>(UITextBox *);
}