#include "UISlider.hpp"



UISlider::UISlider(const std::string& label, int x, int y, int w, int h, float& bind, float min, float max)
    : label(label), linkedValue(bind), minVal(min), maxVal(max)
{
    bounds = { x, y, w, h };
}

void UISlider::handleEvent(const SDL_Event& e) {
    if (!enabled) return;

    auto clamp01 = [](float v){ return v < 0.f ? 0.f : (v > 1.f ? 1.f : v); };

    const int trackH = 6;
    SDL_Rect track = {
        bounds.x,
        bounds.y + (bounds.h - trackH)/2,
        bounds.w,
        trackH
    };

    float t = (linkedValue.get() - minVal) / (maxVal - minVal);
    t = clamp01(t);
    const int usable = track.w - 2*thumbRadius;
    const int cx = track.x + thumbRadius + int(std::round(t * usable));
    const int cy = bounds.y + bounds.h/2;

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        int mx = e.button.x, my = e.button.y;
        const int dx = mx - cx, dy = my - cy;
        const bool onThumb = (dx*dx + dy*dy) <= (thumbRadius*thumbRadius);
        const bool onTrack =
            (mx >= track.x && mx < track.x + track.w &&
             my >= track.y - thumbRadius && my < track.y + track.h + thumbRadius);

        if (onThumb || onTrack) {
            if (focusable) focused = true;
            dragging = true;

            float nt = clamp01( (float(mx) - (track.x + thumbRadius)) / float(usable) );
            linkedValue.get() = minVal + nt * (maxVal - minVal);
        } else if (focusable) {
            focused = false;
        }
    } else if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
        dragging = false;
    } else if (e.type == SDL_MOUSEMOTION && dragging) {
        int mx = e.motion.x;
        float nt = clamp01( (float(mx) - (track.x + thumbRadius)) / float(usable) );
        linkedValue.get() = minVal + nt * (maxVal - minVal);
    }

    if (focused && e.type == SDL_KEYDOWN) {
        const float step = (maxVal - minVal) * 0.01f;
        if (e.key.keysym.sym == SDLK_LEFT)  linkedValue.get() = std::max(minVal, linkedValue.get() - step);
        if (e.key.keysym.sym == SDLK_RIGHT) linkedValue.get() = std::min(maxVal, linkedValue.get() + step);
        if (e.key.keysym.sym == SDLK_ESCAPE) focused = false;
    }
}


bool UISlider::isHovered() const {
    return hovered;
}

void UISlider::update(float) {
    int mx, my; SDL_GetMouseState(&mx, &my);
    hovered = (mx >= bounds.x && mx < bounds.x + bounds.w &&
               my >= bounds.y && my < bounds.y + bounds.h);
}


void UISlider::render(SDL_Renderer* renderer) {
    const UITheme& th = getTheme();
    const auto st = MakeSliderStyle(th);

    const int trackH = st.trackH;
    SDL_Rect track = {
        bounds.x,
        bounds.y + (bounds.h - trackH)/2,
        bounds.w,
        trackH
    };

    UIHelpers::FillRoundedRect(renderer, track.x, track.y, track.w, track.h, trackH/2, st.track);

    auto clamp01 = [](float v){ return v < 0.f ? 0.f : (v > 1.f ? 1.f : v); };
    float t = (linkedValue.get() - minVal) / (maxVal - minVal);
    t = clamp01(t);
    const int usable = track.w - 2*thumbRadius;
    const int cx = track.x + thumbRadius + int(std::round(t * usable));
    const int cy = bounds.y + bounds.h/2;

    if (focusable && focused) {
        UIHelpers::DrawCircleRing(renderer, cx, cy, thumbRadius + 3, 3, st.focusRing);
    }

    SDL_Color drawThumb = dragging ? st.thumbDrag : st.thumb;
    UIHelpers::DrawFilledCircle(renderer, cx, cy, thumbRadius, drawThumb);
}
