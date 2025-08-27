#pragma once

#include "UIElement.hpp"
#include "UIConfig.hpp"
#include "UIHelpers.hpp"
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>
#include <vector>
#include <string>
#include <functional>
#include <optional>

class UIComboBox : public UIElement {
public:
    UIComboBox(int x, int y, int w, int h, const std::vector<std::string>& options, int& selectedIndex);

    void setFont(TTF_Font* f);
    void setOnSelect(std::function<void(int)> callback);
    bool isHovered() const override;
    bool isExpanded() const;
    int  getItemCount() const;
    int  getItemHeight() const;
    const SDL_Rect& getBounds() const;

    void handleEvent(const SDL_Event& e) override;
    void update(float dt) override;
    void render(SDL_Renderer* renderer) override;
    UIComboBox* setTextColor(SDL_Color c);

    UIComboBox* setFocusable(bool f) { focusable = f; return this; }
    bool isFocused() const { return focused; }

private:
    std::vector<std::string> options;
    std::reference_wrapper<int> selectedIndex;
    std::function<void(int)> onSelect;
    TTF_Font* font = nullptr;
    std::optional<SDL_Color> customTextColor;

    bool expanded = false;
    int  hoveredIndex = -1;
    bool hovered = false;

    bool focused = false;
    bool focusable = true;
    int  cornerRadius = 8;
};