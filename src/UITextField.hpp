#pragma once
#include "UIElement.hpp"
#include "UIConfig.hpp"
#include "UIHelpers.hpp"
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <algorithm>
#include "UICommon.hpp"
#include <functional>

class UITextField : public UIElement {
public:
    UITextField(const std::string& label, int x, int y, int w, int h, std::string& bind, int maxLen = 32);
    UITextField* setPlaceholder(const std::string& text);
    UITextField* setFont(TTF_Font* f);
    UITextField* setInputType(InputType type);

    bool isFocusable() const override { return focusable; }

    bool isHovered() const override;
    void handleEvent(const SDL_Event& e) override;
    void update(float dt) override;
    void render(SDL_Renderer* renderer) override;
    std::function<void(const std::string&)> onSubmit;
    UITextField* setOnSubmit(std::function<void(const std::string&)> cb) {
        onSubmit = std::move(cb);
        return this;
    }

    

private:
    std::string label;
    std::reference_wrapper<std::string> linkedText;
    int maxLength = 32;
    bool hovered = false;
    bool focused = false;
    bool focusable = true;
    Uint32 lastBlinkTime = 0;
    bool cursorVisible = true;
    std::string placeholder;
    SDL_Color placeholderColor = {160, 160, 160, 255};
    TTF_Font* font = nullptr;
    InputType inputType = InputType::TEXT;
    int cornerRadius = 10;
    int borderPx     = 1;
    Uint32 lastInputTicks = 0;
    Uint32 lastBlinkTicks = 0;
};
