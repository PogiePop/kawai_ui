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
    Kawai::UIPanel* panel1 = new Kawai::UIPanel(0, 0, 400, 400, Kawai::color_black);
    Kawai::UIPanel* panel2 = new Kawai::UIPanel(50, 50, 100, 100, Kawai::color_magenta);
    Kawai::UIButton* btn1 = new Kawai::UIButton(0, 0, 60, 40);
    Kawai::UIButton* btn2 = new Kawai::UIButton(50, 50, 60, 40);
    Kawai::UIPanel* panel3 = new Kawai::UIPanel(0, 0, 60, 40, Kawai::color_red);
    Kawai::UIText* txt1 = new Kawai::UIText(400, 300, 100, 200, _window.GetFontResources()["default"], "HelloKawaiui", Kawai::BOTTOM_CENTER, 24);
    _window.AddComponent<Kawai::UIRect>(rect);
    _window.AddComponent<Kawai::UIPanel>(panel);
    panel->AddChildComponent<Kawai::UIPanel>(panel1);
    panel1->AddChildComponent<Kawai::UIPanel>(panel2);
    panel2->AddChildComponent<Kawai::UIButton>(btn1);
    _window.AddComponent<Kawai::UIRect>(rect2);
    panel2->AddChildComponent<Kawai::UIButton>(btn2);
    _window.AddComponent<Kawai::UIText>(txt1);
    //Kawai::UIButton* btn2 = new Kawai::UIButton(60, 60, 50, 50);
    //panel2->AddChildComponent<Kawai::UIButton>(btn2);
    ////btn1->SetPriority(10);
    //btn1->SetOnclick([](){
    //    std::println("我是btn1");
    //});
    int k = 0;
    btn1->SetOnclick([&]() {
        txt1->SetFontAlign((Kawai::Align(k++ % 9)));
        });

    
    // panel2->AddChildComponent<Kawai::UIPanel>(panel3);
    //rect->SetPriority(100);


    _window.OnUpdate();
    return 0;



}