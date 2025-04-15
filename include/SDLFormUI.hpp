#ifndef SDLFORMUI
#define SDLFORMUI

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <functional>
#include <memory>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <sstream>


struct UIConfig;

struct UITheme {
    SDL_Color backgroundColor     = { 100, 100, 100, 255 };
    SDL_Color hoverColor          = { 130, 130, 130, 255 };
    SDL_Color borderColor         = { 200, 200, 200, 255 };
    SDL_Color borderHoverColor    = { 255, 255, 255, 255 };
    SDL_Color textColor           = { 255, 255, 255, 255 };
    SDL_Color placeholderColor    = { 160, 160, 160, 255 };
    SDL_Color cursorColor         = { 255, 255, 255, 255 };
    SDL_Color sliderTrackColor    = { 80, 80, 80, 255 };
    SDL_Color sliderThumbColor    = { 180, 180, 255, 255 };
    SDL_Color checkboxTickColor   = { 255, 255, 255, 255 };

    TTF_Font* font = nullptr;
};

TTF_Font* getThemeFont(const UITheme& theme);


class UIElement {
public:
    SDL_Rect bounds;
    bool visible = true;

    virtual void handleEvent(const SDL_Event& e) = 0;
    virtual bool isHovered() const { return false; }
    virtual void update(float dt) = 0;
    virtual void render(SDL_Renderer* renderer) = 0;
    void setTheme(const UITheme& theme) { customTheme = theme; hasCustomTheme = true; }
    const UITheme& getTheme() const { return hasCustomTheme ? customTheme : UIConfig::getTheme(); }

    virtual ~UIElement() = default;

private:
    UITheme customTheme;
    bool hasCustomTheme = false;
};


struct UITheme;
class UIConfig {
public:
    // NOTE: SDLFormUI does NOT take ownership. Caller must manage the lifetime (TTF_CloseFont).
    static void setDefaultFont(TTF_Font* font);
    static TTF_Font* getDefaultFont();

    static void setTheme(const UITheme& theme);
    static const UITheme& getTheme();
    static TTF_Font** getDefaultFontPtr();

private:
    static TTF_Font* defaultFont;
    static UITheme defaultTheme;
};


class UIButton : public UIElement {
public:
    UIButton(const std::string& text, int x, int y, int w, int h, TTF_Font* f = nullptr);
    void setOnClick(std::function<void()> callback);
    void setText(const std::string& newText);
    const std::string& getText() const;
    void handleEvent(const SDL_Event& e) override;
    void update(float dt) override;
    void render(SDL_Renderer* renderer) override;
    void setFont(TTF_Font* f);
    bool isHovered() const;

private:
    std::string label;
    std::function<void()> onClick;
    bool hovered = false;
    bool pressed = false;
    TTF_Font* font = nullptr;
};


class UILabel : public UIElement {
public:
    UILabel(const std::string& text, int x, int y, int w, int h, TTF_Font* font = nullptr);
    void render(SDL_Renderer* renderer) override;
    void update(float dt) override { (void)dt; }
    void handleEvent(const SDL_Event& e) override { (void)e; }

    UILabel* setColor(SDL_Color newColor);
    SDL_Color getColor() const;

private:
    std::string text;
    SDL_Rect bounds;
    TTF_Font* font = nullptr;
    SDL_Color color = {255, 255, 255, 255};
};


class UICheckbox : public UIElement {
public:
    UICheckbox(const std::string& label, int x, int y, int w, int h, bool& bind, TTF_Font* f);

    void setFont(TTF_Font* f);

    void handleEvent(const SDL_Event& e) override;
    bool isHovered() const override;
    void update(float dt) override;
    void render(SDL_Renderer* renderer) override;

private:
    std::string label;
    SDL_Rect bounds;
    bool hovered = false;
    std::reference_wrapper<bool> linkedValue;
    TTF_Font* font = nullptr;
};


class UITextField : public UIElement {
public:
    UITextField(const std::string& label, int x, int y, int w, int h, std::string& bind, int maxLen = 32);
    UITextField* setPlaceholder(const std::string& text);
    UITextField* setFont(TTF_Font* f);

