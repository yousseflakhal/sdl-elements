#include "UIPopup.hpp"

UIPopup::UIPopup(int x, int y, int w, int h) {
    bounds = { x, y, w, h };
}

void UIPopup::addChild(std::shared_ptr<UIElement> el) {
    children.push_back(el);
}

void UIPopup::handleEvent(const SDL_Event& e) {
    for (auto& child : children) {
        if (child)  child->handleEvent(e);
    }
}

void UIPopup::update(float dt) {
    for (auto& child : children) {
        child->update(dt);
    }
}

void UIPopup::render(SDL_Renderer* renderer)
{
    const UITheme& th = getTheme();
    const PopupStyle st = MakePopupStyle(th);

    const SDL_Rect r = bounds;

    if (st.borderPx > 0) {
        UIHelpers::FillRoundedRect(renderer, r.x, r.y, r.w, r.h, st.radius, st.border);
        SDL_Rect inner { r.x + st.borderPx, r.y + st.borderPx,
                         r.w - 2*st.borderPx, r.h - 2*st.borderPx };
        UIHelpers::FillRoundedRect(renderer, inner.x, inner.y, inner.w, inner.h,
                                   std::max(0, st.radius - st.borderPx), st.bg);
    } else {
        UIHelpers::FillRoundedRect(renderer, r.x, r.y, r.w, r.h, st.radius, st.bg);
    }

    for (auto& child : children) {
        child->render(renderer);
    }
}