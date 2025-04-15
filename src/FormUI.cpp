#include "FormUI.hpp"

namespace FormUI {
    static UIManager uiManager;

    void Init(TTF_Font* defaultFont) {
        if (defaultFont) {
            UIConfig::setDefaultFont(defaultFont);
        }
        uiManager.initCursors();
    }

    void Shutdown() {
        uiManager.cleanupCursors();
    }

    std::shared_ptr<UIButton> Button(const std::string& label, int x, int y, int w, int h, std::function<void()> onClick, TTF_Font* font) 
    {
        auto btn = std::make_shared<UIButton>(label, x, y, w, h, font);
        if (onClick) btn->setOnClick(onClick);
        uiManager.addElement(btn);
        return btn;
    }

    std::shared_ptr<UICheckbox> Checkbox(const std::string& label, int x, int y, int w, int h, bool& bind, TTF_Font* font) {
        auto box = std::make_shared<UICheckbox>(label, x, y, w, h, bind, font);
        uiManager.addElement(box);
        return box;
    }
    
    std::shared_ptr<UILabel> Label(const std::string& text, int x, int y, int w, int h, TTF_Font* font) {
        auto label = std::make_shared<UILabel>(text, x, y, w, h, font);
        uiManager.addElement(label);
        return label;
    }

    std::shared_ptr<UISlider> Slider(const std::string& label, int x, int y, int w, int h, float& bind, float min, float max) {
        auto slider = std::make_shared<UISlider>(label, x, y, w, h, bind, min, max);
        uiManager.addElement(slider);
        return slider;
    }

    std::shared_ptr<UITextField> TextField(const std::string& label, int x, int y, int w, int h, std::string& bind, int maxLen) {
        auto field = std::make_shared<UITextField>(label, x, y, w, h, bind, maxLen);
        uiManager.addElement(field);
        return field;
    }

    void AddElement(std::shared_ptr<UIElement> element) {
        uiManager.addElement(element);
    }

    void HandleEvent(const SDL_Event& e) {
        uiManager.handleEvent(e);
    }

    void Update() {
        uiManager.update(0.0f);
    }

    void Render(SDL_Renderer* renderer) {
        uiManager.render(renderer);
    }
}