    bool isHovered() const override;
    void handleEvent(const SDL_Event& e) override;
    void update(float dt) override;
    void render(SDL_Renderer* renderer) override;



private:
    std::string label;
    SDL_Rect bounds;
    std::reference_wrapper<std::string> linkedText;
    int maxLength = 32;
    bool hovered = false;
    bool focused = false;
    Uint32 lastBlinkTime = 0;
    bool cursorVisible = true;
    std::string placeholder;
    SDL_Color placeholderColor = {160, 160, 160, 255};
    TTF_Font* font = nullptr;
};


class UISlider : public UIElement {
public:
    UISlider(const std::string& label, int x, int y, int w, int h, float& bind, float min, float max);

    void handleEvent(const SDL_Event& e) override;
    bool isHovered() const override;
    void update(float dt) override;
    void render(SDL_Renderer* renderer) override;

private:
    std::string label;
    SDL_Rect bounds;
    std::reference_wrapper<float> linkedValue;
    float minValue = 0.0f;
    float maxValue = 1.0f;
    bool hovered = false;
    bool dragging = false;
};

class UIManager {
    public:
        void initCursors();
        void cleanupCursors();
        void addElement(std::shared_ptr<UIElement> el);
        void handleEvent(const SDL_Event& e);
        void update(float dt);
        void render(SDL_Renderer* renderer);

    private:
        std::vector<std::shared_ptr<UIElement>> elements;
        SDL_Cursor* arrowCursor = nullptr;
        SDL_Cursor* handCursor = nullptr;
        SDL_Cursor* ibeamCursor = nullptr;
        bool handCursorActive = false;
    };



namespace FormUI {

class Layout {
public:
    Layout(int x, int y, int spacing = 10)
        : currentX(x), currentY(y), spacing(spacing) {}

    std::shared_ptr<UILabel> addLabel(const std::string& text, int width = 300, int height = 30);
    std::shared_ptr<UICheckbox> addCheckbox(const std::string& label, bool& value, int width = 300, int height = 30);
    std::shared_ptr<UISlider> addSlider(const std::string& label, float& value, float min, float max, int width = 300, int height = 40);
    std::shared_ptr<UITextField> addTextField(const std::string& label, std::string& bind, int maxLen = 32, int width = 300, int height = 40);
    std::shared_ptr<UIButton> addButton(const std::string& label, std::function<void()> onClick, int width, int height, TTF_Font* font);
    std::pair<std::shared_ptr<UILabel>, std::shared_ptr<UIButton>> addLabelButtonRow(
        const std::string& labelText,
        const std::string& buttonText,
        std::function<void()> onClick,
        int labelWidth = 200,
        int buttonWidth = 100,
        int height = 30,
        TTF_Font* labelFont = nullptr,
        TTF_Font* buttonFont = nullptr
    );
    void setDefaultFont(TTF_Font* font) { defaultFont = font; }
    TTF_Font* getDefaultFont() const { return defaultFont; }


private:
    int currentX, currentY;
    int spacing;
    TTF_Font* defaultFont = nullptr;
};

}



namespace FormUI {
    // NOTE: SDLFormUI does not take ownership of the font.
    void Init(TTF_Font* defaultFont = nullptr);
    void Shutdown();

    std::shared_ptr<UIButton> Button( const std::string& label, int x, int y, int w, int h, std::function<void()> onClick = nullptr, TTF_Font* font = nullptr);
    std::shared_ptr<UICheckbox> Checkbox(const std::string& label, int x, int y, int w, int h, bool& bind, TTF_Font* font);
    std::shared_ptr<UILabel> Label(const std::string& text, int x, int y, int w, int h, TTF_Font* font = nullptr);
    std::shared_ptr<UISlider> Slider(const std::string& label, int x, int y, int w, int h, float& bind, float min, float max);
    std::shared_ptr<UITextField> TextField(const std::string& label, int x, int y, int w, int h, std::string& bind, int maxLen = 32);
    void AddElement(std::shared_ptr<UIElement> element);



    void HandleEvent(const SDL_Event& e);
    void Update();
    void Render(SDL_Renderer* renderer);
}


class UIRadioGroup;

class UIRadioButton : public UIElement {
    public:
        UIRadioButton(const std::string& label, int x, int y, int w, int h, UIRadioGroup* group, int id, TTF_Font* font = nullptr);

