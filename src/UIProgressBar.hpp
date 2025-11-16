#pragma once
#include "UIElement.hpp"
#include "UIStyles.hpp"
#include "UIHelpers.hpp"
#include "UIConfig.hpp"
#include <SDL2/SDL_ttf.h>
#include <functional>
#include <string>

enum class UIProgressOrientation { Horizontal, Vertical };

class UIProgressBar : public UIElement {
public:
    explicit UIProgressBar(int x, int y, int w, int h, float& bind);

    void handleEvent(const SDL_Event&) override {}
    void update(float dt) override;
    void render(SDL_Renderer* renderer) override;
    bool isFocusable() const override { return focusable; }

    UIProgressBar* setRange(float mn, float mx);
    UIProgressBar* setValue(float v);
    UIProgressBar* setBuffer(float v);
    UIProgressBar* setIndeterminate(bool b);
    UIProgressBar* setOrientation(UIProgressOrientation o);
    UIProgressBar* setShowText(bool b);
    UIProgressBar* setTextFormatter(std::function<std::string(float)> f);
    UIProgressBar* setCornerRadius(int r) { cornerRadius = (r < 0 ? 0 : r); return this; }
    UIProgressBar* setBorderThickness(int px) { borderPx = (px < 0 ? 0 : px); return this; }
    UIProgressBar* setFocusable(bool f) { focusable = f; return this; }
    
    float value() const { return linked.get(); }
    float min()   const { return minV; }
    float max()   const { return maxV; }

    ~UIProgressBar();

    struct UIProgressStyle {
        int radius      = 6;
        int borderPx    = 1;
        int pad         = 2;
        SDL_Color track{};
        SDL_Color fill{};
        SDL_Color buffer{};
        SDL_Color border{};
        SDL_Color text{};
    };

    

private:
    std::reference_wrapper<float> linked;
    float minV = 0.f, maxV = 1.f;

    float bufferV = -1.f;
    bool  indeterminate = false;
    float marquee = 0.f;
    float marqueeSpeed = 0.9f;

    UIProgressOrientation orient = UIProgressOrientation::Horizontal;
    bool  showText   = false;
    std::function<std::string(float)> formatter;

    int cornerRadius = -1;
    int borderPx     = -1;
    bool focusable   = false;

    mutable UIHelpers::UniqueTexture cachedTex{ nullptr };
    mutable std::string cachedStr;
    mutable int cachedW = 0, cachedH = 0;

    void rebuildText(SDL_Renderer* r, TTF_Font* f, const std::string& s) const;
    static float clamp01(float v) { return v < 0.f ? 0.f : (v > 1.f ? 1.f : v); }
};
