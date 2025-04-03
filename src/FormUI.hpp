#pragma once
#include <string>
#include <functional>
#include <memory>
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "UIButton.hpp"
#include "UIManager.hpp"
#include "UIConfig.hpp"
#include "UICheckbox.hpp"
#include "UILabel.hpp"
#include "UISlider.hpp"


namespace FormUI {
    void Init(TTF_Font* defaultFont = nullptr);
    void Shutdown();

    std::shared_ptr<UIButton> Button(const std::string& label, int x, int y, int w, int h, std::function<void()> onClick = nullptr);
    std::shared_ptr<UICheckbox> Checkbox(const std::string& label, int x, int y, int w, int h, bool* bind);
    std::shared_ptr<UILabel> Label(const std::string& text, int x, int y, int w, int h, TTF_Font* font = nullptr);
    std::shared_ptr<UISlider> Slider(const std::string& label, int x, int y, int w, int h, float* bind, float min, float max);



    void HandleEvent(const SDL_Event& e);
    void Update(float dt);
    void Render(SDL_Renderer* renderer);
}
