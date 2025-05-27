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


enum class InputType {
    TEXT,
    NUMERIC,
    EMAIL,
    PASSWORD
};


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
    virtual void setPosition(int x, int y) { bounds.x = x; bounds.y = y; }
    virtual void setSize(int w, int h) { bounds.w = w; bounds.h = h; }
    virtual void setBounds(int x, int y, int w, int h) { bounds = {x, y, w, h}; }
    void setTheme(const UITheme& theme) { customTheme = theme; hasCustomTheme = true; }
    const UITheme& getTheme() const { return hasCustomTheme ? customTheme : UIConfig::getTheme(); }
    SDL_Point getPosition() const { return { bounds.x, bounds.y }; }
    SDL_Point getSize() const { return { bounds.w, bounds.h }; }
    virtual bool isInside(int x, int y) const {
    return x >= bounds.x && x <= bounds.x + bounds.w &&
           y >= bounds.y && y <= bounds.y + bounds.h;
    }

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
    UITextField* setInputType(InputType type);

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
    InputType inputType = InputType::TEXT;
};



class UIComboBox : public UIElement {
public:
    UIComboBox(int x, int y, int w, int h, const std::vector<std::string>& options, int& selectedIndex);

    void setFont(TTF_Font* f);
    void setOnSelect(std::function<void(int)> callback);
    bool isHovered() const override;
    bool isExpanded() const;
    int getItemCount() const;
    int getItemHeight() const;
    const SDL_Rect& getBounds() const;

    void handleEvent(const SDL_Event& e) override;
    void update(float dt) override;
    void render(SDL_Renderer* renderer) override;

private:
    std::vector<std::string> options;
    std::reference_wrapper<int> selectedIndex;
    std::function<void(int)> onSelect;
    TTF_Font* font = nullptr;
    bool expanded = false;
    int hoveredIndex = -1;
    bool hovered = false;
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


class UISpinner : public UIElement {
    public:
        UISpinner(int x, int y, int w, int h, int& bind, int min = 0, int max = 100, int step = 1);

        void setFont(TTF_Font* f);
        void setOnChange(std::function<void(int)> callback);
        bool isHovered() const;

        void handleEvent(const SDL_Event& e) override;
        void update(float dt) override;
        void render(SDL_Renderer* renderer) override;

    private:
        std::reference_wrapper<int> value;
        int minValue;
        int maxValue;
        int step;
        bool hoveredMinus = false;
        bool hoveredPlus = false;
        TTF_Font* font = nullptr;
        std::function<void(int)> onChange;
        enum class HeldButton { NONE, INCREMENT, DECREMENT };
        HeldButton heldButton = HeldButton::NONE;
        Uint32 pressStartTime = 0;
        Uint32 lastStepTime = 0;
};



class UITextArea : public UIElement {
public:
    UITextArea(const std::string& label, int x, int y, int w, int h, std::string& bind, int maxLen = 512);

    void setFont(TTF_Font* f);
    void setPlaceholder(const std::string& text);
    void updateCursorPosition();
    SDL_Rect getScrollbarRect() const;
    void renderScrollbar(SDL_Renderer* renderer);
    bool isScrollbarHovered() const;
    bool isScrollbarDragging() const;

    void handleEvent(const SDL_Event& e) override;
    void update(float dt) override;
    void render(SDL_Renderer* renderer) override;
    bool isHovered() const override;
    int getWordCount() const;

private:
    std::vector<std::string> wrapTextToLines(const std::string& text, TTF_Font* font, int maxWidth);
    std::string label;
    std::reference_wrapper<std::string> linkedText;
    std::string placeholder;
    int maxLength;
    bool hovered = false;
    bool focused = false;
    TTF_Font* font = nullptr;
    Uint32 lastBlinkTime = 0;
    bool cursorVisible = true;
    size_t cursorPos = 0;
    bool scrollbarHovered = false;
    bool scrollbarDragging = false;
    int scrollbarDragStartY = 0;
    int scrollbarThumbStartOffset = 0;
    InputType inputType = InputType::TEXT;
    float scrollOffsetY = 0.0f;
    float contentHeight = 0.0f;
    int cursorX = 0, cursorY = 0;
};

class UIManager {
    public:
        void initCursors();
        void cleanupCursors();
        void addElement(std::shared_ptr<UIElement> el);
        void showPopup(std::shared_ptr<UIPopup> popup);
        std::shared_ptr<UIPopup> GetActivePopup();
        void closePopup();
        void checkCursorForElement(const std::shared_ptr<UIElement>& el, SDL_Cursor*& cursorToUse);
        void handleEvent(const SDL_Event& e);
        void update(float dt);
        void render(SDL_Renderer* renderer);


    private:
        std::vector<std::shared_ptr<UIElement>> elements;
        SDL_Cursor* arrowCursor = nullptr;
        SDL_Cursor* handCursor = nullptr;
        SDL_Cursor* ibeamCursor = nullptr;
        bool handCursorActive = false;
        std::shared_ptr<UIPopup> activePopup;
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
    std::shared_ptr<UIRadioGroup> addRadioGroup(
        const std::vector<std::pair<std::string, int>>& options,
        int& selectedID,
        int width = 300,
        int height = 30,
        int groupSpacing = 10
    );
    std::shared_ptr<UIComboBox> addComboBox(const std::vector<std::string>& options, int& selectedIndex, int width = 300, int height = 30);
    std::shared_ptr<UISpinner> addSpinner(int& bind, int min = 0, int max = 100, int step = 1, int width = 100, int height = 30);
    std::shared_ptr<UITextArea> addTextArea(const std::string& label, std::string& bind, int maxLen = 512, int width = 300, int height = 100);
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
    std::shared_ptr<UIComboBox> ComboBox(const std::vector<std::string>& options, int x, int y, int w, int h, int& selectedIndex, TTF_Font* font = nullptr);
    std::shared_ptr<UISpinner> Spinner(int x, int y, int w, int h, int& bind, int min = 0, int max = 100, int step = 1, TTF_Font* font = nullptr);
    std::shared_ptr<UITextArea> TextArea(const std::string& label, int x, int y, int w, int h, std::string& bind, int maxLen = 512);
    void AddElement(std::shared_ptr<UIElement> element);
    void ShowPopup(std::shared_ptr<UIPopup> popup);
    void ClosePopup();



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


class UIGroupBox : public UIElement {
public:
    UIGroupBox(const std::string& title, int x, int y, int w, int h);

