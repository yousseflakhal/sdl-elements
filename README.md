# SDLFormUI

SDLFormUI is a lightweight C++ UI library built on SDL2 and SDL_ttf.  
It provides a simple and efficient way to build modern user interfaces for games and tools without any external dependencies.

---

## Features

- Header-only or modular structure: include a single header (`SDLFormUI.hpp`) or use the multi-file version.
- Built-in themes: light, dark, and high-contrast, with full customization options.
- Global configuration through `UIConfig` for font, style, and theme management.
- Straightforward API: create elements, handle events, and render them.
- Full input handling with focus management, keyboard navigation, and text editing (undo/redo).
- Safe resource management using RAII wrappers for SDL textures and surfaces.
- Works directly with SDL2 and SDL_ttf without any additional frameworks.

---

## Components

| Component | Description |
|------------|-------------|
| `UIButton` | Clickable button with hover, press, and keyboard focus |
| `UILabel` | Static text label with automatic texture caching |
| `UICheckbox` | Boolean toggle linked to a `bool&` reference |
| `UITextField` | Single-line editable text box with selection and undo/redo |
| `UIPopup` | Generic popup container with rounded borders |
| `UIDialog` | Modal popup with title, message, and OK/Cancel buttons |
| `UITheme`, `UIStyle`, `UIConfig` | Manage the look and feel globally or per element |

---

## Example Usage

```cpp
#include <SDL.h>
#include <SDL_ttf.h>
#include "SDLFormUI.hpp"

int main() {
    SDL_Init(SDL_INIT_VIDEO);
    TTF_Init();

    SDL_Window* window = SDL_CreateWindow("SDLFormUI Example",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED, 800, 600, 0);
    SDL_Renderer* renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);

    // Load font and configure defaults
    TTF_Font* font = TTF_OpenFont("DejaVuSans.ttf", 16);
    UIConfig::setDefaultFont(font);
    UIConfig::setTheme(MakeDarkTheme());

    bool checked = false;
    std::string text = "Hello SDLFormUI";

    UIButton button("Click Me", 50, 50, 120, 40, font);
    UICheckbox checkbox("Enable feature", 50, 120, 200, 30, checked, font);
    UITextField textField("Label", 50, 180, 200, 40, text);

    bool running = true;
    SDL_Event e;

    while (running) {
        while (SDL_PollEvent(&e)) {
            if (e.type == SDL_QUIT)
                running = false;

            button.handleEvent(e);
            checkbox.handleEvent(e);
            textField.handleEvent(e);
        }

        button.update(0);
        checkbox.update(0);
        textField.update(0);

        SDL_SetRenderDrawColor(renderer, 20, 20, 20, 255);
        SDL_RenderClear(renderer);

        button.render(renderer);
        checkbox.render(renderer);
        textField.render(renderer);

        SDL_RenderPresent(renderer);
    }

    TTF_CloseFont(font);
    SDL_DestroyRenderer(renderer);
    SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}