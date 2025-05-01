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
#include "UITextField.hpp"
#include "UIComboBox.hpp"


namespace FormUI {
    // NOTE: SDLFormUI does not take ownership of the font.
    void Init(TTF_Font* defaultFont = nullptr);
    void Shutdown();

    std::shared_ptr<UIButton> Button( const std::string& label, int x, int y, int w, int h, std::function<void()> onClick = nullptr, TTF_Font* font = nullptr);
    std::shared_ptr<UICheckbox> Checkbox(const std::string& label, int x, int y, int w, int h, bool& bind, TTF_Font* font);
    std::shared_ptr<UILabel> Label(const std::string& text, int x, int y, int w, int h, TTF_Font* font = nullptr);
    std::shared_ptr<UISlider> Slider(const std::string& label, int x, int y, int w, int h, float& bind, float min, float max);
    std::shared_ptr<UITextField> TextField(const std::string& label, int x, int y, int w, int h, std::string& bind, int maxLen = 32);
    std::shared_ptr<UIComboBox> ComboBox(const std::vector<std::string>& options, int x, int y, int w, int h, int& selectedIndex, TTF_Font* font = nullptr);
    void AddElement(std::shared_ptr<UIElement> element);
    void ShowPopup(std::shared_ptr<UIPopup> popup);
    void ClosePopup();



    void HandleEvent(const SDL_Event& e);
    void Update();
    void Render(SDL_Renderer* renderer);
}
