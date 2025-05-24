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
    const UITheme& theme = getTheme();
    TTF_Font* font = UIConfig::getDefaultFont();
    if (!font) return;

    SDL_SetRenderDrawColor(renderer, theme.backgroundColor.r, theme.backgroundColor.g, theme.backgroundColor.b, theme.backgroundColor.a);
    SDL_RenderFillRect(renderer, &bounds);

    SDL_SetRenderDrawColor(renderer, theme.borderColor.r, theme.borderColor.g, theme.borderColor.b, theme.borderColor.a);
    SDL_RenderDrawRect(renderer, &bounds);

    SDL_Surface* titleSurf = TTF_RenderText_Blended(font, title.c_str(), theme.textColor);
    SDL_Surface* msgSurf = TTF_RenderText_Blended(font, message.c_str(), theme.textColor);

    if (titleSurf) {
        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, titleSurf);
        SDL_Rect titleRect = {
            bounds.x + 20,
            bounds.y + 20,
            titleSurf->w,
            titleSurf->h
        };
        SDL_RenderCopy(renderer, tex, nullptr, &titleRect);
        SDL_FreeSurface(titleSurf);
        SDL_DestroyTexture(tex);
    }

    if (msgSurf) {
        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, msgSurf);
        SDL_Rect msgRect = {
            bounds.x + 20,
            bounds.y + 70,
            msgSurf->w,
            msgSurf->h
        };
        SDL_RenderCopy(renderer, tex, nullptr, &msgRect);
        SDL_FreeSurface(msgSurf);
        SDL_DestroyTexture(tex);
    }

    UIPopup::render(renderer);
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