        void handleEvent(const SDL_Event& e) override;
        void update(float dt) override;
        void render(SDL_Renderer* renderer) override;
        bool isHovered() const override;
        void setFont(TTF_Font* font);
        int getID() const;

    private:
        std::string label;
        int id;
        UIRadioGroup* group = nullptr;
        TTF_Font* font = nullptr;
        bool hovered = false;
    };


class UIRadioButton;

class UIRadioGroup {
    public:
        void addButton(std::shared_ptr<UIRadioButton> btn);
        void select(int id);
        int getSelectedID() const;

    private:
        std::vector<std::shared_ptr<UIRadioButton>> buttons;
        int selectedID = -1;
    };

#ifdef SDLFORMUI_IMPLEMENTATION


TTF_Font* getThemeFont(const UITheme& theme) {
    return theme.font ? theme.font : UIConfig::getDefaultFont();
}


TTF_Font* UIConfig::defaultFont = nullptr;
UITheme UIConfig::defaultTheme;

void UIConfig::setDefaultFont(TTF_Font* font) {
    defaultFont = font;
}

TTF_Font* UIConfig::getDefaultFont() {
    return defaultFont;
}

void UIConfig::setTheme(const UITheme& theme) {
    defaultTheme = theme;
}

const UITheme& UIConfig::getTheme() {
    return defaultTheme;
}

TTF_Font** UIConfig::getDefaultFontPtr() {
    return &defaultFont;
}


UIButton::UIButton(const std::string& text, int x, int y, int w, int h, TTF_Font* f)
    : label(text), font(f)
{
    bounds = { x, y, w, h };
}

void UIButton::setOnClick(std::function<void()> callback) {
    onClick = callback;
}

void UIButton::setText(const std::string& newText) {
    label = newText;
}

const std::string& UIButton::getText() const {
    return label;
}

void UIButton::handleEvent(const SDL_Event& e) {
    if (e.type == SDL_MOUSEBUTTONDOWN) {
        int mx = e.button.x;
        int my = e.button.y;
        if (mx >= bounds.x && mx <= bounds.x + bounds.w &&
            my >= bounds.y && my <= bounds.y + bounds.h) {
            pressed = true;
        }
    } else if (e.type == SDL_MOUSEBUTTONUP && pressed) {
        int mx = e.button.x;
        int my = e.button.y;
        if (mx >= bounds.x && mx <= bounds.x + bounds.w &&
            my >= bounds.y && my <= bounds.y + bounds.h) {
            if (onClick) onClick();
        }
        pressed = false;
    }
}

void UIButton::update(float) {
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    SDL_Point mousePoint = { mx, my };
    hovered = SDL_PointInRect(&mousePoint, &bounds);
}


void UIButton::render(SDL_Renderer* renderer) {
    const UITheme& theme = UIConfig::getTheme();

    SDL_SetRenderDrawColor(renderer,
                           hovered ? theme.hoverColor.r : theme.backgroundColor.r,
                           hovered ? theme.hoverColor.g : theme.backgroundColor.g,
                           hovered ? theme.hoverColor.b : theme.backgroundColor.b,
                           hovered ? theme.hoverColor.a : theme.backgroundColor.a);
    SDL_RenderFillRect(renderer, &bounds);

    SDL_SetRenderDrawColor(renderer,
                           hovered ? theme.borderHoverColor.r : theme.borderColor.r,
                           hovered ? theme.borderHoverColor.g : theme.borderColor.g,
                           hovered ? theme.borderHoverColor.b : theme.borderColor.b,
                           hovered ? theme.borderHoverColor.a : theme.borderColor.a);
    SDL_RenderDrawRect(renderer, &bounds);

    TTF_Font* activeFont = font ? font : getThemeFont(getTheme());
    if (!activeFont) {
        SDL_Log("UIButton: No valid font to render label.");
        return;
    }

    SDL_Surface* textSurface = TTF_RenderText_Blended(activeFont, label.c_str(), theme.textColor);
    if (!textSurface) {
        SDL_Log("UIButton: Failed to render text surface: %s", TTF_GetError());
        return;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!textTexture) {
        SDL_Log("UIButton: Failed to create texture from surface: %s", SDL_GetError());
        SDL_FreeSurface(textSurface);
        return;
    }

    SDL_Rect textRect = {
        bounds.x + (bounds.w - textSurface->w) / 2,
        bounds.y + (bounds.h - textSurface->h) / 2,
        textSurface->w,
        textSurface->h
    };
    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}

