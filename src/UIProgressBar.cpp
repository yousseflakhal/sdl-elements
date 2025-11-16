#include "UIProgressBar.hpp"
#include <cmath>

UIProgressBar::UIProgressBar(int x, int y, int w, int h, float& bind)
    : linked(bind)
{
    bounds = { x, y, w, h };
}

UIProgressBar::~UIProgressBar() {}

UIProgressBar* UIProgressBar::setRange(float mn, float mx) {
    if (mx < mn) std::swap(mn, mx);
    minV = mn; maxV = mx;
    return this;
}

UIProgressBar* UIProgressBar::setValue(float v) {
    float cl = (maxV > minV) ? std::max(minV, std::min(maxV, v)) : v;
    linked.get() = cl;
    return this;
}

UIProgressBar* UIProgressBar::setBuffer(float v) {
    bufferV = v;
    return this;
}

UIProgressBar* UIProgressBar::setIndeterminate(bool b) {
    indeterminate = b;
    return this;
}

UIProgressBar* UIProgressBar::setOrientation(UIProgressOrientation o) {
    orient = o;
    return this;
}

UIProgressBar* UIProgressBar::setShowText(bool b) {
    showText = b;
    return this;
}

UIProgressBar* UIProgressBar::setTextFormatter(std::function<std::string(float)> f) {
    formatter = std::move(f);
    return this;
}

void UIProgressBar::rebuildText(SDL_Renderer* r, TTF_Font* f, const std::string& s) const {
    if (!showText || !r || !f) { 
        cachedTex.reset(nullptr); 
        cachedStr.clear(); 
        cachedW = cachedH = 0; 
        return; 
    }
    if (cachedTex && s == cachedStr) return;

    cachedTex.reset(nullptr);
    cachedStr = s;
    
    const UITheme& th = getTheme();
    const auto st = MakeProgressStyle(th, getStyle());
    SDL_Color col = st.text;
    
    auto surf = UIHelpers::MakeSurface(TTF_RenderUTF8_Blended(f, s.c_str(), col));
    if (!surf) { 
        cachedW = cachedH = 0; 
        return; 
    }
    cachedW = surf->w; 
    cachedH = surf->h;
    cachedTex = UIHelpers::MakeTexture(SDL_CreateTextureFromSurface(r, surf.get()));
}

void UIProgressBar::update(float dt) {
    if (!enabled) return;
    if (indeterminate) {
        marquee += std::max(0.0f, dt) * marqueeSpeed;
        if (marquee >= 1.f) marquee -= std::floor(marquee);
    }
}

void UIProgressBar::render(SDL_Renderer* renderer) {
    const UITheme& th = getTheme();
    const UIStyle& ds = getStyle();
    const auto st     = MakeProgressStyle(th, ds);

    const int effRadius   = (cornerRadius >= 0 ? cornerRadius : st.radius);
    const int effBorderPx = (borderPx     >= 0 ? borderPx     : st.borderPx);
    const int pad         = st.pad;

    SDL_Rect dst = bounds;
    if (effBorderPx > 0) {
        UIHelpers::FillRoundedRect(renderer, dst.x, dst.y, dst.w, dst.h, effRadius, st.border);
        SDL_Rect inner{ dst.x + effBorderPx, dst.y + effBorderPx,
                        dst.w - 2*effBorderPx, dst.h - 2*effBorderPx };
        UIHelpers::FillRoundedRect(renderer, inner.x, inner.y, inner.w, inner.h,
                                   std::max(0, effRadius - effBorderPx), st.track);
        dst = inner;
    } else {
        UIHelpers::FillRoundedRect(renderer, dst.x, dst.y, dst.w, dst.h, effRadius, st.track);
    }

    SDL_Rect content{ dst.x + pad, dst.y + pad,
                      std::max(0, dst.w - 2*pad), std::max(0, dst.h - 2*pad) };

    if (content.w <= 0 || content.h <= 0) return;

    float ratio = 0.f;
    if (!indeterminate && maxV > minV) {
        ratio = (linked.get() - minV) / (maxV - minV);
        ratio = clamp01(ratio);
    }

    if (!indeterminate && bufferV >= 0.f) {
        float br = clamp01((bufferV - minV) / (maxV - minV));
        if (orient == UIProgressOrientation::Horizontal) {
            int bw = (int)std::round(br * content.w);
            if (bw > 0) {
                int r = std::min(effRadius, std::min(bw, content.h) / 2);
                UIHelpers::FillRoundedRect(renderer, content.x, content.y, bw, content.h, r, st.buffer);
            }
        } else {
            int bh = (int)std::round(br * content.h);
            if (bh > 0) {
                int r = std::min(effRadius, std::min(content.w, bh) / 2);
                int by = content.y + (content.h - bh);
                UIHelpers::FillRoundedRect(renderer, content.x, by, content.w, bh, r, st.buffer);
            }
        }
    }

    if (indeterminate) {
        const float span = 0.35f;
        if (orient == UIProgressOrientation::Horizontal) {
            int w = (int)std::round(span * content.w);
            int x = content.x + (int)std::round(marquee * (content.w + w)) - w;
            SDL_Rect seg{ std::max(content.x, x), content.y, 
                         std::min(w, content.x + content.w - std::max(content.x, x)), content.h };
            if (seg.w > 0) {
                int r = std::min(effRadius, std::min(seg.w, seg.h) / 2);
                UIHelpers::FillRoundedRect(renderer, seg.x, seg.y, seg.w, seg.h, r, st.fill);
            }
        } else {
            int h = (int)std::round(span * content.h);
            int y = content.y + (int)std::round(marquee * (content.h + h)) - h;
            SDL_Rect seg{ content.x, std::max(content.y, y), content.w,
                         std::min(h, content.y + content.h - std::max(content.y, y)) };
            if (seg.h > 0) {
                int r = std::min(effRadius, std::min(seg.w, seg.h) / 2);
                UIHelpers::FillRoundedRect(renderer, seg.x, seg.y, seg.w, seg.h, r, st.fill);
            }
        }
    } else {
        if (orient == UIProgressOrientation::Horizontal) {
            int fw = (int)std::round(ratio * content.w);
            if (fw > 0) {
                int r = std::min(effRadius, std::min(fw, content.h) / 2);
                UIHelpers::FillRoundedRect(renderer, content.x, content.y, fw, content.h, r, st.fill);
            }
        } else {
            int fh = (int)std::round(ratio * content.h);
            if (fh > 0) {
                int r = std::min(effRadius, std::min(content.w, fh) / 2);
                int fy = content.y + (content.h - fh);
                UIHelpers::FillRoundedRect(renderer, content.x, fy, content.w, fh, r, st.fill);
            }
        }
    }

    if (showText) {
        TTF_Font* font = getThemeFont(th);
        if (font) {
            std::string s;
            if (formatter) {
                float v = (!indeterminate && maxV > minV) ? (linked.get() - minV) / (maxV - minV) : 0.f;
                s = formatter(clamp01(v));
            } else {
                int pct = (int)std::round(ratio * 100.0f);
                s = std::to_string(pct) + "%";
            }
            rebuildText(renderer, font, s);
            if (cachedTex) {
                SDL_Rect dstTxt{
                    bounds.x + (bounds.w - cachedW) / 2,
                    bounds.y + (bounds.h - cachedH) / 2,
                    cachedW, cachedH
                };
                SDL_RenderCopy(renderer, cachedTex.get(), nullptr, &dstTxt);
            }
        }
    }
}