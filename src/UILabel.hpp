#pragma once
#include <string>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "UIElement.hpp"

class UILabel : public UIElement {
public:
    UILabel(const std::string& text, int x, int y, int w, int h, TTF_Font* font = nullptr);
    ~UILabel() = default;
    
    void render(SDL_Renderer* renderer) override;
    void update(float dt) override { (void)dt; }
    void handleEvent(const SDL_Event& e) override { (void)e; }

    UILabel* setColor(SDL_Color newColor);
    SDL_Color getColor() const;
    
    void setText(const std::string& newText);
    const std::string& getText() const { return text; }

private:
    std::string text;
    TTF_Font* font = nullptr;
    SDL_Color color = {255, 255, 255, 255};
    
    mutable UIHelpers::UniqueTexture cachedTexture;
    mutable std::string cachedText;
    mutable SDL_Color cachedColor{0, 0, 0, 0};
    mutable TTF_Font* cachedFont = nullptr;
    mutable int cachedWidth = 0;
    mutable int cachedHeight = 0;
    
    void invalidateCache() const;
};