    void addChild(std::shared_ptr<UIElement> child);
    const std::vector<std::shared_ptr<UIElement>>& getChildren() const;

    void handleEvent(const SDL_Event& e) override;
    void update(float dt) override;
    void render(SDL_Renderer* renderer) override;

private:
    std::string title;
    std::vector<std::shared_ptr<UIElement>> children;
    TTF_Font* font = nullptr;
};


class UIDialog : public UIPopup {
public:
    UIDialog(const std::string& title,
             const std::string& message,
             std::function<void()> onOk = nullptr,
             std::function<void()> onCancel = nullptr);

    void render(SDL_Renderer* renderer) override;
    void handleEvent(const SDL_Event& e) override;

    void close();

private:
    std::string title;
    std::string message;
    std::function<void()> onOk;
    std::function<void()> onCancel;

    std::shared_ptr<UIButton> okButton;
    std::shared_ptr<UIButton> cancelButton;
    bool ignoreNextClick = true;
};


class UIPopup : public UIElement {
public:
    UIPopup(int x, int y, int w, int h);
    void addChild(std::shared_ptr<UIElement> el);
    void handleEvent(const SDL_Event& e) override;
    void update(float dt) override;
    void render(SDL_Renderer* renderer) override;

    std::vector<std::shared_ptr<UIElement>> children;

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

UITextField* UITextField::setInputType(InputType type) {
    inputType = type;
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
            std::string input = e.text.text;

            bool valid = true;
            switch (inputType) {
                case InputType::NUMERIC:
                    valid = std::all_of(input.begin(), input.end(), ::isdigit);
                    break;
                case InputType::EMAIL:
                    valid = std::all_of(input.begin(), input.end(), [](char c) {
                        return std::isalnum(c) || c == '@' || c == '.' || c == '-' || c == '_';
                    });
                    break;
                case InputType::PASSWORD:
                case InputType::TEXT:
                default:
                    valid = true;
                    break;
            }

            if (valid) {
                linkedText.get().append(input);
            }
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

    std::string toRender = (inputType == InputType::PASSWORD) ? std::string(linkedText.get().size(), '*') : linkedText.get();
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



UIComboBox::UIComboBox(int x, int y, int w, int h, const std::vector<std::string>& options, int& selectedIndex)
    : options(options), selectedIndex(selectedIndex)
{
    bounds = { x, y, w, h };
}

void UIComboBox::setFont(TTF_Font* f) {
    font = f;
}

void UIComboBox::setOnSelect(std::function<void(int)> callback) {
    onSelect = callback;
}

bool UIComboBox::isHovered() const {
    return hovered;
}

bool UIComboBox::isExpanded() const {
    return expanded;

}

int UIComboBox::getItemCount() const {
    return static_cast<int>(options.size());
}

int UIComboBox::getItemHeight() const {
    return bounds.h;
}

const SDL_Rect& UIComboBox::getBounds() const {
    return bounds;
}

void UIComboBox::handleEvent(const SDL_Event& e) {
    int mx = e.button.x;
    int my = e.button.y;

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        SDL_Point point = { mx, my };

        if (SDL_PointInRect(&point, &bounds)) {
            expanded = !expanded;
        } else if (expanded) {
            int itemHeight = bounds.h;
            for (size_t i = 0; i < options.size(); ++i) {
                SDL_Rect itemRect = { bounds.x, bounds.y + static_cast<int>((i + 1) * itemHeight), bounds.w, itemHeight };
                if (SDL_PointInRect(&point, &itemRect)) {
                    selectedIndex.get() = static_cast<int>(i);
                    if (onSelect) onSelect(static_cast<int>(i));
                    expanded = false;
                    break;
                }
            }
        } else {
            expanded = false;
        }
    }
}

void UIComboBox::update(float) {
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    SDL_Point p = { mx, my };

    hovered = SDL_PointInRect(&p, &bounds);

    if (!expanded) {
        hoveredIndex = -1;
        return;
    }

    int itemHeight = bounds.h;
    hoveredIndex = -1;

    for (size_t i = 0; i < options.size(); ++i) {
        SDL_Rect itemRect = { bounds.x, bounds.y + static_cast<int>((i + 1) * itemHeight), bounds.w, itemHeight };
        if (SDL_PointInRect(&p, &itemRect)) {
            hoveredIndex = static_cast<int>(i);
            break;
        }
    }
}


void UIComboBox::render(SDL_Renderer* renderer) {
    const UITheme& theme = getTheme();
    TTF_Font* activeFont = font ? font : getThemeFont(theme);
    if (!activeFont) return;

    SDL_SetRenderDrawColor(renderer, theme.backgroundColor.r, theme.backgroundColor.g, theme.backgroundColor.b, theme.backgroundColor.a);
    SDL_RenderFillRect(renderer, &bounds);

    SDL_SetRenderDrawColor(renderer, theme.borderColor.r, theme.borderColor.g, theme.borderColor.b, theme.borderColor.a);
    SDL_RenderDrawRect(renderer, &bounds);

    std::string selectedText = options[selectedIndex.get()];
    SDL_Surface* textSurface = TTF_RenderText_Blended(activeFont, selectedText.c_str(), theme.textColor);
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
    }

