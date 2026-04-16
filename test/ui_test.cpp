#include <ui_window>
#include <ui_component>
#include <print>

using namespace Kawai;

int main() {
    // 1. 创建UI主窗口（800x600）
    UIWindow window(800, 600, "Kawai UI 示例");
    window.SetWindowColor(glm::vec4(0.12f, 0.12f, 0.15f, 1.0f)); // 深灰背景

    // ==========================
    // 2. 创建根面板（作为容器）
    // ==========================
    UIPanel* panel = new UIPanel(
        100, 100,   // 坐标 x,y
        600, 350,   // 宽高 w,h
        glm::vec4(0.18f, 0.28f, 0.4f, 1.0f), // 背景色
        15.0f       // 圆角
    );
    window.AddComponent(panel);

    // ==========================
    // 3. 创建标题文本
    // ==========================
    Font* defaultFont = window.GetFontResources()["default"];
    UIText* titleText = new UIText(
        0, 240,      // 相对于父面板的坐标
        600, 60,     // 宽高
        defaultFont,
        "Kawai UI 演示",
        Align::CENTER,
        36           // 字体大小
    );
    titleText->SetFontColor(glm::vec4(1.0f, 1.0f, 1.0f, 1.0f));
    panel->AddChildComponent(titleText);

    // ==========================
    // 4. 创建功能按钮
    // ==========================
    UIButton* btn1 = new UIButton(100, 140, 180, 70, 12.0f);
    btn1->SetCommonColor(glm::vec4(0.2f, 0.6f, 0.3f, 1.0f));
    btn1->SetHoverColor(glm::vec4(0.3f, 0.75f, 0.4f, 1.0f));
    btn1->SetClickColor(glm::vec4(0.15f, 0.5f, 0.25f, 1.0f));
    // 按钮点击事件
    btn1->SetOnclick([]() {
        std::println("按钮 1 被点击！");
        });
    panel->AddChildComponent(btn1);

    // 按钮文字
    UIText* btn1Text = new UIText(0, 0, 180, 70, defaultFont, "点击按钮 1", Align::CENTER, 24);
    btn1Text->SetFontColor(glm::vec4(1.0f));
    btn1->AddChildComponent(btn1Text);

    // 第二个按钮
    UIButton* btn2 = new UIButton(320, 140, 180, 70, 12.0f);
    btn2->SetCommonColor(glm::vec4(0.7f, 0.25f, 0.25f, 1.0f));
    btn2->SetHoverColor(glm::vec4(0.85f, 0.35f, 0.35f, 1.0f));
    btn2->SetClickColor(glm::vec4(0.6f, 0.2f, 0.2f, 1.0f));
    btn2->SetOnclick([]() {
        std::println("按钮 2 被点击！");
        });
    panel->AddChildComponent(btn2);

    UIText* btn2Text = new UIText(0, 0, 180, 70, defaultFont, "点击按钮 2", Align::CENTER, 24);
    btn2Text->SetFontColor(glm::vec4(1.0f));
    btn2->AddChildComponent(btn2Text);

    UIButton* btn3 = new UIButton(400, 400, 100, 100, 150.0f);
    window.AddComponent(btn3);

    // ==========================
    // 5. 运行窗口主循环
    // ==========================
    window.OnUpdate();

    return 0;
}