void UIButton::setFont(TTF_Font* f) {
    font = f;
}

bool UIButton::isHovered() const {
    return hovered;
}


UICheckbox::UICheckbox(const std::string& label, int x, int y, int w, int h, bool& bind, TTF_Font* f)
    : label(label), linkedValue(bind), font(f)
{
    bounds = { x, y, w, h };
}

void UICheckbox::setFont(TTF_Font* f) {
    font = f;
}

void UICheckbox::handleEvent(const SDL_Event& e) {
    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        int mx = e.button.x;
        int my = e.button.y;
        if (mx >= bounds.x && mx <= bounds.x + bounds.w &&
            my >= bounds.y && my <= bounds.y + bounds.h) {
            linkedValue.get() = !linkedValue.get();
        }
    }
}

bool UICheckbox::isHovered() const {
    return hovered;
}

void UICheckbox::update(float) {
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    SDL_Point mousePoint = { mx, my };
    hovered = SDL_PointInRect(&mousePoint, &bounds);
}

void UICheckbox::render(SDL_Renderer* renderer) {
    const UITheme& theme = UIConfig::getTheme();
    TTF_Font* activeFont = font ? font : getThemeFont(getTheme());
    if (!activeFont) {
        SDL_Log("UICheckbox: No valid font for rendering.");
        return;
    }

    SDL_Surface* textSurface = TTF_RenderText_Blended(activeFont, label.c_str(), theme.textColor);
    if (!textSurface) {
        SDL_Log("UICheckbox: Failed to render text surface: %s", TTF_GetError());
        return;
    }

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!textTexture) {
        SDL_Log("UICheckbox: Failed to create texture from surface: %s", SDL_GetError());
        SDL_FreeSurface(textSurface);
        return;
    }

    int textW = textSurface->w;
    int textH = textSurface->h;
    int margin = 10;
    int boxSize = 20;

    int totalWidth = textW + margin + boxSize;
    int totalHeight = std::max(textH, boxSize);
    bounds.w = totalWidth;
    bounds.h = totalHeight;

    SDL_Rect textRect = {
        bounds.x,
        bounds.y + (bounds.h - textH) / 2,
        textW,
        textH
    };
    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);

    SDL_Rect box = {
        bounds.x + textW + margin,
        bounds.y + (bounds.h - boxSize) / 2,
        boxSize,
        boxSize
    };

    SDL_SetRenderDrawColor(renderer,
        hovered ? theme.borderHoverColor.r : theme.borderColor.r,
        hovered ? theme.borderHoverColor.g : theme.borderColor.g,
        hovered ? theme.borderHoverColor.b : theme.borderColor.b,
        hovered ? theme.borderHoverColor.a : theme.borderColor.a);
    SDL_RenderDrawRect(renderer, &box);

    if (linkedValue.get()) {
        SDL_SetRenderDrawColor(renderer,
            theme.checkboxTickColor.r,
            theme.checkboxTickColor.g,
            theme.checkboxTickColor.b,
            theme.checkboxTickColor.a);
        SDL_Rect inner = {
            box.x + 4,
            box.y + 4,
            boxSize - 8,
            boxSize - 8
        };
        SDL_RenderFillRect(renderer, &inner);
    }

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}


UILabel::UILabel(const std::string& text, int x, int y, int w, int h, TTF_Font* font)
    : text(text), font(font)
{
    bounds = { x, y, w, h };
}

void UILabel::render(SDL_Renderer* renderer) {
    TTF_Font* activeFont = font ? font : getThemeFont(getTheme());
    if (!activeFont) {
        SDL_Log("UILabel: No valid font to render text.");
        return;
    }

    const SDL_Color& textColor = color.a == 0 ? UIConfig::getTheme().textColor : color;

    SDL_Surface* textSurface = TTF_RenderText_Blended(activeFont, text.c_str(), textColor);
    if (!textSurface) {
        SDL_Log("UILabel: Failed to render text surface: %s", TTF_GetError());
        return;
    }

    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, textSurface);
    if (!texture) {
        SDL_Log("UILabel: Failed to create texture from surface: %s", SDL_GetError());
        SDL_FreeSurface(textSurface);
        return;
    }

    SDL_Rect dstRect = {
        bounds.x,
        bounds.y + (bounds.h - textSurface->h) / 2,
        textSurface->w,
        textSurface->h
    };

    SDL_RenderCopy(renderer, texture, nullptr, &dstRect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(texture);
}