    float cx = static_cast<float>(bounds.x + bounds.w - 15);
    float cy = static_cast<float>(bounds.y + bounds.h / 2);
    SDL_Vertex vertices[3] = {
        { { cx - 5.0f, cy - 3.0f }, { theme.textColor.r, theme.textColor.g, theme.textColor.b, theme.textColor.a }, { 0, 0 } },
        { { cx, cy + 3.0f },        { theme.textColor.r, theme.textColor.g, theme.textColor.b, theme.textColor.a }, { 0, 0 } },
        { { cx + 5.0f, cy - 3.0f }, { theme.textColor.r, theme.textColor.g, theme.textColor.b, theme.textColor.a }, { 0, 0 } }
    };
    SDL_RenderGeometry(renderer, nullptr, vertices, 3, nullptr, 0);

    if (expanded) {
        for (int i = 0; i < static_cast<int>(options.size()); ++i) {
            SDL_Rect itemRect = { bounds.x, bounds.y + static_cast<int>((i + 1) * bounds.h), bounds.w, bounds.h };
            SDL_Color bgColor = (i == hoveredIndex) ? theme.hoverColor : theme.backgroundColor;
            SDL_SetRenderDrawColor(renderer, bgColor.r, bgColor.g, bgColor.b, bgColor.a);
            SDL_RenderFillRect(renderer, &itemRect);

            SDL_SetRenderDrawColor(renderer, theme.borderColor.r, theme.borderColor.g, theme.borderColor.b, theme.borderColor.a);
            SDL_RenderDrawRect(renderer, &itemRect);

            SDL_Surface* itemSurface = TTF_RenderText_Blended(activeFont, options[i].c_str(), theme.textColor);
            if (itemSurface) {
                SDL_Texture* itemTexture = SDL_CreateTextureFromSurface(renderer, itemSurface);
                SDL_Rect itemTextRect = {
                    itemRect.x + 5,
                    itemRect.y + (itemRect.h - itemSurface->h) / 2,
                    itemSurface->w,
                    itemSurface->h
                };
                SDL_RenderCopy(renderer, itemTexture, nullptr, &itemTextRect);
                SDL_FreeSurface(itemSurface);
                SDL_DestroyTexture(itemTexture);
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


UISpinner::UISpinner(int x, int y, int w, int h, int& bind, int min, int max, int step)
    : value(bind), minValue(min), maxValue(max), step(step)
{
    bounds = { x, y, w, h };
}

void UISpinner::setFont(TTF_Font* f) {
    font = f;
}

void UISpinner::setOnChange(std::function<void(int)> callback) {
    onChange = callback;
}

bool UISpinner::isHovered() const {
    return hoveredMinus || hoveredPlus;
}

void UISpinner::handleEvent(const SDL_Event& e) {
    int mx = e.button.x;
    int my = e.button.y;
    SDL_Point point = { mx, my };

    SDL_Rect minusRect = { bounds.x, bounds.y, bounds.h, bounds.h };
    SDL_Rect plusRect = { bounds.x + bounds.w - bounds.h, bounds.y, bounds.h, bounds.h };

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        if (SDL_PointInRect(&point, &minusRect)) {
            if (value.get() > minValue) {
                value.get() -= step;
                if (onChange) onChange(value.get());
            }
            heldButton = HeldButton::DECREMENT;
            pressStartTime = SDL_GetTicks();
            lastStepTime = pressStartTime;
        } else if (SDL_PointInRect(&point, &plusRect)) {
            if (value.get() < maxValue) {
                value.get() += step;
                if (onChange) onChange(value.get());
            }
            heldButton = HeldButton::INCREMENT;
            pressStartTime = SDL_GetTicks();
            lastStepTime = pressStartTime;
        }
    }

    if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
        heldButton = HeldButton::NONE;
    }
}

void UISpinner::update(float) {
    int mx, my;
    SDL_GetMouseState(&mx, &my);
    SDL_Point point = { mx, my };

    SDL_Rect minusRect = { bounds.x, bounds.y, bounds.h, bounds.h };
    SDL_Rect plusRect = { bounds.x + bounds.w - bounds.h, bounds.y, bounds.h, bounds.h };

    hoveredMinus = SDL_PointInRect(&point, &minusRect);
    hoveredPlus = SDL_PointInRect(&point, &plusRect);

    Uint32 now = SDL_GetTicks();
    if (heldButton != HeldButton::NONE && now - pressStartTime > 400 && now - lastStepTime > 100) {
        if (heldButton == HeldButton::INCREMENT && value.get() < maxValue) {
            value.get() += step;
            if (onChange) onChange(value.get());
        } else if (heldButton == HeldButton::DECREMENT && value.get() > minValue) {
            value.get() -= step;
            if (onChange) onChange(value.get());
        }
        lastStepTime = now;
    }
}

void UISpinner::render(SDL_Renderer* renderer) {
    const UITheme& theme = getTheme();
    TTF_Font* activeFont = font ? font : UIConfig::getDefaultFont();
    if (!activeFont) return;

    SDL_Rect minusRect = { bounds.x, bounds.y, bounds.h, bounds.h };
    SDL_Rect plusRect  = { bounds.x + bounds.w - bounds.h, bounds.y, bounds.h, bounds.h };
    SDL_Rect centerRect = {
        bounds.x + bounds.h,
        bounds.y,
        bounds.w - 2 * bounds.h,
        bounds.h
    };

    SDL_Color minusColor = hoveredMinus ? theme.hoverColor : theme.borderColor;
    SDL_Color plusColor  = hoveredPlus  ? theme.hoverColor : theme.borderColor;
    SDL_Color centerColor = theme.borderColor;

    SDL_SetRenderDrawColor(renderer, theme.backgroundColor.r, theme.backgroundColor.g, theme.backgroundColor.b, theme.backgroundColor.a);
    SDL_RenderFillRect(renderer, &bounds);

    SDL_SetRenderDrawColor(renderer, centerColor.r, centerColor.g, centerColor.b, centerColor.a);
    SDL_RenderDrawRect(renderer, &centerRect);

    SDL_SetRenderDrawColor(renderer, minusColor.r, minusColor.g, minusColor.b, 255);
    SDL_RenderDrawLine(renderer, minusRect.x, minusRect.y, minusRect.x + minusRect.w, minusRect.y);
    SDL_RenderDrawLine(renderer, minusRect.x, minusRect.y, minusRect.x, minusRect.y + minusRect.h);
    SDL_RenderDrawLine(renderer, minusRect.x, minusRect.y + minusRect.h - 1, minusRect.x + minusRect.w, minusRect.y + minusRect.h - 1);

    SDL_SetRenderDrawColor(renderer, plusColor.r, plusColor.g, plusColor.b, 255);
    SDL_RenderDrawLine(renderer, plusRect.x, plusRect.y, plusRect.x + plusRect.w, plusRect.y);
    SDL_RenderDrawLine(renderer, plusRect.x + plusRect.w - 1, plusRect.y, plusRect.x + plusRect.w - 1, plusRect.y + plusRect.h);
    SDL_RenderDrawLine(renderer, plusRect.x, plusRect.y + plusRect.h - 1, plusRect.x + plusRect.w, plusRect.y + plusRect.h - 1);

    SDL_SetRenderDrawColor(renderer, minusColor.r, minusColor.g, minusColor.b, 255);
    SDL_RenderDrawLine(renderer,
        minusRect.x + minusRect.w / 4,
        minusRect.y + minusRect.h / 2,
        minusRect.x + 3 * minusRect.w / 4,
        minusRect.y + minusRect.h / 2);

    SDL_SetRenderDrawColor(renderer, plusColor.r, plusColor.g, plusColor.b, 255);
    SDL_RenderDrawLine(renderer,
        plusRect.x + plusRect.w / 2,
        plusRect.y + plusRect.h / 4,
        plusRect.x + plusRect.w / 2,
        plusRect.y + 3 * plusRect.h / 4);
    SDL_RenderDrawLine(renderer,
        plusRect.x + plusRect.w / 4,
        plusRect.y + plusRect.h / 2,
        plusRect.x + 3 * plusRect.w / 4,
        plusRect.y + plusRect.h / 2);

    std::ostringstream oss;
    oss << value.get();
    SDL_Surface* surface = TTF_RenderText_Blended(activeFont, oss.str().c_str(), theme.textColor);
    if (surface) {
        SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
        SDL_Rect textRect = {
            centerRect.x + (centerRect.w - surface->w) / 2,
            centerRect.y + (centerRect.h - surface->h) / 2,
            surface->w,
            surface->h
        };
        SDL_RenderCopy(renderer, texture, nullptr, &textRect);
        SDL_FreeSurface(surface);
        SDL_DestroyTexture(texture);
    }
}




UITextArea::UITextArea(const std::string& label, int x, int y, int w, int h, std::string& bind, int maxLen)
    : label(label), linkedText(bind), maxLength(maxLen)
{
    bounds = { x, y, w, h };
}

void UITextArea::setFont(TTF_Font* f) {
    font = f;
}

void UITextArea::setPlaceholder(const std::string& text) {
    placeholder = text;
}

void UITextArea::handleEvent(const SDL_Event& e) {
    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        SDL_Point p{ e.button.x, e.button.y };
        bool wasFocused = focused;
        focused = SDL_PointInRect(&p, &bounds);
        if (!wasFocused && focused) {
            SDL_StartTextInput();
            cursorPos = linkedText.get().size();
            lastBlinkTime = SDL_GetTicks();
            cursorVisible = true;
        } else if (wasFocused && !focused) {
            SDL_StopTextInput();
        }
        SDL_Rect sb = getScrollbarRect();
        if (contentHeight > bounds.h) {
            float vr = float(bounds.h) / contentHeight;
            int th = std::max(int(bounds.h * vr), 20);
            int maxThumb = bounds.h - th;
            int ty = sb.y + int((scrollOffsetY / (contentHeight - bounds.h)) * maxThumb);
            SDL_Rect thumb{ sb.x, ty, sb.w, th };
            if (SDL_PointInRect(&p, &thumb)) {
                scrollbarDragging = true;
                scrollbarDragStartY = e.button.y;
                scrollbarThumbStartOffset = scrollOffsetY;
            }
        }
    } else if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
        scrollbarDragging = false;
    } else if (e.type == SDL_MOUSEMOTION) {
        if (scrollbarDragging) {
            int dy = e.motion.y - scrollbarDragStartY;
            scrollOffsetY = scrollbarThumbStartOffset + dy * (contentHeight - bounds.h) / bounds.h;
            scrollOffsetY = std::clamp(scrollOffsetY, 0.0f, contentHeight - float(bounds.h));
        }
        if (contentHeight > bounds.h) {
            SDL_Point p{ e.motion.x, e.motion.y };
            SDL_Rect sb = getScrollbarRect();
            float vr = float(bounds.h) / contentHeight;
            int th = std::max(int(bounds.h * vr), 20);
            int maxThumb = bounds.h - th;
            int ty = sb.y + int((scrollOffsetY / (contentHeight - bounds.h)) * maxThumb);
            SDL_Rect thumb{ sb.x, ty, sb.w, th };
            scrollbarHovered = SDL_PointInRect(&p, &thumb);
        } else {
            scrollbarHovered = false;
        }
    } else if (focused && e.type == SDL_TEXTINPUT) {
        std::string in = e.text.text;
        bool valid = true;
        switch (inputType) {
            case InputType::NUMERIC:
                valid = std::all_of(in.begin(), in.end(), ::isdigit);
                break;
            case InputType::EMAIL:
                valid = std::all_of(in.begin(), in.end(), [](char c){ return std::isalnum(c) || c=='@' || c=='.' || c=='-' || c=='_'; });
                break;
            default:
                break;
        }
        size_t room = maxLength > 0 ? maxLength - linkedText.get().size() : in.size();
        if (room > 0 && valid) {
            if (in.size() > room) in.resize(room);
            linkedText.get().insert(cursorPos, in);
            cursorPos += in.size();
        }
        updateCursorPosition();
    } else if (focused && e.type == SDL_KEYDOWN) {
        if (e.key.keysym.sym == SDLK_BACKSPACE && cursorPos > 0) {
            linkedText.get().erase(cursorPos - 1, 1);
            cursorPos--;
        } else if (e.key.keysym.sym == SDLK_RETURN) {
            if (linkedText.get().length() < size_t(maxLength)) {
                linkedText.get().insert(cursorPos, "\n");
                cursorPos++;
            }
        } else if (e.key.keysym.sym == SDLK_LEFT && cursorPos > 0) {
            cursorPos--;
        } else if (e.key.keysym.sym == SDLK_RIGHT && cursorPos < linkedText.get().size()) {
            cursorPos++;
        }
        lastBlinkTime = SDL_GetTicks();
        cursorVisible = true;
        updateCursorPosition();
    } else if (e.type == SDL_MOUSEWHEEL) {
        int mx, my; SDL_GetMouseState(&mx, &my);
        if (mx >= bounds.x && mx <= bounds.x + bounds.w && my >= bounds.y && my <= bounds.y + bounds.h) {
            int lh = TTF_FontHeight(font ? font : UIConfig::getDefaultFont());
            scrollOffsetY -= e.wheel.y * lh;
            scrollOffsetY = std::clamp(scrollOffsetY, 0.0f, contentHeight - float(bounds.h));
        }
    }
}


void UITextArea::update(float) {
    int mx, my; SDL_GetMouseState(&mx, &my);
    SDL_Point pt{ mx, my };
    hovered = SDL_PointInRect(&pt, &bounds);
    if (linkedText.get().length() > size_t(maxLength)) {
        linkedText.get().resize(maxLength);
        if (cursorPos > static_cast<size_t>(maxLength)) cursorPos = static_cast<size_t>(maxLength);
    }
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
    TTF_Font* fnt = font?font:UIConfig::getDefaultFont();
    auto lines = wrapTextToLines(linkedText.get().empty()?placeholder:linkedText.get(), fnt, bounds.w-10);
    contentHeight = float(lines.size()*TTF_FontHeight(fnt));
    scrollOffsetY = std::clamp(scrollOffsetY, 0.0f, std::max(0.0f, contentHeight - float(bounds.h)));
}



bool UITextArea::isHovered() const {
    return hovered;
}

int UITextArea::getWordCount() const {
    const std::string& text = linkedText.get();
    int count = 0;
    std::istringstream iss(text);
    std::string word;
    while (iss >> word) ++count;
    return count;
}

std::vector<std::string> UITextArea::wrapTextToLines(const std::string& text, TTF_Font* font, int maxWidth) {
    std::vector<std::string> lines;
    if (!font) return lines;
    std::string currentLine, currentWord;
    for (char c : text) {
        if (c == '\n') {
            lines.push_back(currentLine + currentWord);
            currentLine.clear();
            currentWord.clear();
            continue;
        }
        currentWord += c;
        std::string temp = currentLine + currentWord;
        int w, h;
        TTF_SizeText(font, temp.c_str(), &w, &h);
        if (w > maxWidth) {
            if (currentLine.empty()) {
                std::string part;
                for (size_t i = 0; i < currentWord.size(); ++i) {
                    std::string test = currentWord.substr(0, i+1);
                    TTF_SizeText(font, test.c_str(), &w, &h);
                    if (w > maxWidth) break;
                    part = test;
                }
                if (part.empty()) part = currentWord.substr(0,1);
                lines.push_back(part);
                currentWord = currentWord.substr(part.size());
            } else {
                lines.push_back(currentLine);
                currentLine = currentWord;
                currentWord.clear();
            }
        }
        if (c == ' ') {
            currentLine += currentWord;
            currentWord.clear();
        }
    }
    lines.push_back(currentLine + currentWord);
    return lines;
}

void UITextArea::render(SDL_Renderer* renderer) {
    const UITheme& theme = getTheme();
    TTF_Font* fnt = font ? font : UIConfig::getDefaultFont();
    if (!fnt) return;

    if (!label.empty()) {
        SDL_Surface* ls = TTF_RenderText_Blended(fnt, label.c_str(), theme.textColor);
        SDL_Texture* lt = SDL_CreateTextureFromSurface(renderer, ls);
        SDL_Rect labelRect = {bounds.x, bounds.y - ls->h - 4, ls->w, ls->h};
        SDL_RenderCopy(renderer, lt, nullptr, &labelRect);
        SDL_FreeSurface(ls);
        SDL_DestroyTexture(lt);
    }

    SDL_SetRenderDrawColor(renderer, theme.backgroundColor.r, theme.backgroundColor.g, theme.backgroundColor.b, theme.backgroundColor.a);
    SDL_RenderFillRect(renderer, &bounds);
    SDL_SetRenderDrawColor(renderer, theme.borderColor.r, theme.borderColor.g, theme.borderColor.b, theme.borderColor.a);
    SDL_RenderDrawRect(renderer, &bounds);

    SDL_RenderSetClipRect(renderer, &bounds);
    std::string txt = linkedText.get().empty() ? placeholder : linkedText.get();
    SDL_Color col = linkedText.get().empty() ? theme.placeholderColor : theme.textColor;
    auto lines = wrapTextToLines(txt, fnt, bounds.w - 10);
    int lh = TTF_FontHeight(fnt);
    int y = bounds.y + 5 - static_cast<int>(scrollOffsetY);

    for (const auto& line : lines) {
        if (!line.empty()) {
            SDL_Surface* s = TTF_RenderText_Blended(fnt, line.c_str(), col);
            SDL_Texture* t = SDL_CreateTextureFromSurface(renderer, s);
            SDL_Rect dst = {bounds.x + 5, y, s->w, s->h};
            SDL_RenderCopy(renderer, t, nullptr, &dst);
            SDL_FreeSurface(s);
            SDL_DestroyTexture(t);
        }
        y += lh;
    }

    if (focused && cursorVisible) {
        int onScreenY = cursorY - static_cast<int>(scrollOffsetY);
        onScreenY = std::clamp(onScreenY, bounds.y, bounds.y + bounds.h - lh);
        SDL_SetRenderDrawColor(renderer, theme.cursorColor.r, theme.cursorColor.g, theme.cursorColor.b, theme.cursorColor.a);
        SDL_Rect cursorRect = {cursorX, onScreenY, 2, lh};
        SDL_RenderFillRect(renderer, &cursorRect);
    }

    if (focused || !linkedText.get().empty()) {
        int words = getWordCount();
        std::string wcLabel = std::to_string(words) + " words";
        SDL_Surface* wcSurface = TTF_RenderText_Blended(fnt, wcLabel.c_str(), theme.placeholderColor);
        if (wcSurface) {
            SDL_Texture* wcTexture = SDL_CreateTextureFromSurface(renderer, wcSurface);
            SDL_Rect wcRect = {
                bounds.x,
                bounds.y + bounds.h + 4,
                wcSurface->w,
                wcSurface->h
            };
            SDL_RenderCopy(renderer, wcTexture, nullptr, &wcRect);
            SDL_FreeSurface(wcSurface);
            SDL_DestroyTexture(wcTexture);
        }
    }

    SDL_RenderSetClipRect(renderer, nullptr);
    if (contentHeight > bounds.h) {
        renderScrollbar(renderer);
    }
}


void UITextArea::updateCursorPosition() {
    TTF_Font* fnt = font ? font : UIConfig::getDefaultFont();
    int lh = TTF_FontHeight(fnt);
    std::string before = linkedText.get().substr(0, cursorPos);
    auto lines = wrapTextToLines(before, fnt, bounds.w - 10);
    int caretLine = int(lines.size()) - 1;
    const std::string& last = lines.empty() ? "" : lines.back();
    int w = 0;
    if (!last.empty()) {
        TTF_SizeText(fnt, last.c_str(), &w, nullptr);
    }
    cursorX = bounds.x + 5 + w;
    cursorY = bounds.y + 5 + caretLine * lh;
    float top = float(cursorY - bounds.y);
    float bottom = top + lh;
    if (top < scrollOffsetY) {
        scrollOffsetY = top;
    } else if (bottom > scrollOffsetY + bounds.h) {
        scrollOffsetY = bottom - bounds.h;
    }
    scrollOffsetY = std::clamp(scrollOffsetY, 0.0f, std::max(0.0f, contentHeight - float(bounds.h)));
}

SDL_Rect UITextArea::getScrollbarRect() const {
    const int W = 10;
    return { bounds.x + bounds.w - W - 2, bounds.y + 2, W, bounds.h - 4 };
}

void UITextArea::renderScrollbar(SDL_Renderer* renderer) {
    const UITheme& theme = getTheme();
    SDL_Rect sb = getScrollbarRect();
    float vr = float(bounds.h)/contentHeight;
    int th = std::max(int(bounds.h*vr), 20);
    float maxScroll = contentHeight - bounds.h;
    int maxThumb = bounds.h - th;
    int ty = sb.y + (maxScroll>0 ? int(scrollOffsetY/maxScroll*maxThumb) : 0);
    SDL_SetRenderDrawColor(renderer, theme.sliderTrackColor.r,theme.sliderTrackColor.g,theme.sliderTrackColor.b,150);
    SDL_RenderFillRect(renderer, &sb);
    SDL_Rect thumb{ sb.x, ty, sb.w, th };
    SDL_SetRenderDrawColor(renderer, theme.sliderThumbColor.r,theme.sliderThumbColor.g,theme.sliderThumbColor.b,200);
    SDL_RenderFillRect(renderer, &thumb);
}

bool UITextArea::isScrollbarHovered() const {
    return scrollbarHovered;
}

bool UITextArea::isScrollbarDragging() const {
    return scrollbarDragging;
}


void UIManager::addElement(std::shared_ptr<UIElement> el) {
    elements.push_back(el);
}

void UIManager::showPopup(std::shared_ptr<UIPopup> popup) {
    activePopup = popup;
}

void UIManager::closePopup() {
    activePopup = nullptr;
}

void UIManager::checkCursorForElement(const std::shared_ptr<UIElement>& el, SDL_Cursor*& cursorToUse) {
    if (!el->visible) return;
    el->update(0.0f);
    if (auto ta = dynamic_cast<UITextArea*>(el.get())) {
        if (ta->isScrollbarHovered() || ta->isScrollbarDragging()) {
            return;
        }
        if (ta->isHovered()) {
            cursorToUse = ibeamCursor;
            return;
        }
    }
    if (el->isHovered()) {
        if (dynamic_cast<UITextField*>(el.get())) {
            cursorToUse = ibeamCursor;
        } else if (
            dynamic_cast<UIButton*>(el.get()) ||
            dynamic_cast<UICheckbox*>(el.get()) ||
            dynamic_cast<UIRadioButton*>(el.get()) ||
            dynamic_cast<UISlider*>(el.get()) ||
            dynamic_cast<UIComboBox*>(el.get()) ||
            dynamic_cast<UISpinner*>(el.get())
        ) {
            cursorToUse = handCursor;
        }
    }
    if (auto group = dynamic_cast<UIGroupBox*>(el.get())) {
        for (auto& child : group->getChildren()) {
            checkCursorForElement(child, cursorToUse);
        }
    }
    if (auto combo = dynamic_cast<UIComboBox*>(el.get())) {
        if (combo->isExpanded()) {
            int mx, my;
            SDL_GetMouseState(&mx, &my);
            int itemHeight = combo->getItemHeight();
            int baseY = combo->getBounds().y;
            int itemCount = combo->getItemCount();
            for (int i = 0; i < itemCount; ++i) {
                SDL_Rect itemRect = {
                    combo->getBounds().x,
                    baseY + (i + 1) * itemHeight,
                    combo->getBounds().w,
                    itemHeight
                };
                SDL_Point pt{ mx, my };
                if (SDL_PointInRect(&pt, &itemRect)) {
                    cursorToUse = handCursor;
                    return;
                }
            }
        }
    }
}


void UIManager::handleEvent(const SDL_Event& e) {
    if (activePopup && activePopup->visible) {
        activePopup->handleEvent(e);
        return;
    }

    for (const auto& el : elements) {
        if (el->visible) {
            auto combo = dynamic_cast<UIComboBox*>(el.get());
            if (combo && combo->isExpanded()) {
                combo->handleEvent(e);
                return;
            }
        }
    }

    for (const auto& el : elements) {
        if (el->visible)
            el->handleEvent(e);
    }
}

void UIManager::update(float dt) {
    if (activePopup && !activePopup->visible) {
        activePopup = nullptr;
    }

    SDL_Cursor* cursorToUse = arrowCursor;

    if (activePopup && activePopup->visible) {
        activePopup->update(dt);
        for (const auto& child : activePopup->children) {
            checkCursorForElement(child, cursorToUse);
        }
    } else {
        for (const auto& el : elements) {
            auto combo = dynamic_cast<UIComboBox*>(el.get());
            if (combo && combo->isExpanded()) {
                combo->update(dt);
                checkCursorForElement(el, cursorToUse);
                if (SDL_GetCursor() != cursorToUse)
                    SDL_SetCursor(cursorToUse);
                return;
            }
        }
        for (const auto& el : elements) {
            el->update(dt);
            checkCursorForElement(el, cursorToUse);
        }
    }
    if (SDL_GetCursor() != cursorToUse)
        SDL_SetCursor(cursorToUse);
}


void UIManager::render(SDL_Renderer* renderer) {
    for (auto& el : elements) {
        if (el->visible)
            el->render(renderer);
    }
    if (activePopup && activePopup->visible) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);
        SDL_Rect fullscreen = { 0, 0, 800, 600 };
        SDL_RenderFillRect(renderer, &fullscreen);
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);

