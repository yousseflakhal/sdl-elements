#include "UIRadioGroup.hpp"

void UIRadioGroup::addButton(std::shared_ptr<UIRadioButton> btn) {
    buttons.push_back(btn);
}

void UIRadioGroup::select(int id) {
    selectedID = id;
}

int UIRadioGroup::getSelectedID() const {
    return selectedID;
}
