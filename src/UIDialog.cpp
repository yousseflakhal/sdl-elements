#include "UIDialog.hpp"
#include "UIConfig.hpp"
#include <SDL2/SDL_ttf.h>
#include <SDL2/SDL.h>

UIDialog::UIDialog(const std::string& title,
                   const std::string& message,
                   std::function<void()> onOk,
                   std::function<void()> onCancel)
    : UIPopup((800 - 400) / 2, (600 - 200) / 2, 400, 200),
      title(title), message(message), onOk(onOk), onCancel(onCancel)
{
    const int dialogWidth = 400;
    const int dialogHeight = 200;
    bounds = {
        (800 - dialogWidth) / 2,
        (600 - dialogHeight) / 2,
        dialogWidth,
        dialogHeight
    };

    TTF_Font* font = UIConfig::getDefaultFont();
    int btnW = 100, btnH = 40, spacing = 20;
    int btnY = bounds.y + bounds.h - btnH - spacing;
    int okX = bounds.x + bounds.w / 2 - btnW - spacing / 2;
    int cancelX = bounds.x + bounds.w / 2 + spacing / 2;

    okButton = std::make_shared<UIButton>("OK", okX, btnY, btnW, btnH, font);
    cancelButton = std::make_shared<UIButton>("Cancel", cancelX, btnY, btnW, btnH, font);

    okButton->setOnClick([this, onOk]() {
        if (onOk) onOk();
        close();
    });

    cancelButton->setOnClick([this, onCancel]() {
        if (onCancel) onCancel();
        close();
    });

    addChild(okButton);
    addChild(cancelButton);
}

void UIDialog::render(SDL_Renderer* renderer) {
    UIPopup::render(renderer);

    const UITheme& th = getTheme();
    const auto lst = MakeLabelStyle(th);
    const auto pst = MakePopupStyle(th);
    TTF_Font* font = UIConfig::getDefaultFont();
    if (!font) return;

    int x = bounds.x + pst.pad;
    int y = bounds.y + pst.pad;

    if (!title.empty()) {
        SDL_Surface* s = TTF_RenderUTF8_Blended(font, title.c_str(), lst.fg);
        if (s) {
            SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
            SDL_Rect dst{ x, y, s->w, s->h };
            SDL_RenderCopy(renderer, t, nullptr, &dst);
            y += s->h + (pst.pad / 2);
            SDL_DestroyTexture(t);
            SDL_FreeSurface(s);
        }
    }

    if (!message.empty()) {
        SDL_Surface* s = TTF_RenderUTF8_Blended(font, message.c_str(), lst.fg);
        if (s) {
            SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
            SDL_Rect dst{ x, y, s->w, s->h };
            SDL_RenderCopy(renderer, t, nullptr, &dst);
            SDL_DestroyTexture(t);
            SDL_FreeSurface(s);
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
    UIPopup::handleEvent(e);
}

void UIDialog::close() {
    visible = false;
}
