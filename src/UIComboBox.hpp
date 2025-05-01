#pragma once

#include "UIElement.hpp"
#include <vector>
#include <string>
#include <functional>

class UIComboBox : public UIElement {
public:
    UIComboBox(int x, int y, int w, int h, const std::vector<std::string>& options, int& selectedIndex);

    void setFont(TTF_Font* f);
    void setOnSelect(std::function<void(int)> callback);

    void handleEvent(const SDL_Event& e) override;
    void update(float dt) override;
    void render(SDL_Renderer* renderer) override;

private:
    std::vector<std::string> options;
    std::reference_wrapper<int> selectedIndex;
    std::function<void(int)> onSelect;
    TTF_Font* font = nullptr;
    bool expanded = false;
    int hoveredIndex = -1;
};