UILabel* UILabel::setColor(SDL_Color newColor) {
    color = newColor;
    return this;
}

SDL_Color UILabel::getColor() const {
    return color;
}


UITextField::UITextField(const std::string& label, int x, int y, int w, int h, std::string& bind, int maxLen)
    : label(label), linkedText(bind), maxLength(maxLen)
{
    bounds = { x, y, w, h };
}

UITextField* UITextField::setPlaceholder(const std::string& text) {
    placeholder = text;
    return this;
}

UITextField* UITextField::setFont(TTF_Font* f) {
    font = f;
    return this;
}

bool UITextField::isHovered() const {
    return hovered;
}

void UITextField::handleEvent(const SDL_Event& e) {
    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        int mx = e.button.x;
        int my = e.button.y;
        SDL_Point point = { mx, my };

        bool wasFocused = focused;
        focused = SDL_PointInRect(&point, &bounds);

        if (!wasFocused && focused) {
            SDL_StartTextInput();
        } else if (wasFocused && !focused) {
            SDL_StopTextInput();
        }
    }

    if (focused && e.type == SDL_TEXTINPUT) {
        // SDL_Log("Text input: %s", e.text.text);
        if (linkedText.get().length() < static_cast<size_t>(maxLength)) {
            linkedText.get().append(e.text.text);
        }
    }

    if (focused && e.type == SDL_KEYDOWN) {
        if (e.key.keysym.sym == SDLK_BACKSPACE && !linkedText.get().empty()) {
            linkedText.get().pop_back();
        } else if (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_KP_ENTER) {
            focused = false;
            SDL_StopTextInput();
        }
    }
}

void UITextField::update(float) {
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    SDL_Point point = { mx, my };
    hovered = SDL_PointInRect(&point, &bounds);

    if (focused) {
        Uint32 now = SDL_GetTicks();
        if (now - lastBlinkTime >= 500) {
            cursorVisible = !cursorVisible;
            lastBlinkTime = now;
        }
    } else {
        cursorVisible = false;
        lastBlinkTime = SDL_GetTicks();
    }
}


void UITextField::render(SDL_Renderer* renderer) {
    const UITheme& theme = getTheme();
    TTF_Font* activeFont = font ? font : getThemeFont(getTheme());
    if (!activeFont) return;

    SDL_Surface* labelSurface = TTF_RenderText_Blended(activeFont, label.c_str(), theme.textColor);
    if (!labelSurface) return;

    SDL_Texture* labelTexture = SDL_CreateTextureFromSurface(renderer, labelSurface);
    SDL_Rect labelRect = { bounds.x, bounds.y - labelSurface->h - 4, labelSurface->w, labelSurface->h };
    SDL_RenderCopy(renderer, labelTexture, nullptr, &labelRect);
    SDL_FreeSurface(labelSurface);
    SDL_DestroyTexture(labelTexture);

    SDL_SetRenderDrawColor(renderer, theme.backgroundColor.r, theme.backgroundColor.g, theme.backgroundColor.b, theme.backgroundColor.a);
    SDL_RenderFillRect(renderer, &bounds);

    SDL_Color borderCol = hovered ? theme.borderHoverColor : theme.borderColor;
    SDL_SetRenderDrawColor(renderer, borderCol.r, borderCol.g, borderCol.b, borderCol.a);
    SDL_RenderDrawRect(renderer, &bounds);

    std::string toRender = linkedText.get();
    SDL_Color textCol = theme.textColor;
    if (toRender.empty() && !focused && !placeholder.empty()) {
        toRender = placeholder;
        textCol = theme.placeholderColor;
    }

    if (!toRender.empty()) {
        SDL_Surface* textSurface = TTF_RenderText_Blended(activeFont, toRender.c_str(), textCol);
        if (textSurface) {
            SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
            SDL_Rect textRect = {
                bounds.x + 5,
                bounds.y + (bounds.h - textSurface->h) / 2,
                textSurface->w,
                textSurface->h
            };
            SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);
            SDL_FreeSurface(textSurface);
            SDL_DestroyTexture(textTexture);

            if (focused && cursorVisible) {
                SDL_SetRenderDrawColor(renderer, theme.cursorColor.r, theme.cursorColor.g, theme.cursorColor.b, theme.cursorColor.a);
                SDL_Rect cursorRect = { textRect.x + textRect.w + 2, textRect.y, 2, textRect.h };
                SDL_RenderFillRect(renderer, &cursorRect);
            }
        }
    }
}



