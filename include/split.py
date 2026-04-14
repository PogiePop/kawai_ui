import re
import os

# ====================== 【只改这一行！】 ======================
# 把这里改成你 真正的头文件名！！！
INPUT_FILE = "ui_component"
# ==============================================================

OUTPUT_DIR = "./"  # 直接输出到当前文件夹
os.makedirs(OUTPUT_DIR, exist_ok=True)

# 读取文件
try:
    with open(INPUT_FILE, "r", encoding="utf-8") as f:
        content = f.read()
except FileNotFoundError:
    print(f"\n❌ 错误：找不到文件 {INPUT_FILE}")
    print("✅ 请把 INPUT_FILE 改成你当前文件夹里的真实文件名！")
    print("📁 当前目录文件：", os.listdir("./"))
    exit()

# 抽取父类 UIComponent
header_parts = []
in_comp = False
for line in content.splitlines():
    if "class UIComponent" in line:
        in_comp = True
    if in_comp:
        header_parts.append(line)
    if in_comp and line.strip() == "};" and not "{" in line:
        break

header_content = "\n".join(header_parts)

# 写入父类
with open("kawai_ui_component.h", "w", encoding="utf-8") as f:
    f.write("""#ifndef KAWAI_UI_COMPONENT
#define KAWAI_UI_COMPONENT
#include <shader>
#include <mesh>
#include <ui_render>
#include <print>
#include <ui_colors>
#include <functional>
#include <ka_font>

namespace Kawai
{

class Event;
class UIFocusEvent;
class UIBlurEvent;
class UIWindow;
enum ComponentType
{
    Rect,
    Panel,
    Button,
    Text
};

#define COMPONENT_TYPE_DEF(type)                                                        \\
    static ComponentType GetStaticType() { return ComponentType::type; }                \\
    virtual ComponentType GetComponentType() const override { return GetStaticType(); } \\
    virtual const char *GetTypeName() const override { return #type; }

""")
    f.write(header_content)
    f.write("\n}\n#endif\n")

# 抽取子类
def extract_class(class_name):
    pattern = re.compile(r'class ' + re.escape(class_name) + r'\s*:.+?};', re.DOTALL)
    match = pattern.search(content)
    return match.group(0) if match else ""

# 抽取 Align 枚举
align = ""
align_match = re.search(r'enum Align\s*\{.+?\};', content, re.DOTALL)
if align_match:
    align = align_match.group(0)

# 保存子类
def save(name, code, extra=""):
    guard = f"KAWAI_UI_{name.split('_')[-1].upper()}"
    filename = f"kawai_ui_{name.split('_')[-1].lower()}.h"
    with open(filename, "w", encoding="utf-8") as f:
        f.write(f"#ifndef {guard}\n#define {guard}\n#include \"kawai_ui_component.h\"\n\n")
        if extra:
            f.write(extra + "\n\n")
        f.write("namespace Kawai\n{\n\n")
        f.write(code)
        f.write("\n}\n#endif\n")

save("UIRect", extract_class("UIRect"))
save("UIPanel", extract_class("UIPanel"))
save("UIButton", extract_class("UIButton"))
save("UIText", extract_class("UIText"), align)

print("\n✅ 拆分成功！生成文件：")
print(" - kawai_ui_component.h")
print(" - kawai_ui_rect.h")
print(" - kawai_ui_panel.h")
print(" - kawai_ui_button.h")
print(" - kawai_ui_text.h")