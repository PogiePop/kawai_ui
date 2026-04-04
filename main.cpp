#include <print>
#include <shader>
#include <ui_window>
#include <ui_colors>

int main()
{
    Kawai::UIWindow _window(800, 600, "Kawai");
    Kawai::UIRect* rect = new Kawai::UIRect(0, 0, 200, 200);
    Kawai::UIRect* rect2 = new Kawai::UIRect(200, 200, 50, 50, glm::vec4(0.3f, 0.4f, 0.6f, 1.0f));
    Kawai::UIPanel* panel = new Kawai::UIPanel(50, 50, 500, 500, Kawai::color_cyan);
    Kawai::UIPanel* panel1 = new Kawai::UIPanel(50, 50, 400, 400, Kawai::color_black);
    Kawai::UIPanel* panel2 = new Kawai::UIPanel(50, 50, 100, 100, Kawai::color_magenta);
    panel->AddChildComponent<Kawai::UIPanel>(panel1);
    panel1->AddChildComponent<Kawai::UIPanel>(panel2);
    _window.AddComponent<Kawai::UIRect>(rect);
    _window.AddComponent<Kawai::UIRect>(rect2);
    _window.AddComponent<Kawai::UIPanel>(panel);
    rect->SetPriority(100);


    _window.OnUpdate();
    return 0;



}