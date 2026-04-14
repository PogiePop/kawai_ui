import re
import os

# ====================== 配置项 ======================
INPUT_CPP_FILE = "ui_components.cpp"  # 你的原始实现文件
OUTPUT_DIR = "./"                   # 输出目录
# ====================================================

def save_file(path, content):
    with open(path, "w", encoding="utf-8") as f:
        f.write(content)

def main():
    if not os.path.exists(INPUT_CPP_FILE):
        print(f"❌ 错误：找不到文件 {INPUT_CPP_FILE}")
        return

    with open(INPUT_CPP_FILE, "r", encoding="utf-8") as f:
        code = f.read()

    # 公共头
    common_includes = """#include <ui_component>
#include <print>
#include <ui_window>
#include <ka_time>
#include <glm/gtc/matrix_transform.hpp>
#include <event/event>
#include <event/event_dispatch>
#include <event/event_ui>
#include <event/event_mouse>
#include <xutility>

namespace Kawai
{"""

    # ==============================================
    # 1. 提取 UIComponent 实现
    # ==============================================
    component_impl = []
    template_instantiate = ""

    comp_pattern = re.compile(
        r"(void UIComponent::SetPriority.+?)"
        r"(template void UIComponent::AddChildComponent<.+?>\s*\})?",
        re.DOTALL
    )
    comp_match = comp_pattern.search(code)
    if comp_match:
        component_impl.append(comp_match.group(1).strip())

    # 提取模板显式实例化
    template_pattern = re.compile(r"template void UIComponent::AddChildComponent.+?;", re.DOTALL)
    template_matches = template_pattern.findall(code)
    if template_matches:
        template_instantiate = "\n\n" + "\n".join(template_matches)

    component_code = f"{common_includes}\n"
    if component_impl:
        component_code += "\n".join(component_impl) + "\n"
    component_code += template_instantiate + "\n}"
    save_file(os.path.join(OUTPUT_DIR, "kawai_ui_component.cpp"), component_code)

    # ==============================================
    # 2. 提取 UIRect
    # ==============================================
    rect_pattern = re.compile(r"(void UIRect::render.+?bool UIRect::OnEvent.+?\}\s*\})", re.DOTALL)
    rect_match = rect_pattern.search(code)
    if rect_match:
        rect_code = f"""#include "kawai_ui_rect.h"
#include <ui_render>
#include <glm/gtc/matrix_transform.hpp>
#include <event/event_mouse>
#include <event/event_dispatch>

namespace Kawai
{{

{rect_match.group(1).strip()}

}}"""
        save_file(os.path.join(OUTPUT_DIR, "kawai_ui_rect.cpp"), rect_code)

    # ==============================================
    # 3. 提取 UIPanel
    # ==============================================
    panel_pattern = re.compile(r"(void UIPanel::render.+?bool UIPanel::OnEvent.+?\}\s*\})", re.DOTALL)
    panel_match = panel_pattern.search(code)
    if panel_match:
        panel_code = f"""#include "kawai_ui_panel.h"
#include <ui_render>
#include <glm/gtc/matrix_transform.hpp>
#include <event/event_mouse>
#include <event/event_dispatch>

namespace Kawai
{{

{panel_match.group(1).strip()}

}}"""
        save_file(os.path.join(OUTPUT_DIR, "kawai_ui_panel.cpp"), panel_code)

    # ==============================================
    # 4. 提取 UIButton
    # ==============================================
    button_pattern = re.compile(
        r"(void UIButton::update.+?bool UIButton::OnEvent.+?\}\s*\})",
        re.DOTALL
    )
    button_match = button_pattern.search(code)
    if button_match:
        button_code = f"""#include "kawai_ui_button.h"
#include <ui_render>
#include <ka_time>
#include <glm/gtc/matrix_transform.hpp>
#include <event/event_mouse>
#include <event/event_dispatch>
#include <print>

namespace Kawai
{{

{button_match.group(1).strip()}

}}"""
        save_file(os.path.join(OUTPUT_DIR, "kawai_ui_button.cpp"), button_code)

    # ==============================================
    # 5. 提取 UIText
    # ==============================================
    text_pattern = re.compile(
        r"(bool UIText::OnEvent.+?void UIText::DrawLine.+?\}\s*\})",
        re.DOTALL
    )
    text_match = text_pattern.search(code)
    if text_match:
        text_code = f"""#include "kawai_ui_text.h"
#include <ui_render>
#include <glm/gtc/matrix_transform.hpp>
#include <print>

namespace Kawai
{{

{text_match.group(1).strip()}

}}"""
        save_file(os.path.join(OUTPUT_DIR, "kawai_ui_text.cpp"), text_code)

    print("✅ 分割完成！生成以下文件：")
    print("   - kawai_ui_component.cpp")
    print("   - kawai_ui_rect.cpp")
    print("   - kawai_ui_panel.cpp")
    print("   - kawai_ui_button.cpp")
    print("   - kawai_ui_text.cpp")

if __name__ == "__main__":
    main()