#include "UIHelpers.hpp"
#include <cmath>
#include <algorithm>

namespace UIHelpers {

void DrawFilledCircle(SDL_Renderer* renderer, int cx, int cy, int radius, SDL_Color color) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    const float threshold = 0.5f;
    const float maxDist = radius + threshold;

    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            float distance = std::hypotf(x, y);

            if (distance <= radius - threshold) {
                SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
                SDL_RenderDrawPoint(renderer, cx + x, cy + y);
            } else if (distance <= maxDist) {
                float alpha = color.a * (1.0f - (distance - (radius - threshold)));
                SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, static_cast<Uint8>(alpha));
                SDL_RenderDrawPoint(renderer, cx + x, cy + y);
            }
        }
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

void DrawCircleRing(SDL_Renderer* renderer, int cx, int cy, int radius, int thickness, SDL_Color color) {
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
    const int innerRadius = radius - thickness;
    const float feather = 0.5f;

    for (int y = -radius; y <= radius; y++) {
        for (int x = -radius; x <= radius; x++) {
            float distance = std::hypotf(x, y);

            if (distance >= innerRadius - feather && distance <= radius + feather) {
                float alpha = color.a;
                if (distance > radius - feather) {
                    alpha *= (radius + feather - distance) / (2 * feather);
                }
                if (distance < innerRadius + feather) {
                    alpha *= (distance - (innerRadius - feather)) / (2 * feather);
                }

                alpha = std::clamp(alpha, 0.0f, 255.0f);
                SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, static_cast<Uint8>(alpha));
                SDL_RenderDrawPoint(renderer, cx + x, cy + y);
            }
        }
    }
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
}

}