UISlider::UISlider(const std::string& label, int x, int y, int w, int h, float& bind, float min, float max)
    : label(label), linkedValue(bind), minValue(min), maxValue(max)
{
    bounds = { x, y, w, h };
}

void UISlider::handleEvent(const SDL_Event& e) {
    int mx = e.button.x;
    int my = e.button.y;

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        if (mx >= bounds.x && mx <= bounds.x + bounds.w &&
            my >= bounds.y && my <= bounds.y + bounds.h) {
            dragging = true;
        }
    } else if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
        dragging = false;
    } else if (e.type == SDL_MOUSEMOTION && dragging && linkedValue) {
        float relX = mx - bounds.x;
        float t = std::clamp(relX / float(bounds.w), 0.0f, 1.0f);
        linkedValue.get() = minValue + t * (maxValue - minValue);
    }
}

bool UISlider::isHovered() const {
    return hovered;
}

void UISlider::update(float) {
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    SDL_Point point = { mx, my };
    hovered = SDL_PointInRect(&point, &bounds);
}

void UISlider::render(SDL_Renderer* renderer) {
    const UITheme& theme = getTheme();
    TTF_Font* font = getThemeFont(getTheme());
    if (!font || !linkedValue) return;

    SDL_Surface* labelSurface = TTF_RenderText_Blended(font, label.c_str(), theme.textColor);
    if (labelSurface) {
        SDL_Texture* labelTexture = SDL_CreateTextureFromSurface(renderer, labelSurface);
        SDL_Rect labelRect = { bounds.x, bounds.y - labelSurface->h - 4, labelSurface->w, labelSurface->h };
        SDL_RenderCopy(renderer, labelTexture, nullptr, &labelRect);
        SDL_FreeSurface(labelSurface);
        SDL_DestroyTexture(labelTexture);
    }

    SDL_Rect track = {
        bounds.x,
        bounds.y + bounds.h / 2 - 4,
        bounds.w,
        8
    };
    SDL_SetRenderDrawColor(renderer, theme.sliderTrackColor.r, theme.sliderTrackColor.g, theme.sliderTrackColor.b, theme.sliderTrackColor.a);
    SDL_RenderFillRect(renderer, &track);

    float t = (linkedValue.get() - minValue) / (maxValue - minValue);
    int thumbX = bounds.x + static_cast<int>(t * bounds.w);
    SDL_Rect thumb = { thumbX - 6, bounds.y + bounds.h / 2 - 10, 12, 20 };
    SDL_SetRenderDrawColor(renderer, theme.sliderThumbColor.r, theme.sliderThumbColor.g, theme.sliderThumbColor.b, theme.sliderThumbColor.a);
    SDL_RenderFillRect(renderer, &thumb);

    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2) << linkedValue.get();
    SDL_Surface* valueSurface = TTF_RenderText_Blended(font, oss.str().c_str(), theme.textColor);
    if (valueSurface) {
        SDL_Texture* valueTexture = SDL_CreateTextureFromSurface(renderer, valueSurface);
        SDL_Rect valueRect = {
            bounds.x + bounds.w + 10,
            bounds.y + (bounds.h - valueSurface->h) / 2,
            valueSurface->w,
            valueSurface->h
        };
        SDL_RenderCopy(renderer, valueTexture, nullptr, &valueRect);
        SDL_FreeSurface(valueSurface);
        SDL_DestroyTexture(valueTexture);
    }
}


void UIManager::addElement(std::shared_ptr<UIElement> el) {
    elements.push_back(el);
}

void UIManager::handleEvent(const SDL_Event& e) {
    for (auto& el : elements) {
        if (el->visible)
            el->handleEvent(e);
    }
}