        activePopup->render(renderer);
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

std::shared_ptr<UIRadioGroup> Layout::addRadioGroup(
    const std::vector<std::pair<std::string, int>>& options,
    int& selectedID,
    int width,
    int height,
    int groupSpacing
) {
    auto group = std::make_shared<UIRadioGroup>();
    group->select(selectedID);

    for (const auto& [label, id] : options) {
        auto btn = std::make_shared<UIRadioButton>(label, currentX, currentY, width, height, group.get(), id, defaultFont);
        group->addButton(btn);
        FormUI::AddElement(btn);
        currentY += height + groupSpacing;
    }

    return group;
}

std::shared_ptr<UIComboBox> Layout::addComboBox(const std::vector<std::string>& options, int& selectedIndex, int width, int height) {
    auto combo = FormUI::ComboBox(options, currentX, currentY, width, height, selectedIndex, defaultFont);
    currentY += height + spacing;
    return combo;
}

std::shared_ptr<UISpinner> Layout::addSpinner(int& bind, int min, int max, int step, int width, int height) {
    auto spinner = FormUI::Spinner(currentX, currentY, width, height, bind, min, max, step, defaultFont);
    currentY += height + spacing;
    return spinner;
}

std::shared_ptr<UITextArea> Layout::addTextArea(const std::string& label, std::string& bind, int maxLen, int width, int height) {
    auto textArea = FormUI::TextArea(label, currentX, currentY, width, height, bind, maxLen);
    textArea->setFont(defaultFont);
    currentY += height + spacing;
    return textArea;
}

}



