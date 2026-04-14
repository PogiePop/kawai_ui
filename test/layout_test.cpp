#include <print>
#include <ui_component>

using namespace Kawai;

int main()
{
	UICanvas* canvas = new UICanvas(0, 0, 800, 600);
	std::println("canvas.x = {}, .y = {}, .w = {}, .h = {}", canvas->x, canvas->y, canvas->w, canvas->h);
	UIPanel* p1 = new UIPanel(50, 50, 400, 500);
	UIPanel* p2 = new UIPanel(0, 0, 100, 100);
	canvas->AddChildComponent<UIPanel>(p1);
	p1->AddChildComponent<UIPanel>(p2);
	auto p2w= p2->GetWorldPos();
	std::println("{}, {}", p2w.x, p2w.y);
	return 0;
}