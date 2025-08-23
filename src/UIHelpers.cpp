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

void UIHelpers::FillRoundedRect(SDL_Renderer* renderer,
                                         int x, int y, int w, int h,
                                         int radius, SDL_Color color) {
    SDL_BlendMode original_mode;
    SDL_GetRenderDrawBlendMode(renderer, &original_mode);
    SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);

    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);

    SDL_Rect center = { x + radius, y, w - 2*radius, h };
    SDL_RenderFillRect(renderer, &center);
    SDL_Rect sides  = { x, y + radius, w, h - 2*radius };
    SDL_RenderFillRect(renderer, &sides);

    const int centers[4][2] = {
        { x + radius,     y + radius     },
        { x + w - radius, y + radius     },
        { x + radius,     y + h - radius },
        { x + w - radius, y + h - radius }
    };

    for (int corner = 0; corner < 4; ++corner) {
        int cx = centers[corner][0];
        int cy = centers[corner][1];

        int start_x = (corner % 2 == 0) ? x : x + w - radius;
        int end_x   = (corner % 2 == 0) ? x + radius : x + w;
        int start_y = (corner < 2) ? y : y + h - radius;
        int end_y   = (corner < 2) ? y + radius : y + h;

        for (int py = start_y; py < end_y; ++py) {
            for (int px = start_x; px < end_x; ++px) {
                float dx = px - cx + 0.5f;
                float dy = py - cy + 0.5f;
                float distance = sqrtf(dx*dx + dy*dy);

                if (distance <= radius - 0.5f) {
                    SDL_RenderDrawPoint(renderer, px, py);
                } else if (distance < radius + 0.5f) {
                    Uint8 alpha = (Uint8)(color.a * (1.0f - (distance - (radius - 0.5f))));
                    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, alpha);
                    SDL_RenderDrawPoint(renderer, px, py);
                    SDL_SetRenderDrawColor(renderer, color.r, color.g, color.b, color.a);
                }
            }
        }
    }

    SDL_SetRenderDrawBlendMode(renderer, original_mode);
}

void UIHelpers::DrawShadowRoundedRect(SDL_Renderer* renderer, const SDL_Rect& rect, int radius, int offset, Uint8 alpha) {
    SDL_Rect shadow = { rect.x + offset, rect.y + offset, rect.w, rect.h };
    SDL_Color sc = { 0, 0, 0, alpha };
    FillRoundedRect(renderer, shadow.x, shadow.y, shadow.w, shadow.h, radius, sc);
}

void UIHelpers::StrokeRoundedRectOutside(SDL_Renderer* renderer,
                                         const SDL_Rect& innerRect,
                                         int radius,
                                         int thickness,
                                         SDL_Color ringColor,
                                         SDL_Color innerBg) {
    SDL_Rect outer = {
        innerRect.x - thickness,
        innerRect.y - thickness,
        innerRect.w + 2*thickness,
        innerRect.h + 2*thickness
    };
    FillRoundedRect(renderer, outer.x, outer.y, outer.w, outer.h, radius + thickness, ringColor);

    FillRoundedRect(renderer, innerRect.x, innerRect.y, innerRect.w, innerRect.h, radius, innerBg);
}