namespace FormUI {
    static UIManager uiManager;
    static std::shared_ptr<UIPopup> internalPopup;

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

    std::shared_ptr<UIComboBox> ComboBox(const std::vector<std::string>& options, int x, int y, int w, int h, int& selectedIndex, TTF_Font* font) {
        auto box = std::make_shared<UIComboBox>(x, y, w, h, options, selectedIndex);
        box->setFont(font ? font : UIConfig::getDefaultFont());
        uiManager.addElement(box);
        return box;
    }

    std::shared_ptr<UISpinner> Spinner(int x, int y, int w, int h, int& bind, int min, int max, int step, TTF_Font* font) {
        auto spinner = std::make_shared<UISpinner>(x, y, w, h, bind, min, max, step);
        if (font) spinner->setFont(font);
        AddElement(spinner);
        return spinner;
    }

    std::shared_ptr<UITextArea> TextArea(const std::string& label, int x, int y, int w, int h, std::string& bind, int maxLen) {
        return std::make_shared<UITextArea>(label, x, y, w, h, bind, maxLen);
    }


    void AddElement(std::shared_ptr<UIElement> element) {
        uiManager.addElement(element);
    }

    void ShowPopup(std::shared_ptr<UIPopup> popup) {
        internalPopup = popup;
        uiManager.showPopup(popup);
    }