void UIManager::update(float dt) {
    SDL_Cursor* cursorToUse = arrowCursor;

    for (auto& el : elements) {
        if (!el->visible) continue;

        el->update(dt);

        if (el->isHovered()) {
            if (dynamic_cast<UITextField*>(el.get())) {
                cursorToUse = ibeamCursor;
            } else if (
                dynamic_cast<UIButton*>(el.get()) ||
                dynamic_cast<UICheckbox*>(el.get()) ||
                dynamic_cast<UIRadioButton*>(el.get()) ||
                dynamic_cast<UISlider*>(el.get())
            ) {
                cursorToUse = handCursor;
            }
        }
    }

    if (SDL_GetCursor() != cursorToUse) {
        SDL_SetCursor(cursorToUse);
    }
}



void UIManager::render(SDL_Renderer* renderer) {
    for (auto& el : elements) {
        if (el->visible)
            el->render(renderer);
    }
}

void UIManager::initCursors() {
    arrowCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    handCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
    ibeamCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);
    SDL_SetCursor(arrowCursor);
}

void UIManager::cleanupCursors() {
    SDL_FreeCursor(arrowCursor);
    SDL_FreeCursor(handCursor);
    SDL_FreeCursor(ibeamCursor);
}


namespace FormUI {

std::shared_ptr<UILabel> Layout::addLabel(const std::string& text, int width, int height) {
    auto labelEl = FormUI::Label(text, currentX, currentY, width, height, defaultFont);
    currentY += height + spacing;
    return labelEl;
}

std::shared_ptr<UICheckbox> Layout::addCheckbox(const std::string& label, bool& value, int width, int height) {
    auto checkbox = FormUI::Checkbox(label, currentX, currentY, width, height, value, defaultFont);
    currentY += height + spacing;
    return checkbox;
}

std::shared_ptr<UISlider> Layout::addSlider(const std::string& label, float& value, float min, float max, int width, int height) {
    auto slider = FormUI::Slider(label, currentX, currentY, width, height, value, min, max);
    currentY += height + spacing;
    return slider;
}

std::shared_ptr<UITextField> Layout::addTextField(const std::string& label, std::string& bind, int maxLen, int width, int height) {
    auto textField = FormUI::TextField(label, currentX, currentY, width, height, bind, maxLen);
    textField->setFont(defaultFont);
    currentY += height + spacing;
    return textField;
}

std::shared_ptr<UIButton> Layout::addButton(const std::string& label, std::function<void()> onClick, int width, int height, TTF_Font* font)
{
    auto button = FormUI::Button(label, currentX, currentY, width, height, onClick, font ? font : defaultFont);
    currentY += height + spacing;
    return button;
}

std::pair<std::shared_ptr<UILabel>, std::shared_ptr<UIButton>> Layout::addLabelButtonRow(
    const std::string& labelText,
    const std::string& buttonText,
    std::function<void()> onClick,
    int labelWidth,
    int buttonWidth,
    int height,
    TTF_Font* labelFont,
    TTF_Font* buttonFont
) {
    auto label = FormUI::Label(labelText, currentX, currentY, labelWidth, height, labelFont ? labelFont : defaultFont);
    auto button = FormUI::Button(buttonText, currentX + labelWidth + 10, currentY, buttonWidth, height, onClick, buttonFont ? buttonFont : defaultFont);
    currentY += height + spacing;
    return { label, button };
}


}


namespace FormUI {
    static UIManager uiManager;

    void Init(TTF_Font* defaultFont) {
        if (defaultFont) {
            UIConfig::setDefaultFont(defaultFont);
        }
        uiManager.initCursors();
    }

    void Shutdown() {
        uiManager.cleanupCursors();
    }

    std::shared_ptr<UIButton> Button(const std::string& label, int x, int y, int w, int h, std::function<void()> onClick, TTF_Font* font)
    {
        auto btn = std::make_shared<UIButton>(label, x, y, w, h, font);
        if (onClick) btn->setOnClick(onClick);
        uiManager.addElement(btn);
        return btn;
    }

    std::shared_ptr<UICheckbox> Checkbox(const std::string& label, int x, int y, int w, int h, bool& bind, TTF_Font* font) {
        auto box = std::make_shared<UICheckbox>(label, x, y, w, h, bind, font);
        uiManager.addElement(box);
        return box;
    }

