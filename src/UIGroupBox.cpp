#include "UIGroupBox.hpp"
#include "UIConfig.hpp"

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
    const UITheme& theme = getTheme();
    SDL_SetRenderDrawColor(renderer,
        theme.borderColor.r,
        theme.borderColor.g,
        theme.borderColor.b,
        theme.borderColor.a);

    int titleWidth = 0, titleHeight = 0;
    if (!title.empty() && font) {
        TTF_SizeText(font, title.c_str(), &titleWidth, &titleHeight);
    }

    const int padding = 8;
    const int gapStart = bounds.x + padding;
    const int gapEnd = gapStart + titleWidth + padding;

    SDL_RenderDrawLine(renderer, bounds.x, bounds.y, gapStart, bounds.y);
    SDL_RenderDrawLine(renderer, gapEnd, bounds.y, bounds.x + bounds.w, bounds.y);

    SDL_RenderDrawLine(renderer, bounds.x, bounds.y, bounds.x, bounds.y + bounds.h);
    SDL_RenderDrawLine(renderer, bounds.x + bounds.w, bounds.y, bounds.x + bounds.w, bounds.y + bounds.h);
    SDL_RenderDrawLine(renderer, bounds.x, bounds.y + bounds.h, bounds.x + bounds.w, bounds.y + bounds.h);

    if (!title.empty() && font) {
        SDL_Surface* surf = TTF_RenderUTF8_Blended(font, title.c_str(), theme.textColor);
        if (surf) {
            SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_Rect textRect = {
                bounds.x + padding + 4,
                bounds.y - surf->h / 2,
                surf->w,
                surf->h
            };
            SDL_RenderCopy(renderer, tex, nullptr, &textRect);
            SDL_FreeSurface(surf);
            SDL_DestroyTexture(tex);
        }
    }

    for (auto& child : children) {
        child->render(renderer);
    }
}