    void ClosePopup() {
        if (internalPopup) internalPopup->visible = false;
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

    int cx = bounds.x + 12;
    int cy = bounds.y + bounds.h / 2;
    int outerRadius = 10;
    int thickness = 2;

    SDL_Color ringColor = hovered ? theme.borderHoverColor : theme.borderColor;

    UIHelpers::DrawCircleRing(renderer, cx, cy, outerRadius, thickness, ringColor);

    if (group && group->getSelectedID() == id) {
        int innerRadius = outerRadius - thickness - 2;
        UIHelpers::DrawFilledCircle(renderer, cx, cy, innerRadius, ringColor);
    }

    SDL_Surface* textSurface = TTF_RenderText_Blended(activeFont, label.c_str(), theme.textColor);
    if (!textSurface) return;

    SDL_Texture* textTexture = SDL_CreateTextureFromSurface(renderer, textSurface);
    SDL_Rect textRect = {
        bounds.x + 30,
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


UIGroupBox::UIGroupBox(const std::string& title, int x, int y, int w, int h)
    : title(title)
{
    bounds = { x, y, w, h };
    font = getThemeFont(getTheme());
}

void UIGroupBox::addChild(std::shared_ptr<UIElement> child) {
    children.push_back(child);
}

const std::vector<std::shared_ptr<UIElement>>& UIGroupBox::getChildren() const {
    return children;
}

void UIGroupBox::handleEvent(const SDL_Event& e) {
    for (auto& child : children)
        child->handleEvent(e);
}

void UIGroupBox::update(float dt) {
    for (auto& child : children)
        child->update(dt);
}

void UIGroupBox::render(SDL_Renderer* renderer) {
    const UITheme& theme = getTheme();
    SDL_SetRenderDrawColor(renderer,
        theme.borderColor.r,
        theme.borderColor.g,
        theme.borderColor.b,
        theme.borderColor.a);

    int titleWidth = 0, titleHeight = 0;
    if (!title.empty() && font) {
        TTF_SizeText(font, title.c_str(), &titleWidth, &titleHeight);
    }

    const int padding = 8;
    const int gapStart = bounds.x + padding;
    const int gapEnd = gapStart + titleWidth + padding;

    SDL_RenderDrawLine(renderer, bounds.x, bounds.y, gapStart, bounds.y);
    SDL_RenderDrawLine(renderer, gapEnd, bounds.y, bounds.x + bounds.w, bounds.y);

    SDL_RenderDrawLine(renderer, bounds.x, bounds.y, bounds.x, bounds.y + bounds.h);
    SDL_RenderDrawLine(renderer, bounds.x + bounds.w, bounds.y, bounds.x + bounds.w, bounds.y + bounds.h);
    SDL_RenderDrawLine(renderer, bounds.x, bounds.y + bounds.h, bounds.x + bounds.w, bounds.y + bounds.h);

    if (!title.empty() && font) {
        SDL_Surface* surf = TTF_RenderText_Blended(font, title.c_str(), theme.textColor);
        if (surf) {
            SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, surf);
            SDL_Rect textRect = {
                bounds.x + padding + 4,
                bounds.y - surf->h / 2,
                surf->w,
                surf->h
            };
            SDL_RenderCopy(renderer, tex, nullptr, &textRect);
            SDL_FreeSurface(surf);
            SDL_DestroyTexture(tex);
        }
    }

    for (auto& child : children) {
        child->render(renderer);
    }
}


UIDialog::UIDialog(const std::string& title,
                   const std::string& message,
                   std::function<void()> onOk,
                   std::function<void()> onCancel)
    : UIPopup((800 - 400) / 2, (600 - 200) / 2, 400, 200),
      title(title), message(message), onOk(onOk), onCancel(onCancel)
{
    const int dialogWidth = 400;
    const int dialogHeight = 200;
    bounds = {
        (800 - dialogWidth) / 2,
        (600 - dialogHeight) / 2,
        dialogWidth,
        dialogHeight
    };

    TTF_Font* font = UIConfig::getDefaultFont();
    int btnW = 100, btnH = 40, spacing = 20;
    int btnY = bounds.y + bounds.h - btnH - spacing;
    int okX = bounds.x + bounds.w / 2 - btnW - spacing / 2;
    int cancelX = bounds.x + bounds.w / 2 + spacing / 2;

    okButton = std::make_shared<UIButton>("OK", okX, btnY, btnW, btnH, font);
    cancelButton = std::make_shared<UIButton>("Cancel", cancelX, btnY, btnW, btnH, font);

    okButton->setOnClick([this, onOk]() {
        if (onOk) onOk();
        close();
    });

    cancelButton->setOnClick([this, onCancel]() {
        if (onCancel) onCancel();
        close();
    });

    addChild(okButton);
    addChild(cancelButton);
}

void UIDialog::render(SDL_Renderer* renderer) {
    const UITheme& theme = getTheme();
    TTF_Font* font = UIConfig::getDefaultFont();
    if (!font) return;

    SDL_SetRenderDrawColor(renderer, theme.backgroundColor.r, theme.backgroundColor.g, theme.backgroundColor.b, theme.backgroundColor.a);
    SDL_RenderFillRect(renderer, &bounds);

    SDL_SetRenderDrawColor(renderer, theme.borderColor.r, theme.borderColor.g, theme.borderColor.b, theme.borderColor.a);
    SDL_RenderDrawRect(renderer, &bounds);

    SDL_Surface* titleSurf = TTF_RenderText_Blended(font, title.c_str(), theme.textColor);
    SDL_Surface* msgSurf = TTF_RenderText_Blended(font, message.c_str(), theme.textColor);

    if (titleSurf) {
        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, titleSurf);
        SDL_Rect titleRect = {
            bounds.x + 20,
            bounds.y + 20,
            titleSurf->w,
            titleSurf->h
        };
        SDL_RenderCopy(renderer, tex, nullptr, &titleRect);
        SDL_FreeSurface(titleSurf);
        SDL_DestroyTexture(tex);
    }

    if (msgSurf) {
        SDL_Texture* tex = SDL_CreateTextureFromSurface(renderer, msgSurf);
        SDL_Rect msgRect = {
            bounds.x + 20,
            bounds.y + 70,
            msgSurf->w,
            msgSurf->h
        };
        SDL_RenderCopy(renderer, tex, nullptr, &msgRect);
        SDL_FreeSurface(msgSurf);
        SDL_DestroyTexture(tex);
    }

    UIPopup::render(renderer);
}

void UIDialog::handleEvent(const SDL_Event& e) {
    if (ignoreNextClick) {
        if (e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP) return;
        ignoreNextClick = false;
    }
    if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_ESCAPE) {
        close();
        return;
    }
    UIPopup::handleEvent(e);
}

void UIDialog::close() {
    visible = false;
}


UIPopup::UIPopup(int x, int y, int w, int h) {
    bounds = { x, y, w, h };
}

void UIPopup::addChild(std::shared_ptr<UIElement> el) {
    children.push_back(el);
}

void UIPopup::handleEvent(const SDL_Event& e) {
    for (auto& child : children) {
        if (child)  child->handleEvent(e);
    }
}

void UIPopup::update(float dt) {
    for (auto& child : children) {
        child->update(dt);
    }
}

void UIPopup::render(SDL_Renderer* renderer) {
    for (auto& child : children) {
        child->render(renderer);
    }
}

#endif // SDLFORMUI_IMPLEMENTATION
#endif // SDLFORMUI