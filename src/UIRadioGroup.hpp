#pragma once
#include <memory>
#include <vector>
#include "UIRadioButton.hpp"

class UIRadioButton;

class UIRadioGroup {
    public:
        void addButton(std::shared_ptr<UIRadioButton> btn);
        void select(int id);
        int getSelectedID() const;
    
    private:
        std::vector<std::shared_ptr<UIRadioButton>> buttons;
        int selectedID = -1;
    };