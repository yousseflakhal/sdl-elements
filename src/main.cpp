#define SDL_MAIN_HANDLED
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include "FormUI.hpp"

int main() {
    if (SDL_Init(SDL_INIT_VIDEO) < 0) return 1;
    if (TTF_Init() == -1) {
        SDL_Quit();
        return 1;
    }

    SDL_Window* window = SDL_CreateWindow("SDLFormUI Demo", SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, SDL_WINDOW_SHOWN);
    if (!window) {
        SDL_Quit();
        return 1;
    }

    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    TTF_Font* font = TTF_OpenFont("assets/OpenSans-Regular.ttf", 16);
    if (!font) {
        SDL_DestroyRenderer(renderer);
        SDL_DestroyWindow(window);
        SDL_Quit();
        return 1;
    }

    FormUI::Init(font);

    FormUI::Button("Click Me", 100, 100, 120, 40, [] {
        printf("Button clicked!\n");
    });

    bool enableMusic = true;
    FormUI::Checkbox("Enable Music", 100, 200, 200, 30, &enableMusic);

    FormUI::Label("Settings", 100, 60, 300, 30)->setColor({100, 200, 255, 255});

    bool running = true;
    SDL_Event e;

    float volume = 0.5f;
    FormUI::Slider("Volume", 100, 300, 300, 40, &volume, 0.0f, 1.0f);

    std::string username;
    FormUI::TextField("Username", 100, 400, 300, 40, &username, 20)->setPlaceholder("Enter your name");

    SDL_StartTextInput();
    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT) running = false;
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
    SDL_StopTextInput();
    TTF_Quit();
    SDL_Quit();

    return 0;
}
