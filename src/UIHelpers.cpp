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

void UIHelpers::DrawRoundStrokeLine(SDL_Renderer* r, float x1, float y1, float x2, float y2, float thickness, SDL_Color color) {
    float dx = x2 - x1, dy = y2 - y1;
    float len = std::sqrt(dx*dx + dy*dy);
    if (len <= 0.0001f) {
        UIHelpers::DrawFilledCircle(r, (int)std::round(x1), (int)std::round(y1), (int)std::round(thickness * 0.45f), color);
        return;
    }
    float ux = dx / len, uy = dy / len;
    const float radius = thickness * 0.5f;
    const float step   = std::max(0.25f, std::min(0.35f, radius * 0.35f));
    const int   n      = (int)std::ceil(len / step);

    for (int i = 0; i <= n; ++i) {
        float t = (i * step);
        float px = x1 + ux * t;
        float py = y1 + uy * t;
        UIHelpers::DrawFilledCircle(r, (int)std::round(px), (int)std::round(py), (int)std::round(radius), color);
    }
}

void UIHelpers::DrawCheckmark(SDL_Renderer* r, const SDL_Rect& box, float thickness, SDL_Color color, float pad) {
    const float scaleX = 0.84f;
    const float scaleY = 0.84f;

    float x1 = box.x + pad;                float y1 = box.y + box.h * 0.56f;
    float xm = box.x + box.w * 0.45f;      float ym = box.y + box.h - pad;
    float x2 = box.x + box.w - pad;        float y2 = box.y + pad + 1.0f;

    const float cx = box.x + box.w * 0.5f;
    const float cy = box.y + box.h * 0.5f;
    auto S = [&](float& x, float& y){ x = cx + (x - cx) * scaleX; y = cy + (y - cy) * scaleY; };

    S(x1,y1); S(xm,ym); S(x2,y2);

    float t = thickness * std::min(scaleX, scaleY);
    UIHelpers::DrawRoundStrokeLine(r, x1, y1, xm, ym, t, color);
    UIHelpers::DrawRoundStrokeLine(r, xm, ym, x2, y2, t, color);
    UIHelpers::DrawFilledCircle(r, (int)std::round(xm), (int)std::round(ym),
                                (int)std::round(t * 0.50f), color);
}

void UIHelpers::DrawChevronDown(SDL_Renderer* r, int cx, int cy, int width, int height, float thickness, SDL_Color color) {
    const float halfW = width * 0.5f;
    const float halfH = height * 0.5f;
    float x1 = cx - halfW, y1 = cy - halfH;
    float xm = cx,         yxm = cy + halfH;
    float x2 = cx + halfW, y2 = cy - halfH;
    UIHelpers::DrawRoundStrokeLine(r, x1, y1, xm, yxm, thickness, color);
    UIHelpers::DrawRoundStrokeLine(r, xm, yxm, x2, y2, thickness, color);
}