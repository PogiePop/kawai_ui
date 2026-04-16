#include <ui_window>

int main()
{
    using namespace Kawai;

    UIWindow window(800, 600, "Input Test");

    auto font = window.GetFontResources()["default"];

    auto input = new UITextBox(200, 250, 500, 60, font, 28);
    Kawai::UIPanel* panel3 = new Kawai::UIPanel(0, 0, 60, 40, Kawai::color_red);


    window.AddComponent(input);
    /*window.AddComponent(panel3);*/

    window.OnUpdate();
}