    std::shared_ptr<UILabel> Label(const std::string& text, int x, int y, int w, int h, TTF_Font* font) {
        auto label = std::make_shared<UILabel>(text, x, y, w, h, font);
        uiManager.addElement(label);
        return label;
    }

    std::shared_ptr<UISlider> Slider(const std::string& label, int x, int y, int w, int h, float& bind, float min, float max) {
        auto slider = std::make_shared<UISlider>(label, x, y, w, h, bind, min, max);
        uiManager.addElement(slider);
        return slider;
    }

    std::shared_ptr<UITextField> TextField(const std::string& label, int x, int y, int w, int h, std::string& bind, int maxLen) {
        auto field = std::make_shared<UITextField>(label, x, y, w, h, bind, maxLen);
        uiManager.addElement(field);
        return field;
    }

    void AddElement(std::shared_ptr<UIElement> element) {
        uiManager.addElement(element);
    }

    void HandleEvent(const SDL_Event& e) {
        uiManager.handleEvent(e);
    }

    void Update() {
        uiManager.update(0.0f);
    }

    void Render(SDL_Renderer* renderer) {
        uiManager.render(renderer);
    }
}


UIRadioButton::UIRadioButton(const std::string& label, int x, int y, int w, int h, UIRadioGroup* group, int id, TTF_Font* font)
    : label(label), id(id), group(group), font(font)
{
    bounds = { x, y, w, h };
}

void UIRadioButton::setFont(TTF_Font* f) {
    font = f;
}

int UIRadioButton::getID() const {
    return id;
}

bool UIRadioButton::isHovered() const {
    return hovered;
}

void UIRadioButton::handleEvent(const SDL_Event& e) {
    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        int mx = e.button.x;
        int my = e.button.y;
        if (mx >= bounds.x && mx <= bounds.x + bounds.w &&
            my >= bounds.y && my <= bounds.y + bounds.h) {
            if (group) group->select(id);
        }
    }
}

void UIRadioButton::update(float) {
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    SDL_Point p = { mx, my };
    hovered = SDL_PointInRect(&p, &bounds);
}

void UIRadioButton::render(SDL_Renderer* renderer) {
    const UITheme& theme = getTheme();
    TTF_Font* activeFont = font ? font : getThemeFont(getTheme());
    if (!activeFont) return;

    int radius = 8;
    int cx = bounds.x + 10;
    int cy = bounds.y + bounds.h / 2;

    SDL_Rect outer = { cx - radius, cy - radius, radius * 2, radius * 2 };
    SDL_SetRenderDrawColor(renderer,
        hovered ? theme.borderHoverColor.r : theme.borderColor.r,
        hovered ? theme.borderHoverColor.g : theme.borderColor.g,
        hovered ? theme.borderHoverColor.b : theme.borderColor.b,
        hovered ? theme.borderHoverColor.a : theme.borderColor.a);
    SDL_RenderDrawRect(renderer, &outer);

    if (group && group->getSelectedID() == id) {
        SDL_Rect inner = { cx - radius / 2, cy - radius / 2, radius, radius };
        SDL_SetRenderDrawColor(renderer,
            theme.checkboxTickColor.r,
            theme.checkboxTickColor.g,
            theme.checkboxTickColor.b,
            theme.checkboxTickColor.a);
        SDL_RenderFillRect(renderer, &inner);
    }

    SDL_Surface* textSurface = TTF_RenderText_Blended(activeFont, label.c_str(), theme.textColor);
    if (!textSurface) return;

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect textRect = {
        cx + radius + 10,
        bounds.y + (bounds.h - textSurface->h) / 2,
        textSurface->w,
        textSurface->h
    };
    SDL_RenderCopy(renderer, textTexture, nullptr, &textRect);

    SDL_FreeSurface(textSurface);
    SDL_DestroyTexture(textTexture);
}


void UIRadioGroup::addButton(std::shared_ptr<UIRadioButton> btn) {
    buttons.push_back(btn);
}

void UIRadioGroup::select(int id) {
    selectedID = id;
}

int UIRadioGroup::getSelectedID() const {
    return selectedID;
}

#endif // SDLFORMUI_IMPLEMENTATION
#endif // SDLFORMUI