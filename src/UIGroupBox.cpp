#include "UIGroupBox.hpp"

UIGroupBox::UIGroupBox(const std::string& title, int x, int y, int w, int h)
    : title(title)
{
    bounds = { x, y, w, h };
    font = getThemeFont(getTheme());
}

void UIGroupBox::addChild(std::shared_ptr<UIElement> child) {
    children.push_back(child);
}

const std::vector<std::shared_ptr<UIElement>>& UIGroupBox::getChildren() const {
    return children;
}

void UIGroupBox::handleEvent(const SDL_Event& e) {
    for (auto& child : children)
        child->handleEvent(e);
}

void UIGroupBox::update(float dt) {
    for (auto& child : children)
        child->update(dt);
}

void UIGroupBox::render(SDL_Renderer* renderer) {
    const UITheme& th = getTheme();
    const auto st = MakeGroupBoxStyle(th);

    TTF_Font* fnt = font ? font : (th.font ? th.font : UIConfig::getDefaultFont());
    if (!fnt) return;

    int titleW = 0, titleH = 0;
    SDL_Texture* titleTex = nullptr;
    if (!title.empty()) {
        SDL_Surface* ts = TTF_RenderUTF8_Blended(fnt, title.c_str(), st.title);
        if (ts) {
            titleW = ts->w; titleH = ts->h;
            titleTex = SDL_CreateTextureFromSurface(renderer, ts);
            SDL_FreeSurface(ts);
        }
    }

    SDL_Rect frame = bounds;

    if (st.bg.a > 0) {
        UIHelpers::FillRoundedRect(renderer, frame.x, frame.y, frame.w, frame.h, st.radius, st.bg);
    }

    const int b = std::max<int>(1, st.borderPx);
    SDL_Color frameCol = st.border;

    UIHelpers::FillRoundedRect(renderer, frame.x, frame.y, b, frame.h, st.radius, frameCol);
    UIHelpers::FillRoundedRect(renderer, frame.x + frame.w - b, frame.y, b, frame.h, st.radius, frameCol);
    UIHelpers::FillRoundedRect(renderer, frame.x, frame.y + frame.h - b, frame.w, b, st.radius, frameCol);

    int titleStartX = frame.x + st.titlePadX;
    int titleEndX   = titleStartX + titleW + st.titlePadX;
    int topY        = frame.y;

    if (titleTex)
        UIHelpers::FillRoundedRect(renderer, frame.x, topY, std::max(0, titleStartX - frame.x), b, st.radius, frameCol);
    else
        UIHelpers::FillRoundedRect(renderer, frame.x, topY, frame.w, b, st.radius, frameCol);

    if (titleTex)
        UIHelpers::FillRoundedRect(renderer, titleEndX, topY, std::max(0, frame.x + frame.w - titleEndX), b, st.radius, frameCol);

    if (titleTex) {
        SDL_Rect td { titleStartX, frame.y + st.titlePadY, titleW, titleH };
        SDL_RenderCopy(renderer, titleTex, nullptr, &td);
        SDL_DestroyTexture(titleTex);
    }

    for (auto& child : children) {
        if (child) child->render(renderer);
    }
}
