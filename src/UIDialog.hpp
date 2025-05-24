#pragma once
#include "UIPopup.hpp"
#include "UIButton.hpp"
#include <SDL2/SDL.h>
#include <functional>
#include <memory>
#include "FormUI.hpp"

class UIDialog : public UIPopup {
public:
    UIDialog(const std::string& title,
             const std::string& message,
             std::function<void()> onOk = nullptr,
             std::function<void()> onCancel = nullptr);

    void render(SDL_Renderer* renderer) override;
    void handleEvent(const SDL_Event& e) override;

    void close();

private:
    std::string title;
    std::string message;
    std::function<void()> onOk;
    std::function<void()> onCancel;

    std::shared_ptr<UIButton> okButton;
    std::shared_ptr<UIButton> cancelButton;
    bool ignoreNextClick = true;
};
