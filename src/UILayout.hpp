#pragma once

#include "FormUI.hpp"

namespace FormUI {

class Layout {
public:
    Layout(int x, int y, int spacing = 10)
        : currentX(x), currentY(y), spacing(spacing) {}

    std::shared_ptr<UILabel> addLabel(const std::string& text, int width = 300, int height = 30);
    std::shared_ptr<UICheckbox> addCheckbox(const std::string& label, bool& value, int width = 300, int height = 30);
    std::shared_ptr<UISlider> addSlider(const std::string& label, float& value, float min, float max, int width = 300, int height = 40);
    std::shared_ptr<UITextField> addTextField(const std::string& label, std::string& bind, int maxLen = 32, int width = 300, int height = 40);
    std::shared_ptr<UIButton> addButton(const std::string& label, std::function<void()> onClick, int width, int height, TTF_Font* font);
    std::pair<std::shared_ptr<UILabel>, std::shared_ptr<UIButton>> addLabelButtonRow(
        const std::string& labelText,
        const std::string& buttonText,
        std::function<void()> onClick,
        int labelWidth = 200,
        int buttonWidth = 100,
        int height = 30,
        TTF_Font* labelFont = nullptr,
        TTF_Font* buttonFont = nullptr
    );
    std::shared_ptr<UIRadioGroup> addRadioGroup(
        const std::vector<std::pair<std::string, int>>& options,
        int& selectedID,
        int width = 300,
        int height = 30,
        int groupSpacing = 10
    );
    std::shared_ptr<UIComboBox> addComboBox(const std::vector<std::string>& options, int& selectedIndex, int width = 300, int height = 30);
    std::shared_ptr<UISpinner> addSpinner(int& bind, int min = 0, int max = 100, int step = 1, int width = 100, int height = 30);
    std::shared_ptr<UITextArea> addTextArea(const std::string& label, std::string& bind, int maxLen = 512, int width = 300, int height = 100);
    void setDefaultFont(TTF_Font* font) { defaultFont = font; }
    TTF_Font* getDefaultFont() const { return defaultFont; }
        

private:
    int currentX, currentY;
    int spacing;
    TTF_Font* defaultFont = nullptr;
};

}
