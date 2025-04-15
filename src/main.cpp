#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "FormUI.hpp"
#include "UILayout.hpp"
#include "UIRadioGroup.hpp"
#include "UIRadioButton.hpp"

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

    bool checked = false;
    std::string name;
    float volume = 0.5f;

    auto layout = FormUI::Layout(50, 50, 40);
    // layout.addLabel("Settings");
    // layout.addTextField("Name:", name);
    // layout.addSlider("Volume", volume, 0.0f, 1.0f);
    // layout.addCheckbox("Enable Feature", checked);
    // layout.addButton("OK", []() { SDL_Log("Clicked OK!"); }, 100, 40, nullptr);

    // Set theme
    UITheme redTheme;
    redTheme.backgroundColor     = { 80, 0, 0, 255 };
    redTheme.hoverColor          = { 120, 0, 0, 255 };
    redTheme.borderColor         = { 180, 50, 50, 255 };
    redTheme.borderHoverColor    = { 255, 100, 100, 255 };
    redTheme.textColor           = { 255, 255, 255, 255 };
    UIConfig::setTheme(redTheme);

    // Create radio group
    auto radioGroup = std::make_shared<UIRadioGroup>();
    radioGroup->select(1); // Default selection

    // Create radio buttons
    auto radio1 = std::make_shared<UIRadioButton>("Easy",   50, 300, 200, 30, radioGroup.get(), 1, font);
    auto radio2 = std::make_shared<UIRadioButton>("Medium", 50, 340, 200, 30, radioGroup.get(), 2, font);
    auto radio3 = std::make_shared<UIRadioButton>("Hard",   50, 380, 200, 30, radioGroup.get(), 3, font);

    radioGroup->addButton(radio1);
    radioGroup->addButton(radio2);
    radioGroup->addButton(radio3);

    // Register radio buttons in UI
    FormUI::AddElement(radio1);
    FormUI::AddElement(radio2);
    FormUI::AddElement(radio3);

    // Add print selection button
    FormUI::Button("Print Selection", 50, 430, 200, 40, [radioGroup]() {
        SDL_Log("Selected Radio ID: %d", radioGroup->getSelectedID());
    });

    // Main loop
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

    // Cleanup
    FormUI::Shutdown();
    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
    return 0;
}
