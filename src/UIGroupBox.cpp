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
        theme.backgroundColor.r,
        theme.backgroundColor.g,
        theme.backgroundColor.b,
        theme.backgroundColor.a);
    SDL_RenderFillRect(renderer, &bounds);

    SDL_SetRenderDrawColor(renderer,
        theme.borderColor.r,
        theme.borderColor.g,
        theme.borderColor.b,
        theme.borderColor.a);
    SDL_RenderDrawRect(renderer, &bounds);

    if (!title.empty() && font) {
        SDL_Surface* surf = TTF_RenderText_Blended(font, title.c_str(), theme.textColor);
        if (surf) {
            SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_Rect textRect = { bounds.x + 8, bounds.y - surf->h / 2, surf->w, surf->h };
            SDL_RenderCopy(renderer, tex, nullptr, &textRect);
            SDL_FreeSurface(surf);
            SDL_DestroyTexture(tex);
        }
    }

    for (auto& child : children)
        child->render(renderer);
}
