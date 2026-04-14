#include<ui_layout>
#include<ui_component>
#include<queue>

namespace Kawai
{
	void UIAnchorLayout::ApplyLayout(const std::vector<UIComponent*>& roots
		, int windowW, int windowH)
	{
        using QueuePair = std::pair<UIComponent*, glm::vec2>;
        std::queue<QueuePair> q;

        // 根节点先入队 → 父尺寸 = 窗口尺寸
        for (auto* comp : roots)
        {
            q.emplace(comp, glm::vec2{ (float)windowW, (float)windowH });
        }

        // ================================
        // 层序遍历（BFS）= 先父后子 ✅
        // ================================
        while (!q.empty())
        {
            auto [comp, parentSize] = q.front();
            q.pop();

            // 1. 先更新自己（父节点一定先算）
            UpdateComponent(comp, parentSize.x, parentSize.y);

            // 2. 子节点依次入队，使用【当前已算好的comp尺寸】作为父尺寸
            for (auto* child : comp->childrens)
            {
                q.emplace(child, glm::vec2{ (float)comp->w, (float)comp->h });
            }
        }

        // 全局更新 NDC 坐标
        for (auto* comp : roots)
        {
            comp->UpdateComponentSize();
        }
	}

    void UIAnchorLayout::UpdateComponent(UIComponent* comp, float parentW, float parentH)
    {
        auto& anchor = comp->anchor;
        auto& offset = comp->offset;
        auto& pivot = comp->pivot;

        float xMin = anchor.min.x * parentW + offset.left;
        float xMax = anchor.max.x * parentW - offset.right;

        float yMin = anchor.min.y * parentH + offset.bottom;
        float yMax = anchor.max.y * parentH - offset.top;

        float width = xMax - xMin;
        float height = yMax - yMin;

        comp->w = width;
        comp->h = height;

        comp->x = xMin - pivot.pivot.x * width;
        comp->y = yMin - pivot.pivot.y * height;

        comp->TransformToNDCP();
        comp->childChange = true;
    }
}