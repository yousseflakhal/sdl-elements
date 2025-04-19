#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "FormUI.hpp"
#include "UILayout.hpp"
#include "UIRadioGroup.hpp"
#include "UIRadioButton.hpp"
#include "UIGroupBox.hpp"

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window* window = SDL_CreateWindow("SDLFormUI Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    TTF_Font* font = TTF_OpenFont("assets/OpenSans-Regular.ttf", 16);
    if (!font) {
        SDL_Log("Failed to load font: %s", TTF_GetError());
        return 1;
    }

    FormUI::Init(font);

    bool optionA = false;
    float settingLevel = 0.3f;

    auto groupBox = std::make_shared<UIGroupBox>("Audio Settings", 400, 50, 300, 200);
    FormUI::AddElement(groupBox);

    auto checkbox = std::make_shared<UICheckbox>("Enable Sound", 420, 90, 200, 30, optionA, font);
    auto slider   = std::make_shared<UISlider>("Volume", 420, 130, 250, 40, settingLevel, 0.0f, 1.0f);

    groupBox->addChild(checkbox);
    groupBox->addChild(slider);
    bool quit = false;
    SDL_Event e;
    while (!quit) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) quit = true;
            FormUI::HandleEvent(e);
        }

        FormUI::Update();

        SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
        SDL_RenderClear(renderer);
        FormUI::Render(renderer);
        SDL_RenderPresent(renderer);
    }

    FormUI::Shutdown();
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
