#include "UIDialog.hpp"
#include "UIConfig.hpp"
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>

UIDialog::UIDialog(const std::string& title,
                   const std::string& message,
                   std::function<void()> onOk,
                   std::function<void()> onCancel)
    : UIPopup(0, 0, 400, 200),
      title(title), message(message), onOk(onOk), onCancel(onCancel)
{
    TTF_Font* font = UIConfig::getDefaultFont();

    okButton     = std::make_shared<UIButton>("OK",     0, 0, 100, 40, font);
    cancelButton = std::make_shared<UIButton>("Cancel", 0, 0, 100, 40, font);

    okButton->setOnClick([this, onOk]() { if (onOk) onOk(); close(); });
    cancelButton->setOnClick([this, onCancel]() { if (onCancel) onCancel(); close(); });

    addChild(okButton);
    addChild(cancelButton);

    layoutButtons();
}

void UIDialog::render(SDL_Renderer* renderer) {
    UIPopup::render(renderer);

    const UITheme& th = getTheme();
    const auto lst = MakeLabelStyle(th);
    const auto pst = MakePopupStyle(th, getStyle());
    TTF_Font* font = UIConfig::getDefaultFont();
    if (!font) return;

    int x = bounds.x + pst.pad;
    int y = bounds.y + pst.pad;

    if (!title.empty()) {
        auto surface = UIHelpers::MakeSurface(
            TTF_RenderUTF8_Blended(font, title.c_str(), lst.fg)
        );
        
        if (surface) {
            auto texture = UIHelpers::MakeTexture(
                SDL_CreateTextureFromSurface(renderer, surface.get())
            );
            
            SDL_Rect dst{ x, y, surface->w, surface->h };
            SDL_RenderCopy(renderer, texture.get(), nullptr, &dst);
            y += surface->h + (pst.pad / 2);
        }
    }

    if (!message.empty()) {
        auto surface = UIHelpers::MakeSurface(
            TTF_RenderUTF8_Blended(font, message.c_str(), lst.fg)
        );
        
        if (surface) {
            auto texture = UIHelpers::MakeTexture(
                SDL_CreateTextureFromSurface(renderer, surface.get())
            );
            
            SDL_Rect dst{ x, y, surface->w, surface->h };
            SDL_RenderCopy(renderer, texture.get(), nullptr, &dst);
        }
    }
}

void UIDialog::handleEvent(const SDL_Event& e) {
    if (ignoreNextClick) {
        if (e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP) return;
        ignoreNextClick = false;
    }
    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
        close();
        return;
    }
    if (e.type == SDL_KEYDOWN &&
        (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_KP_ENTER)) {
        if (onOk) onOk();
        close();
        return;
    }
    UIPopup::handleEvent(e);
}

void UIDialog::close() {
    visible = false;
}

void UIDialog::setBounds(int x, int y, int w, int h) {
    UIElement::setBounds(x, y, w, h);
    layoutButtons();
}

void UIDialog::layoutButtons() {
    if (!okButton || !cancelButton) return;

    const auto pst = MakePopupStyle(getTheme(), getStyle());
    const int btnW = 100, btnH = 40;
    const int gap  = pst.pad / 2;

    const int btnY = bounds.y + bounds.h - pst.pad - btnH;
    const int okX     = bounds.x + (bounds.w - (btnW*2 + gap)) / 2;
    const int cancelX = okX + btnW + gap;

    okButton->setBounds(okX,     btnY, btnW, btnH);
    cancelButton->setBounds(cancelX, btnY, btnW, btnH);
}