#include "UILayout.hpp"

namespace FormUI {

std::shared_ptr<UILabel> Layout::addLabel(const std::string& text, int width, int height) {
    auto labelEl = FormUI::Label(text, currentX, currentY, width, height, defaultFont);
    currentY += height + spacing;
    return labelEl;
}

std::shared_ptr<UICheckbox> Layout::addCheckbox(const std::string& label, bool& value, int width, int height) {
    auto checkbox = FormUI::Checkbox(label, currentX, currentY, width, height, value, defaultFont);
    currentY += height + spacing;
    return checkbox;
}

std::shared_ptr<UISlider> Layout::addSlider(const std::string& label, float& value, float min, float max, int width, int height) {
    auto slider = FormUI::Slider(label, currentX, currentY, width, height, value, min, max);
    currentY += height + spacing;
    return slider;
}

std::shared_ptr<UITextField> Layout::addTextField(const std::string& label, std::string& bind, int maxLen, int width, int height) {
    auto textField = FormUI::TextField(label, currentX, currentY, width, height, bind, maxLen);
    textField->setFont(defaultFont);
    currentY += height + spacing;
    return textField;
}

std::shared_ptr<UIButton> Layout::addButton(const std::string& label, std::function<void()> onClick, int width, int height, TTF_Font* font)
{
    auto button = FormUI::Button(label, currentX, currentY, width, height, onClick, font ? font : defaultFont);
    currentY += height + spacing;
    return button;
}

std::pair<std::shared_ptr<UILabel>, std::shared_ptr<UIButton>> Layout::addLabelButtonRow(
    const std::string& labelText,
    const std::string& buttonText,
    std::function<void()> onClick,
    int labelWidth,
    int buttonWidth,
    int height,
    TTF_Font* labelFont,
    TTF_Font* buttonFont
) {
    auto label = FormUI::Label(labelText, currentX, currentY, labelWidth, height, labelFont ? labelFont : defaultFont);
    auto button = FormUI::Button(buttonText, currentX + labelWidth + 10, currentY, buttonWidth, height, onClick, buttonFont ? buttonFont : defaultFont);
    currentY += height + spacing;
    return { label, button };
}

std::shared_ptr<UIRadioGroup> Layout::addRadioGroup(
    const std::vector<std::pair<std::string, int>>& options,
    int& selectedID,
    int width,
    int height,
    int groupSpacing
) {
    auto group = std::make_shared<UIRadioGroup>();
    group->select(selectedID);

    for (const auto& [label, id] : options) {
        auto btn = std::make_shared<UIRadioButton>(label, currentX, currentY, width, height, group.get(), id, defaultFont);
        group->addButton(btn);
        FormUI::AddElement(btn);
        currentY += height + groupSpacing;
    }

    return group;
}

std::shared_ptr<UIComboBox> Layout::addComboBox(const std::vector<std::string>& options, int& selectedIndex, int width, int height) {
    auto combo = FormUI::ComboBox(options, currentX, currentY, width, height, selectedIndex, defaultFont);
    currentY += height + spacing;
    return combo;
}

std::shared_ptr<UISpinner> Layout::addSpinner(int& bind, int min, int max, int step, int width, int height) {
    auto spinner = FormUI::Spinner(currentX, currentY, width, height, bind, min, max, step, defaultFont);
    currentY += height + spacing;
    return spinner;
}

std::shared_ptr<UITextArea> Layout::addTextArea(const std::string& label, std::string& bind, int maxLen, int width, int height) {
    auto textArea = FormUI::TextArea(label, currentX, currentY, width, height, bind, maxLen);
    textArea->setFont(defaultFont);
    currentY += height + spacing;
    return textArea;
}

}

