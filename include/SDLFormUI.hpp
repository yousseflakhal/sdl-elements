#ifndef SDLFORMUI_HPP
#define SDLFORMUI_HPP

#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <functional>
#include <memory>
#include <vector>
#include <algorithm>
#include <iomanip>
#include <sstream>
#include <cmath>
#include <optional>


enum class InputType {
    TEXT,
    NUMERIC,
    EMAIL,
    PASSWORD
};


enum class StyleId { Classic, Minimal /*, Material*/ };
enum class ThemeId { Light, Dark, HighContrast };


struct UIConfig;

struct UITheme {
    SDL_Color backgroundColor   = {100,100,100,255};
    SDL_Color hoverColor        = {130,130,130,255};
    SDL_Color borderColor       = {200,200,200,255};
    SDL_Color borderHoverColor  = {255,255,255,255};
    SDL_Color textColor         = {255,255,255,255};
    SDL_Color placeholderColor  = {160,160,160,255};
    SDL_Color cursorColor       = {255,255,255,255};
    SDL_Color sliderTrackColor  = {80,80,80,255};
    SDL_Color sliderThumbColor  = {180,180,255,255};
    SDL_Color checkboxTickColor = {255,255,255,255};
    SDL_Color focusRing         = {13,110,253,178};
    SDL_Color selectionBg       = {0,120,215,120};

    TTF_Font* font = nullptr;

    UITheme();
    enum class ThemePreset { Neutral, Bootstrap, Solarized, Dracula, Fluent, Material };
    enum class ThemeMode   { Light, Dark, HighContrast };
    enum class Accent      { Blue, Red, Green, Orange, Purple, Teal, Pink, None };
    enum class Shape       { Square, Rounded, Pill };
    enum class Density     { Compact, Default, Comfortable };

    struct ThemeOptions {
        ThemePreset preset = ThemePreset::Neutral;
        ThemeMode   mode   = ThemeMode::Light;
        Accent      accent = Accent::Blue;
        Shape       shape  = Shape::Rounded;
        Density     density= Density::Default;
    };
};

UITheme MakeLightTheme();
UITheme MakeDarkTheme();
UITheme MakeHighContrastTheme();

TTF_Font* getThemeFont(const UITheme& theme);


struct UIStyle;
struct UITheme;

class UIConfig {
public:
    // NOTE: SDLFormUI does NOT take ownership. Caller must manage the lifetime (TTF_CloseFont).
    static void setDefaultFont(TTF_Font* font);
    static TTF_Font* getDefaultFont();

    static void setTheme(const UITheme& theme);
    static const UITheme& getTheme();

    static void setStyle(const UIStyle& style);
    static const UIStyle& getStyle();

    static void setStyle(StyleId id);
    static void setTheme(ThemeId id);
    static void setLook(StyleId style, ThemeId theme);

    static void setStyle(std::string_view name);
    static void setTheme(std::string_view name);
    static void setLook(std::string_view styleName,
                        std::string_view themeName);

    using StyleFactory = UIStyle(*)();
    using ThemeFactory = UITheme(*)();
    static void registerStyle(std::string_view name, StyleFactory f);
    static void registerTheme(std::string_view name, ThemeFactory f);

    static TTF_Font** getDefaultFontPtr();

private:
    static TTF_Font* defaultFont;
    static UITheme   defaultTheme;

    static UIStyle   defaultStyle;
};


struct UITextFieldStyle {
    int radius      = 10;
    int borderPx    = 1;
    SDL_Color bg{};
    SDL_Color fg{};
    SDL_Color placeholder{};
    SDL_Color border{};
    SDL_Color borderHover{};
    SDL_Color borderFocus{};
    SDL_Color selectionBg{};
    SDL_Color caret{};
};

struct UITextAreaStyle {
    int radius   = 10;
    int borderPx = 1;
    SDL_Color bg{};
    SDL_Color fg{};
    SDL_Color placeholder{};
    SDL_Color border{};
    SDL_Color borderHover{};
    SDL_Color borderFocus{};
    SDL_Color selectionBg{};
    SDL_Color caret{};
};

struct UIButtonStyle {
    int radius   = 10;
    int borderPx = 1;
    SDL_Color text{};
    SDL_Color border{};
    SDL_Color borderFocus{};
};

struct UICheckboxStyle {
    int boxSize   = 18;
    int radius    = 4;
    int borderPx  = 1;
    int spacingPx = 8;
    SDL_Color text{};
    SDL_Color boxBg{};
    SDL_Color border{};
    SDL_Color borderHover{};
    SDL_Color borderFocus{};
    SDL_Color tick{};
};

struct UIGroupBoxStyle {
    int  radius    = 10;
    int  borderPx  = 1;
    int  titlePadX = 8;
    int  titlePadY = 2;
    SDL_Color title;
    SDL_Color border;
    SDL_Color bg;
};

struct UIRadioStyle {
    int outerRadius     = 9;
    int ringThickness   = 5;
    int borderThickness = 2;
    int spacingPx       = 12;
    int gapTextPx       = 18;
    SDL_Color text{};
    SDL_Color border{};
    SDL_Color borderHover{};
    SDL_Color borderFocus{};
    SDL_Color selected{};
};

struct UIComboBoxStyle {
    int radius   = 10;
    int borderPx = 1;
    int padX     = 10;
    SDL_Color fieldBg{};
    SDL_Color fieldFg{};
    SDL_Color placeholder{};
    SDL_Color border{};
    SDL_Color borderHover{};
    SDL_Color borderFocus{};
    SDL_Color caret;
    SDL_Color menuBg{};
    SDL_Color menuBorder{};
    SDL_Color itemFg{};
    SDL_Color itemHoverBg{};
    SDL_Color itemSelectedBg{};
    SDL_Color itemSelectedFg{};
};

struct UISpinnerStyle {
    int radius   = 10;
    int borderPx = 1;
    int padX     = 10;
    SDL_Color fieldBg{};
    SDL_Color fieldBorder{};
    SDL_Color text{};
    SDL_Color btnBg{};
    SDL_Color btnBgHover{};
    SDL_Color btnBorder{};
    SDL_Color btnGlyph{};
    SDL_Color focusRing{};
};

struct UISliderStyle {
    int trackH = 6;
    SDL_Color track{};
    SDL_Color thumb{};
    SDL_Color thumbDrag{};
    SDL_Color focusRing{};
};

struct UILabelStyle {
    SDL_Color fg;
};

struct PopupStyle {
    SDL_Color bg;
    SDL_Color border;
    SDL_Color borderFocus;
    int       radius;
    int       borderPx;
    int       pad;
};

UITextFieldStyle MakeTextFieldStyle(const UITheme& t, const UIStyle& s);
UITextAreaStyle  MakeTextAreaStyle (const UITheme& t, const UIStyle& s);
UIButtonStyle    MakeButtonStyle   (const UITheme& t, const UIStyle& s);
UICheckboxStyle  MakeCheckboxStyle (const UITheme& t, const UIStyle& s);
UIGroupBoxStyle  MakeGroupBoxStyle (const UITheme& t, const UIStyle& s);
UIRadioStyle     MakeRadioStyle    (const UITheme& t, const UIStyle& s);
UIComboBoxStyle  MakeComboBoxStyle (const UITheme& t, const UIStyle& s);
UISpinnerStyle   MakeSpinnerStyle  (const UITheme& t, const UIStyle& s);
UISliderStyle    MakeSliderStyle   (const UITheme& t, const UIStyle&);
UISliderStyle    MakeSliderStyle   (const UITheme& t);
UILabelStyle     MakeLabelStyle    (const UITheme& th, const UIStyle&);
UILabelStyle     MakeLabelStyle    (const UITheme& th);
PopupStyle       MakePopupStyle    (const UITheme& th, const UIStyle& s);

UITextFieldStyle MakeTextFieldStyle(const UITheme& t);
UITextAreaStyle  MakeTextAreaStyle (const UITheme& t);
UIButtonStyle    MakeButtonStyle   (const UITheme& t);
UICheckboxStyle  MakeCheckboxStyle (const UITheme& t);
UIGroupBoxStyle  MakeGroupBoxStyle (const UITheme& t);
UIRadioStyle     MakeRadioStyle    (const UITheme& t);
UIComboBoxStyle  MakeComboBoxStyle (const UITheme& t);
UISpinnerStyle   MakeSpinnerStyle  (const UITheme& t);
PopupStyle       MakePopupStyle    (const UITheme& th);


struct UIStyle {
    int radiusSm   = 6;
    int radiusMd   = 10;
    int radiusLg   = 16;
    int borderThin = 1;
    int borderThick= 2;
    int padSm      = 6;
    int padMd      = 10;
    int padLg      = 16;
};

UIStyle MakeClassicStyle();
UIStyle MakeMinimalStyle();


class UIElement {
public:
    SDL_Rect bounds;
    bool visible = true;
    bool enabled = true;

    virtual void handleEvent(const SDL_Event& e) = 0;
    virtual bool isHovered() const { return false; }
    virtual void update(float dt) = 0;
    virtual void render(SDL_Renderer* renderer) = 0;
    virtual void setPosition(int x, int y) { bounds.x = x; bounds.y = y; }
    virtual void setSize(int w, int h) { bounds.w = w; bounds.h = h; }
    virtual void setBounds(int x, int y, int w, int h) { bounds = {x, y, w, h}; }
    virtual bool isFocusable() const { return false; }

    void setTheme(const UITheme& theme) { customTheme = theme; hasCustomTheme = true; }
    const UITheme& getTheme() const { return hasCustomTheme ? customTheme : UIConfig::getTheme(); }

    void setStyle(const UIStyle& style) { customStyle = style; hasCustomStyle = true; }
    const UIStyle& getStyle() const { return hasCustomStyle ? customStyle : UIConfig::getStyle(); }
    void clearThemeOverride() { hasCustomTheme = false; }
    void clearStyleOverride() { hasCustomStyle = false; }

    SDL_Point getPosition() const { return { bounds.x, bounds.y }; }
    SDL_Point getSize() const { return { bounds.w, bounds.h }; }
    virtual bool isInside(int x, int y) const {
        return x >= bounds.x && x <= bounds.x + bounds.w &&
               y >= bounds.y && y <= bounds.y + bounds.h;
    }
    void setEnabled(bool e) { enabled = e; }
    bool isEnabled() const { return enabled; }

    virtual ~UIElement() = default;

private:
    UITheme customTheme;
    UIStyle customStyle;
    bool hasCustomTheme = false;
    bool hasCustomStyle = false;
};


class UIPopup : public UIElement {
public:
    UIPopup(int x, int y, int w, int h);
    void addChild(std::shared_ptr<UIElement> el);
    void handleEvent(const SDL_Event& e) override;
    void update(float dt) override;
    void render(SDL_Renderer* renderer) override;
    int getPadFromTheme() const {
        return MakePopupStyle(getTheme(), getStyle()).pad;
    }
    void centerInRenderer(SDL_Renderer* r) {
        if (!r) return;
        int rw = 0, rh = 0;
        if (SDL_GetRendererOutputSize(r, &rw, &rh) == 0) {
            setBounds((rw - bounds.w) / 2, (rh - bounds.h) / 2, bounds.w, bounds.h);
        }
    }

    std::vector<std::shared_ptr<UIElement>> children;

};


class UIButton;

class UIDialog : public UIPopup {
public:
    UIDialog(const std::string& title,
             const std::string& message,
             std::function<void()> onOk = nullptr,
             std::function<void()> onCancel = nullptr);

    void render(SDL_Renderer* renderer) override;
    void handleEvent(const SDL_Event& e) override;

    void close();
    void setBounds(int x, int y, int w, int h) override;


private:
    std::string title;
    std::string message;
    std::function<void()> onOk;
    std::function<void()> onCancel;
    void layoutButtons();

    std::shared_ptr<UIButton> okButton;
    std::shared_ptr<UIButton> cancelButton;
    bool ignoreNextClick = true;
};


namespace UIHelpers {
    void DrawFilledCircle(SDL_Renderer* renderer, int cx, int cy, int radius, SDL_Color color);
    void DrawCircleRing(SDL_Renderer* renderer, int cx, int cy, int radius, int thickness, SDL_Color color);
    void FillRoundedRect(SDL_Renderer* renderer,
                                  int x, int y, int w, int h,
                                  int radius, SDL_Color color);
    inline SDL_Color AdjustBrightness(SDL_Color c, int delta) {
        auto clamp = [](int v){ return std::max(0, std::min(255, v)); };
        return SDL_Color{ Uint8(clamp(c.r + delta)), Uint8(clamp(c.g + delta)),
                          Uint8(clamp(c.b + delta)), c.a };
    }
    inline SDL_Color WithAlpha(SDL_Color c, Uint8 a) { c.a = a; return c; }
    void DrawShadowRoundedRect(SDL_Renderer* r, const SDL_Rect& rect, int radius, int offset, Uint8 alpha);
    inline float RelativeLuma(SDL_Color c) {
        auto lin = [](float u){ u/=255.0f; return (u<=0.04045f)? u/12.92f : powf((u+0.055f)/1.055f, 2.4f); };
        float R = lin(c.r), G = lin(c.g), B = lin(c.b);
        return 0.2126f*R + 0.7152f*G + 0.0722f*B;
    }
    inline SDL_Color Lighten(SDL_Color c, int delta) { return AdjustBrightness(c, +std::abs(delta)); }
    inline SDL_Color Darken (SDL_Color c, int delta) { return AdjustBrightness(c, -std::abs(delta)); }

    inline SDL_Color PickHoverColor(SDL_Color bg) {
        float L = RelativeLuma(bg);
        if (L < 0.10f) {
            return Lighten(bg, 12);
        } else {
            return Darken(bg, 12);
        }
    }

    inline SDL_Color PickFocusRing(SDL_Color bg) {
        float L = RelativeLuma(bg);
        return (L > 0.85f) ? Darken(bg, 40) : Lighten(bg, 40);
    }

    void StrokeRoundedRectOutside(SDL_Renderer* r,
                                  const SDL_Rect& innerRect,
                                  int radius,
                                  int thickness,
                                  SDL_Color ringColor,
                                  SDL_Color innerBg);
    void DrawRoundStrokeLine(SDL_Renderer* r, float x1, float y1, float x2, float y2, float thickness, SDL_Color color);
    void DrawCheckmark(SDL_Renderer* r, const SDL_Rect& box, float thickness, SDL_Color color, float pad);
    inline SDL_Color RGBA(int r, int g, int b, int a = 255) {
        auto clip = [](int v){ return static_cast<Uint8>(std::clamp(v, 0, 255)); };
        return SDL_Color{ clip(r), clip(g), clip(b), clip(a) };
    }

    void DrawChevronDown(SDL_Renderer* r, int cx, int cy, int width, int height, float thickness, SDL_Color color);
    struct SurfaceDeleter {
        void operator()(SDL_Surface* s) const noexcept {
            if (s) SDL_FreeSurface(s);
        }
    };

    struct TextureDeleter {
        void operator()(SDL_Texture* t) const noexcept {
            if (t) SDL_DestroyTexture(t);
        }
    };

    using UniqueSurface = std::unique_ptr<SDL_Surface, SurfaceDeleter>;
    using UniqueTexture = std::unique_ptr<SDL_Texture, TextureDeleter>;

    inline UniqueSurface MakeSurface(SDL_Surface* raw) {
        return UniqueSurface(raw);
    }

    inline UniqueTexture MakeTexture(SDL_Texture* raw) {
        return UniqueTexture(raw);
    }

}


class UIRadioGroup;

class UIRadioButton : public UIElement {
public:
    UIRadioButton(const std::string& label, int x, int y, int w, int h, UIRadioGroup* group, int id, TTF_Font* font = nullptr);

    void handleEvent(const SDL_Event& e) override;
    void update(float dt) override;
    void render(SDL_Renderer* renderer) override;
    bool isHovered() const override;
    bool isFocusable() const override { return true; }
    void setFont(TTF_Font* font);
    int getID() const;


    UIRadioButton* setFocusable(bool f) { focusable = f; return this; }
    bool isFocused() const { return focused; }

private:
    std::string label;
    int id;
    UIRadioGroup* group = nullptr;
    TTF_Font* font = nullptr;
    bool hovered = false;
    bool pressed = false;
    bool focused = false;
    bool focusable = true;
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


class UIButton : public UIElement {
public:
    UIButton(const std::string& text, int x, int y, int w, int h, TTF_Font* f = nullptr);
    void setOnClick(std::function<void()> callback);
    void setText(const std::string& newText);
    const std::string& getText() const;
    void handleEvent(const SDL_Event& e) override;
    void update(float dt) override;
    void render(SDL_Renderer* renderer) override;
    bool isFocusable() const override { return focusable; }
    void setFont(TTF_Font* f);
    bool isHovered() const;
    UIButton* setTextColor(SDL_Color c);
    UIButton* setBackgroundColor(SDL_Color c);
    UIButton* setBorderColor(SDL_Color c);
    UIButton* setCornerRadius(int r) { cornerRadius = (r < 0 ? 0 : r); return this; }
    UIButton* setBorderThickness(int px) { borderPx = (px < 0 ? 0 : px); return this; }
    UIButton* setFocusable(bool f) { focusable = f; return this; }
    bool isFocused() const { return focused; }

private:
    std::string label;
    std::function<void()> onClick;
    bool hovered = false;
    bool pressed = false;
    TTF_Font* font = nullptr;
    std::optional<SDL_Color> customTextColor;
    std::optional<SDL_Color> customBgColor;
    std::optional<SDL_Color> customBorderColor;
    int cornerRadius = 4;
    int borderPx     = 1;
    bool focused = false;
    bool focusable = true;
    int pressOffset = 1;
};


class UILabel : public UIElement {
public:
    UILabel(const std::string& text, int x, int y, int w, int h, TTF_Font* font = nullptr);
    ~UILabel();

    void render(SDL_Renderer* renderer) override;
    void update(float dt) override { (void)dt; }
    void handleEvent(const SDL_Event& e) override { (void)e; }

    UILabel* setColor(SDL_Color newColor);
    SDL_Color getColor() const;

    void setText(const std::string& newText);
    const std::string& getText() const { return text; }

private:
    std::string text;
    TTF_Font* font = nullptr;
    SDL_Color color = {255, 255, 255, 255};

    mutable SDL_Texture* cachedTexture = nullptr;
    mutable std::string cachedText;
    mutable SDL_Color cachedColor{0, 0, 0, 0};
    mutable TTF_Font* cachedFont = nullptr;
    mutable int cachedWidth = 0;
    mutable int cachedHeight = 0;

    void invalidateCache() const;
};


class UICheckbox : public UIElement {
public:
    UICheckbox(const std::string& label, int x, int y, int w, int h, bool& bind, TTF_Font* f);

    void setFont(TTF_Font* f);
     UICheckbox* setTextColor(SDL_Color c)    { customTextColor   = c; hasCustomTextColor   = true; return this; }
    UICheckbox* setCheckedColor(SDL_Color c) { customCheckedColor= c; hasCustomCheckedColor= true; return this; }
    UICheckbox* setBoxBackground(SDL_Color c){ customBoxBgColor   = c; hasCustomBoxBgColor   = true; return this; }
    UICheckbox* setBorderColor(SDL_Color c)   { customBorderColor = c; hasCustomBorderColor = true; return this; }
    UICheckbox* setBorderThickness(int px) { borderPx = std::max(0, px); return this; }

    bool isFocusable() const override { return focusable; }

    void handleEvent(const SDL_Event& e) override;
    bool isHovered() const override;
    void update(float dt) override;
    void render(SDL_Renderer* renderer) override;

private:
    std::string label;
    bool hovered = false;
    bool focused = false;
    bool focusable = true;
    std::reference_wrapper<bool> linkedValue;
    TTF_Font* font = nullptr;
    SDL_Color customTextColor{};    bool hasCustomTextColor    = false;
    SDL_Color customCheckedColor{}; bool hasCustomCheckedColor = false;
    SDL_Color customBoxBgColor{};   bool hasCustomBoxBgColor   = false;
    SDL_Color customBorderColor{};  bool hasCustomBorderColor  = false;
    int borderPx = 1;
};


class UITextField : public UIElement {
public:
    UITextField(const std::string& label, int x, int y, int w, int h, std::string& bind, int maxLen = 32);
    UITextField* setPlaceholder(const std::string& text);
    UITextField* setFont(TTF_Font* f);
    UITextField* setInputType(InputType type);

    bool isFocusable() const override { return focusable; }

    bool isHovered() const override;
    void handleEvent(const SDL_Event& e) override;
    void update(float dt) override;
    void render(SDL_Renderer* renderer) override;
    void undo();
    void redo();
    void enableHistory(bool on) { historyEnabled = on; }
    void clearHistory() { undoStack.clear(); redoStack.clear(); }
    std::function<void(const std::string&)> onSubmit;
    UITextField* setOnSubmit(std::function<void(const std::string&)> cb) {
        onSubmit = std::move(cb);
        return this;
    }
    inline bool hasSelection() const { return selAnchor >= 0 && selAnchor != caret; }

    inline std::pair<int,int> selRange() const {
        if (!hasSelection()) return {0, 0};
        return { std::min(caret, selAnchor), std::max(caret, selAnchor) };
    }

    inline void clearSelection() { selAnchor = -1; }

    inline void selectAll() {
        selAnchor = 0;
        caret = (int)linkedText.get().size();
    }



private:
    struct EditRec {
        enum Kind { Typing, Backspace, DeleteKey, Cut, Paste } kind;
        size_t pos{};
        std::string before, after;
        size_t cursorBefore{}, cursorAfter{};
        int selABefore{-1}, selBBefore{-1};
        int selAAfter{-1},  selBAfter{-1};
        Uint32 time{};
    };

    void clearRedo();
    void pushEdit(EditRec e, bool tryCoalesce);
    void replaceRange(size_t a, size_t b, std::string_view repl, EditRec::Kind kind, bool tryCoalesce);
    void applyReplaceNoHistory(size_t a, size_t b, std::string_view repl,
                           size_t newCursor, int newSelA, int newSelB);

    std::vector<EditRec> undoStack;
    std::vector<EditRec> redoStack;
    bool   historyEnabled{true};
    Uint32 coalesceMs{350};
    std::string label;
    std::reference_wrapper<std::string> linkedText;
    int maxLength = 32;
    bool hovered = false;
    bool focused = false;
    bool focusable = true;
    Uint32 lastBlinkTime = 0;
    bool cursorVisible = true;
    std::string placeholder;
    SDL_Color placeholderColor = {160, 160, 160, 255};
    TTF_Font* font = nullptr;
    InputType inputType = InputType::TEXT;
    int cornerRadius = 10;
    int borderPx     = 1;
    Uint32 lastInputTicks = 0;
    Uint32 lastBlinkTicks = 0;
    int caret = 0;
    int selAnchor = -1;
    bool selectingDrag = false;
    int scrollX = 0;
    Uint32 lastClickTicks = 0;
    std::string preedit;
    int preeditCursor = 0;
    int lastClickX = 0, lastClickY = 0;
    int clickCount = 0;
    mutable TTF_Font* cacheFont = nullptr;
    mutable std::vector<int> glyphX;
    mutable std::string measuredTextCache;
    void rebuildGlyphX(TTF_Font* f);
    int  prefixWidth(size_t i) const {
        if (glyphX.empty()) return 0;
        if (i >= glyphX.size()) return glyphX.back();
        return glyphX[i];
    }
};



class UIComboBox : public UIElement {
public:
    UIComboBox(int x, int y, int w, int h, const std::vector<std::string>& options, int& selectedIndex);

    void setFont(TTF_Font* f);
    void setOnSelect(std::function<void(int)> callback);
    bool isHovered() const override;
    bool isExpanded() const;
    int  getItemCount() const;
    int  getItemHeight() const;
    const SDL_Rect& getBounds() const;

    bool isFocusable() const override { return focusable; }

    UIComboBox* setPlaceholder(std::string ph) { placeholder = std::move(ph); return this; }

    void handleEvent(const SDL_Event& e) override;
    void update(float dt) override;
    void render(SDL_Renderer* renderer) override;
    bool isInside(int x, int y) const override;
    UIComboBox* setTextColor(SDL_Color c);

    UIComboBox* setFocusable(bool f) { focusable = f; return this; }
    bool isFocused() const { return focused; }

private:
    std::vector<std::string> options;
    std::reference_wrapper<int> selectedIndex;
    std::function<void(int)> onSelect;
    TTF_Font* font = nullptr;
    std::optional<SDL_Color> customTextColor;

    bool expanded = false;
    int  hoveredIndex = -1;
    bool hovered = false;

    bool focused = false;
    bool focusable = true;
    int  cornerRadius = 8;
    std::string placeholder;
};


class UISlider : public UIElement {
public:
    UISlider(const std::string& label, int x, int y, int w, int h, float& bind, float min, float max);

    bool isFocusable() const override { return focusable; }

    void handleEvent(const SDL_Event& e) override;
    bool isHovered() const override;
    void update(float dt) override;
    void render(SDL_Renderer* renderer) override;

private:
    std::string label;
    std::reference_wrapper<float> linkedValue;
    float minVal = 0.0f, maxVal = 100.0f;

    bool hovered = false;
    bool focused = false;
    bool focusable = true;
    bool dragging = false;

    int thumbRadius = 9;
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



static constexpr size_t MAX_UNDO_STACK = 100;

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
    bool isFocusable() const override { return true; }

    void handleEvent(const SDL_Event& e) override;
    void update(float dt) override;
    void render(SDL_Renderer* renderer) override;
    bool isHovered() const override;
    int getWordCount() const;
    void setSelection(size_t a, size_t b);
    inline std::pair<size_t,size_t> selRange() const {
        return selStart < selEnd ? std::make_pair(selStart, selEnd)
                                : std::make_pair(selEnd, selStart);
    }
    inline void selectAll() {
        selStart = 0;
        selEnd   = linkedText.get().size();
        cursorPos = selEnd;
    }
    bool hasSelection() const {
        return selStart != std::string::npos && selEnd != std::string::npos && selStart != selEnd;
    }
    void clearSelection() { selStart = selEnd = std::string::npos; }
    std::pair<size_t,size_t> selectionRange() const {
        if (!hasSelection()) return {0,0};
        return selStart < selEnd ? std::pair<size_t,size_t>(selStart, selEnd)
                                : std::pair<size_t,size_t>(selEnd, selStart);
    }

    size_t indexFromMouse(int mx, int my) const;
    void setIMERectAtCaret();

private:
    std::vector<std::string> wrapTextToLines(const std::string& text, TTF_Font* font, int maxWidth) const ;
    void rebuildLayout(TTF_Font* fnt, int maxWidthPx) const;
    int lineOfIndex(size_t pos) const;
    int xAtIndex(size_t pos) const;
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
    int cornerRadius = 10;
    int borderPx     = 1;
    int paddingPx    = 8;
    size_t selStart = std::string::npos;
    size_t selEnd   = std::string::npos;
    bool   selectingDrag = false;
    std::string imeText;
    int imeStart = 0;
    int imeLength = 0;
    bool imeActive = false;
    bool selectingMouse = false;
    size_t selectAnchor = 0;
    Uint32 lastClickTicks = 0;
    int clickCount = 0;
    int lastClickX = -10000;
    int lastClickY = -10000;
    int preferredColumn = -1;
    int preferredXpx    = -1;
    mutable std::string      cacheText;
    mutable int              cacheWidthPx = -1;
    mutable TTF_Font*        cacheFont    = nullptr;
    mutable std::vector<std::string> lines;
    mutable std::vector<size_t>       lineStart;
    mutable std::vector<std::vector<int>> prefixX;
    mutable std::string cacheTextNoNL;
    mutable std::vector<size_t> mapOrigToNoNL;
    mutable std::vector<size_t> mapNoNLToOrig;
    struct EditRec {
        size_t pos = 0;
        std::string before;
        std::string after;

        size_t cursorBefore = 0, selABefore = 0, selBBefore = 0;
        size_t cursorAfter  = 0, selAAfter  = 0, selBAfter  = 0;

        enum Kind { Typing, Backspace, DeleteKey, Paste, Cut, Replace } kind = Replace;
        Uint32 time = 0;
    };

    std::vector<EditRec> undoStack;
    std::vector<EditRec> redoStack;
    Uint32 coalesceMs = 500;
    bool historyEnabled = true;

    void undo();
    void redo();
    void clearRedo();
    void pushEdit(EditRec e, bool tryCoalesce);
    void applyReplaceNoHistory(size_t a, size_t b, std::string_view repl,
                            size_t newCursor, size_t newSelA, size_t newSelB);
    void replaceRange(size_t a, size_t b, std::string_view repl, EditRec::Kind kind,
                    bool tryCoalesce);
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


class UIManager {
public:
    enum ShortcutScope { Global=0, WhenNoTextEditing=1, ModalOnly=2 };
    ~UIManager() noexcept;

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

    void registerElement(UIElement* e, bool focusable);
    void setFocusOrder(const std::vector<UIElement*>& order);
    void focusNext();
    void focusPrev();
    void clearFocus();
    void captureMouse(UIElement* e);
    void releaseMouse();
    void setActiveModal(UIElement* m);
    UIElement* activeModal() const;
    void registerShortcut(SDL_Keycode key, Uint16 mods, ShortcutScope scope, std::function<void()> cb);

private:
    bool tryShortcuts_(const SDL_Event& e);
    UIElement* hitTestTopMost_(int x, int y);
    int  findFocusIndex_(UIElement* e);
    void setFocusedIndex_(int idx);
    std::vector<std::shared_ptr<UIElement>> elements;
    SDL_Cursor* arrowCursor = nullptr;
    SDL_Cursor* handCursor = nullptr;
    SDL_Cursor* ibeamCursor = nullptr;
    bool handCursorActive = false;
    std::shared_ptr<UIPopup> activePopup;

    std::vector<UIElement*> focusOrder_;
    int focusedIndex_ = -1;
    UIElement* mouseCaptured_ = nullptr;
    UIElement* activeModal_ = nullptr;

    struct Shortcut {
        SDL_Keycode key;
        Uint16 mods;
        int scope;
        std::function<void()> cb;
    };
    std::vector<Shortcut> shortcuts_;
    bool cursorsReady = false;

    bool pendingPopupClose = false;
    void ensureCursorsInit_();
    void cleanupCursors_();
    std::vector<UIElement*> savedFocusOrder_;
    int savedFocusedIndex_ = -1;
};



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

#ifdef SDLFORMUI_IMPLEMENTATION


UITheme::UITheme() : font(nullptr) {}

TTF_Font* getThemeFont(const UITheme& theme) {
    return theme.font ? theme.font : UIConfig::getDefaultFont();
}

UITheme MakeLightTheme() {
    UITheme t;
    t.backgroundColor   = {255,255,255,255};
    t.textColor         = {33,37,41,255};
    t.placeholderColor  = {160,160,160,255};
    t.borderColor       = {206,212,218,255};
    t.borderHoverColor  = {173,181,189,255};
    t.focusRing         = {13,110,253,178};
    t.selectionBg       = {0,120,215,120};
    t.sliderTrackColor  = {222,226,230,255};
    t.sliderThumbColor  = {13,110,253,255};
    t.checkboxTickColor = {13,110,253,255};
    t.cursorColor       = {33,37,41,255};
    t.font = nullptr;
    return t;
}

UITheme MakeDarkTheme() {
    UITheme th;
    th.backgroundColor   = {18,18,20,255};
    th.hoverColor        = {28,28,32,255};
    th.borderColor       = {60,60,66,255};
    th.borderHoverColor  = {108,117,125,255};
    th.textColor         = {222,226,230,255};
    th.placeholderColor  = {134,142,150,255};
    th.cursorColor       = {255,255,255,255};
    th.selectionBg       = {13,110,253,120};
    th.focusRing         = {13,110,253,178};
    th.sliderTrackColor  = {44,44,50,255};
    th.sliderThumbColor  = {108,117,125,255};
    th.checkboxTickColor = {255,255,255,255};
    return th;
}

UITheme MakeHighContrastTheme() {
    UITheme t;
    t.backgroundColor   = {0,0,0,255};
    t.textColor         = {255,255,255,255};
    t.placeholderColor  = {200,200,200,255};
    t.borderColor       = {255,255,255,255};
    t.borderHoverColor  = {255,255,0,255};
    t.focusRing         = {255,255,0,255};
    t.selectionBg       = {255,255,0,120};
    t.sliderTrackColor  = {120,120,120,255};
    t.sliderThumbColor  = {255,255,255,255};
    t.checkboxTickColor = {255,255,0,255};
    t.cursorColor       = {255,255,255,255};
    return t;
}


TTF_Font* UIConfig::defaultFont = nullptr;
UITheme   UIConfig::defaultTheme;
UIStyle   UIConfig::defaultStyle = MakeClassicStyle();

static UIStyle styleFromEnum(StyleId id) {
    switch (id) {
        case StyleId::Classic: return MakeClassicStyle();
        case StyleId::Minimal: return MakeMinimalStyle();
    }
    return MakeClassicStyle();
}

static UITheme themeFromEnum(ThemeId id) {
    switch (id) {
        case ThemeId::Light:        return MakeLightTheme();
        case ThemeId::Dark:         return MakeDarkTheme();
        case ThemeId::HighContrast: return MakeHighContrastTheme();
    }
    return MakeLightTheme();
}

struct NoThrowHash {
    size_t operator()(const std::string& s) const noexcept {
        return std::hash<std::string>{}(s);
    }
};
struct NoThrowEq {
    bool operator()(const std::string& a, const std::string& b) const noexcept {
        return a == b;
    }
};

static std::unordered_map<std::string, UIConfig::StyleFactory, NoThrowHash, NoThrowEq> gStyleReg = {
    {"classic", &MakeClassicStyle},
    {"minimal", &MakeMinimalStyle},
};


static std::unordered_map<std::string, UIConfig::ThemeFactory, NoThrowHash, NoThrowEq> gThemeReg = {
    {"light",  &MakeLightTheme},
    {"dark",   &MakeDarkTheme},
    {"high-contrast", &MakeHighContrastTheme},
    {"classic-light",  &MakeLightTheme},
    {"classic-dark",   &MakeDarkTheme},
    {"bootstrap-light",&MakeLightTheme},
    {"bootstrap-dark", &MakeDarkTheme},
};

static std::string lower(std::string_view s) {
    std::string out(s);
    std::transform(out.begin(), out.end(), out.begin(),
                   [](unsigned char c){ return std::tolower(c); });
    return out;
}

void UIConfig::setDefaultFont(TTF_Font* font) { defaultFont = font; }
TTF_Font* UIConfig::getDefaultFont() { return defaultFont; }
TTF_Font** UIConfig::getDefaultFontPtr() { return &defaultFont; }

void UIConfig::setTheme(const UITheme& theme) { defaultTheme = theme; }
const UITheme& UIConfig::getTheme() { return defaultTheme; }

void UIConfig::setStyle(const UIStyle& style) { defaultStyle = style; }
const UIStyle& UIConfig::getStyle() { return defaultStyle; }

void UIConfig::setStyle(StyleId id) { setStyle(styleFromEnum(id)); }
void UIConfig::setTheme(ThemeId id) { setTheme(themeFromEnum(id)); }
void UIConfig::setLook(StyleId s, ThemeId t) {
    setStyle(styleFromEnum(s));
    setTheme(themeFromEnum(t));
}

void UIConfig::setStyle(std::string_view name) {
    auto key = lower(name);
    if (auto it = gStyleReg.find(key); it != gStyleReg.end())
        setStyle(it->second());
}

void UIConfig::setTheme(std::string_view name) {
    auto key = lower(name);
    if (auto it = gThemeReg.find(key); it != gThemeReg.end())
        setTheme(it->second());
}

void UIConfig::setLook(std::string_view styleName, std::string_view themeName) {
    setStyle(styleName);
    setTheme(themeName);
}

void UIConfig::registerStyle(std::string_view name, StyleFactory f) {
    gStyleReg[lower(name)] = f;
}

void UIConfig::registerTheme(std::string_view name, ThemeFactory f) {
    gThemeReg[lower(name)] = f;
}


UITextFieldStyle MakeTextFieldStyle(const UITheme& t, const UIStyle& ds) {
    UITextFieldStyle s;
    s.radius       = ds.radiusMd;
    s.borderPx     = ds.borderThin;
    s.bg           = t.backgroundColor;
    s.fg           = t.textColor;
    s.placeholder  = t.placeholderColor;
    s.border       = t.borderColor;
    s.borderHover  = t.borderHoverColor;
    s.borderFocus  = t.focusRing;
    s.selectionBg  = t.selectionBg;
    s.caret        = t.cursorColor;
    return s;
}

UITextAreaStyle MakeTextAreaStyle(const UITheme& t, const UIStyle& ds) {
    UITextAreaStyle s;
    s.radius       = ds.radiusMd;
    s.borderPx     = ds.borderThin;
    s.bg           = t.backgroundColor;
    s.fg           = t.textColor;
    s.placeholder  = t.placeholderColor;
    s.border       = t.borderColor;
    s.borderHover  = t.borderHoverColor;
    s.borderFocus  = t.focusRing;
    s.selectionBg  = t.selectionBg;
    s.caret        = t.cursorColor;
    return s;
}

UIButtonStyle MakeButtonStyle(const UITheme& t, const UIStyle& ds) {
    UIButtonStyle s;
    s.radius      = ds.radiusMd;
    s.borderPx    = ds.borderThin;
    s.text        = t.textColor;
    s.border      = t.borderColor;
    s.borderFocus = t.focusRing;
    return s;
}

UICheckboxStyle MakeCheckboxStyle(const UITheme& t, const UIStyle& ds) {
    UICheckboxStyle s;
    s.boxSize     = 18;
    s.radius      = std::min(ds.radiusSm, 6);
    s.borderPx    = ds.borderThin;
    s.spacingPx   = 8;
    s.text        = t.textColor;
    s.boxBg       = t.backgroundColor;
    s.border      = t.borderColor;
    s.borderHover = t.borderHoverColor;
    s.borderFocus = t.focusRing;
    s.tick        = t.checkboxTickColor;
    return s;
}

UIGroupBoxStyle MakeGroupBoxStyle(const UITheme& t, const UIStyle& ds) {
    UIGroupBoxStyle s;
    s.radius   = ds.radiusMd;
    s.borderPx = ds.borderThin;
    s.title    = t.textColor;
    s.border   = t.borderColor;
    s.bg       = {0,0,0,0};
    return s;
}

UIRadioStyle MakeRadioStyle(const UITheme& t, const UIStyle& ds) {
    UIRadioStyle s;
    s.outerRadius     = std::max(7, ds.radiusSm);
    s.ringThickness   = std::max(3, s.outerRadius / 3);
    s.borderThickness = std::max(1, ds.borderThin);
    s.spacingPx       = 12;
    s.gapTextPx       = 18;
    s.text        = t.textColor;
    s.border      = t.borderColor;
    s.borderHover = t.borderHoverColor;
    s.borderFocus = t.focusRing;
    s.selected    = t.checkboxTickColor;
    return s;
}

UIComboBoxStyle MakeComboBoxStyle(const UITheme& t, const UIStyle& ds) {
    UIComboBoxStyle s;
    s.radius    = ds.radiusMd;
    s.borderPx  = ds.borderThin;
    s.padX      = ds.padMd;
    s.fieldBg   = t.backgroundColor;
    s.fieldFg   = t.textColor;
    s.placeholder = t.placeholderColor;
    s.border    = t.borderColor;
    s.borderHover = t.borderHoverColor;
    s.borderFocus = t.focusRing;
    s.caret     = t.textColor;
    s.menuBg    = t.backgroundColor;
    s.menuBorder= t.borderColor;
    s.itemFg        = t.textColor;
    s.itemHoverBg   = UIHelpers::PickHoverColor(t.backgroundColor);
    s.itemSelectedBg= UIHelpers::Darken(t.backgroundColor, 8);
    s.itemSelectedFg= t.textColor;
    return s;
}

UISpinnerStyle MakeSpinnerStyle(const UITheme& t, const UIStyle& ds) {
    UISpinnerStyle s;
    s.radius      = ds.radiusMd;
    s.borderPx    = ds.borderThin;
    s.padX        = ds.padMd;
    s.fieldBg     = t.backgroundColor;
    s.fieldBorder = t.borderColor;
    s.text        = t.textColor;
    s.btnBg       = t.backgroundColor;
    s.btnBgHover  = UIHelpers::PickHoverColor(t.backgroundColor);
    s.btnBorder   = t.borderColor;
    s.btnGlyph    = t.textColor;
    s.focusRing   = t.focusRing;
    return s;
}

UISliderStyle MakeSliderStyle(const UITheme& t) {
    UISliderStyle s;
    s.track     = t.sliderTrackColor;
    s.thumb     = t.sliderThumbColor;
    s.thumbDrag = UIHelpers::AdjustBrightness(s.thumb, +18);
    s.focusRing = t.focusRing;
    return s;
}

UILabelStyle MakeLabelStyle(const UITheme& th) {
    UILabelStyle st;
    st.fg = th.textColor;
    return st;
}

PopupStyle MakePopupStyle(const UITheme& th, const UIStyle& ds) {
    PopupStyle st;
    st.bg          = UIHelpers::RGBA(th.backgroundColor.r, th.backgroundColor.g, th.backgroundColor.b, 245);
    st.border      = th.borderColor;
    st.borderFocus = th.focusRing;
    st.radius      = ds.radiusMd;
    st.borderPx    = ds.borderThin;
    st.pad         = ds.padLg;
    return st;
}

UILabelStyle MakeLabelStyle(const UITheme& th, const UIStyle&) {
    return MakeLabelStyle(th);
}

UISliderStyle MakeSliderStyle(const UITheme& t, const UIStyle&) {
    return MakeSliderStyle(t);
}

UITextFieldStyle MakeTextFieldStyle(const UITheme& t) { return MakeTextFieldStyle(t, UIConfig::getStyle()); }
UITextAreaStyle  MakeTextAreaStyle (const UITheme& t) { return MakeTextAreaStyle (t, UIConfig::getStyle()); }
UIButtonStyle    MakeButtonStyle   (const UITheme& t) { return MakeButtonStyle   (t, UIConfig::getStyle()); }
UICheckboxStyle  MakeCheckboxStyle (const UITheme& t) { return MakeCheckboxStyle (t, UIConfig::getStyle()); }
UIGroupBoxStyle  MakeGroupBoxStyle (const UITheme& t) { return MakeGroupBoxStyle (t, UIConfig::getStyle()); }
UIRadioStyle     MakeRadioStyle    (const UITheme& t) { return MakeRadioStyle    (t, UIConfig::getStyle()); }
UIComboBoxStyle  MakeComboBoxStyle (const UITheme& t) { return MakeComboBoxStyle (t, UIConfig::getStyle()); }
UISpinnerStyle   MakeSpinnerStyle  (const UITheme& t) { return MakeSpinnerStyle  (t, UIConfig::getStyle()); }
PopupStyle       MakePopupStyle    (const UITheme& th){ return MakePopupStyle    (th, UIConfig::getStyle()); }


UIStyle MakeClassicStyle() {
    UIStyle s;
    s.radiusSm = 6;
    s.radiusMd = 10;
    s.radiusLg = 16;
    s.borderThin = 1;
    s.borderThick = 2;
    s.padSm = 6;
    s.padMd = 10;
    s.padLg = 16;
    return s;
}

UIStyle MakeMinimalStyle() {
    UIStyle s;
    s.radiusSm = 4;
    s.radiusMd = 6;
    s.radiusLg = 10;
    s.borderThin = 1;
    s.borderThick = 1;
    s.padSm = 4;
    s.padMd = 8;
    s.padLg = 12;
    return s;
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

void UIPopup::render(SDL_Renderer* renderer)
{
    const UITheme& th = getTheme();
    const UIStyle& ds = getStyle();
    const PopupStyle st = MakePopupStyle(th, ds);

    const SDL_Rect r = bounds;

    if (st.borderPx > 0) {
        UIHelpers::FillRoundedRect(renderer, r.x, r.y, r.w, r.h, st.radius, st.border);
        SDL_Rect inner { r.x + st.borderPx, r.y + st.borderPx,
                         r.w - 2*st.borderPx, r.h - 2*st.borderPx };
        UIHelpers::FillRoundedRect(renderer, inner.x, inner.y, inner.w, inner.h,
                                   std::max(0, st.radius - st.borderPx), st.bg);
    } else {
        UIHelpers::FillRoundedRect(renderer, r.x, r.y, r.w, r.h, st.radius, st.bg);
    }

    for (auto& child : children) {
        child->render(renderer);
    }
}


UIDialog::UIDialog(const std::string& title,
                   const std::string& message,
                   std::function<void()> onOk,
                   std::function<void()> onCancel)
    : UIPopup(0, 0, 400, 200),
      title(title), message(message), onOk(onOk), onCancel(onCancel)
{
    TTF_Font* font = UIConfig::getDefaultFont();

    okButton     = std::make_shared<UIButton>("OK",     0, 0, 100, 40, font);
    cancelButton = std::make_shared<UIButton>("Cancel", 0, 0, 100, 40, font);

    okButton->setOnClick([this, onOk]() { if (onOk) onOk(); close(); });
    cancelButton->setOnClick([this, onCancel]() { if (onCancel) onCancel(); close(); });

    addChild(okButton);
    addChild(cancelButton);

    layoutButtons();
}

void UIDialog::render(SDL_Renderer* renderer) {
    UIPopup::render(renderer);

    const UITheme& th = getTheme();
    const auto lst = MakeLabelStyle(th);
    const auto pst = MakePopupStyle(th, getStyle());
    TTF_Font* font = UIConfig::getDefaultFont();
    if (!font) return;

    int x = bounds.x + pst.pad;
    int y = bounds.y + pst.pad;

    if (!title.empty()) {
        auto surface = UIHelpers::MakeSurface(
            TTF_RenderUTF8_Blended(font, title.c_str(), lst.fg)
        );

        if (surface) {
            auto texture = UIHelpers::MakeTexture(
                SDL_CreateTextureFromSurface(renderer, surface.get())
            );

            SDL_Rect dst{ x, y, surface->w, surface->h };
            SDL_RenderCopy(renderer, texture.get(), nullptr, &dst);
            y += surface->h + (pst.pad / 2);
        }
    }

    if (!message.empty()) {
        auto surface = UIHelpers::MakeSurface(
            TTF_RenderUTF8_Blended(font, message.c_str(), lst.fg)
        );

        if (surface) {
            auto texture = UIHelpers::MakeTexture(
                SDL_CreateTextureFromSurface(renderer, surface.get())
            );

            SDL_Rect dst{ x, y, surface->w, surface->h };
            SDL_RenderCopy(renderer, texture.get(), nullptr, &dst);
        }
    }
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
    if (e.type == SDL_KEYDOWN &&
        (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_KP_ENTER)) {
        if (onOk) onOk();
        close();
        return;
    }
    UIPopup::handleEvent(e);
}

void UIDialog::close() {
    visible = false;
}

void UIDialog::setBounds(int x, int y, int w, int h) {
    UIElement::setBounds(x, y, w, h);
    layoutButtons();
}

void UIDialog::layoutButtons() {
    if (!okButton || !cancelButton) return;

    const auto pst = MakePopupStyle(getTheme(), getStyle());
    const int btnW = 100, btnH = 40;
    const int gap  = pst.pad / 2;

    const int btnY = bounds.y + bounds.h - pst.pad - btnH;
    const int okX     = bounds.x + (bounds.w - (btnW*2 + gap)) / 2;
    const int cancelX = okX + btnW + gap;

    okButton->setBounds(okX,     btnY, btnW, btnH);
    cancelButton->setBounds(cancelX, btnY, btnW, btnH);
}


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


static inline SDL_Color PrimaryBlue() { return SDL_Color{ 0x0D, 0x6E, 0xFD, 255 }; }
static inline SDL_Color Darken(SDL_Color c, int d){ return UIHelpers::Darken(c, d); }

UIRadioButton::UIRadioButton(const std::string& label, int x, int y, int w, int h, UIRadioGroup* group, int id, TTF_Font* font)
    : label(label), id(id), group(group), font(font)
{
    bounds = { x, y, w, h };
}

void UIRadioButton::setFont(TTF_Font* f) { font = f; }
int  UIRadioButton::getID() const { return id; }
bool UIRadioButton::isHovered() const { return hovered; }

void UIRadioButton::handleEvent(const SDL_Event& e) {
    if (e.type == SDL_USEREVENT) {
        if (e.user.code == 0xF001) {
            focused = true;
            return;
        }
        if (e.user.code == 0xF002) {
            focused = false;
            return;
        }
    }

    if (!enabled) return;

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        SDL_Point p{ e.button.x, e.button.y };
        if (SDL_PointInRect(&p, &bounds)) {
            pressed = true;
        }
    } else if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
        SDL_Point p{ e.button.x, e.button.y };
        if (pressed && SDL_PointInRect(&p, &bounds)) {
            if (group) group->select(id);
        }
        pressed = false;
    }

    if (focused && e.type == SDL_KEYDOWN && enabled) {
        if (e.key.keysym.sym == SDLK_SPACE || e.key.keysym.sym == SDLK_RETURN) {
            if (group) group->select(id);
        }
    }
}

void UIRadioButton::update(float) {
    if (!enabled) { hovered = false; pressed = false; return; }
    int mx, my; SDL_GetMouseState(&mx, &my);
    SDL_Point p{ mx, my };
    hovered = SDL_PointInRect(&p, &bounds);
}

void UIRadioButton::render(SDL_Renderer* renderer) {
    const UITheme& th = getTheme();
    const UIStyle& ds = getStyle();
    const auto st = MakeRadioStyle(th, ds);

    TTF_Font* activeFont = font ? font : (th.font ? th.font : UIConfig::getDefaultFont());
    if (!activeFont) return;

    const bool selected = (group && group->getSelectedID() == id);
    const int cx = bounds.x + st.spacingPx;
    const int cy = bounds.y + bounds.h / 2;

    SDL_Color ringCol   = hovered ? st.borderHover : st.border;
    SDL_Color selectCol = st.selected;
    SDL_Color textCol   = st.text;

    Uint8 globalAlpha = enabled ? 255 : 160;
    ringCol.a   = globalAlpha;
    selectCol.a = globalAlpha;
    textCol.a   = globalAlpha;

    if (focusable && focused) {
        SDL_Color halo = st.borderFocus;
        halo.a = std::min<int>(halo.a, globalAlpha);
        UIHelpers::DrawCircleRing(renderer, cx, cy, st.outerRadius + 3, 3, halo);
    }

    if (selected) {
        SDL_Color c = pressed ? UIHelpers::AdjustBrightness(selectCol, -18) : selectCol;
        UIHelpers::DrawCircleRing(renderer, cx, cy, st.outerRadius, st.ringThickness, c);
    } else {
        SDL_Color c = pressed ? UIHelpers::AdjustBrightness(ringCol, -18) : ringCol;
        UIHelpers::DrawCircleRing(renderer, cx, cy, st.outerRadius, st.borderThickness, c);
    }

    auto surface = UIHelpers::MakeSurface(
        TTF_RenderUTF8_Blended(activeFont, label.c_str(), textCol)
    );

    if (!surface) return;

    auto texture = UIHelpers::MakeTexture(
        SDL_CreateTextureFromSurface(renderer, surface.get())
    );

    SDL_Rect textRect = {
        bounds.x + st.spacingPx + st.outerRadius + st.gapTextPx - st.outerRadius,
        bounds.y + (bounds.h - surface->h)/2,
        surface->w,
        surface->h
    };

    SDL_RenderCopy(renderer, texture.get(), nullptr, &textRect);
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

    if (e.type == SDL_USEREVENT) {
        if (e.user.code == 0xF001) { focused = true;  return; }
        if (e.user.code == 0xF002) { focused = false; return; }
    }
    if (!enabled) return;

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        int mx = e.button.x, my = e.button.y;
        if (mx >= bounds.x && mx <= bounds.x + bounds.w &&
            my >= bounds.y && my <= bounds.y + bounds.h) {
            pressed = true;
            if (focusable) focused = true;
        } else if (focusable) {
            focused = false;
        }
    } else if (e.type == SDL_MOUSEBUTTONUP && pressed) {
        int mx = e.button.x, my = e.button.y;
        if (mx >= bounds.x && mx <= bounds.x + bounds.w &&
            my >= bounds.y && my <= bounds.y + bounds.h) {
            if (onClick) onClick();
        }
        pressed = false;
    }

    if (focusable && focused && e.type == SDL_KEYDOWN && enabled) {
        if (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_SPACE) {
            pressed = true;
        }
    } else if (focusable && focused && e.type == SDL_KEYUP) {
        if ((e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_SPACE) && pressed) {
            if (onClick) onClick();
            pressed = false;
        }
    }
}

void UIButton::update(float) {
    if (!enabled) { hovered = false; pressed = false; return; }
    int mx, my; SDL_GetMouseState(&mx, &my);
    SDL_Point mousePoint = { mx, my };
    hovered = SDL_PointInRect(&mousePoint, &bounds);
}


void UIButton::render(SDL_Renderer* renderer) {
    const UITheme& th = getTheme();
    const UIStyle& ds = getStyle();
    const auto st = MakeButtonStyle(th, ds);

    SDL_Color baseBg     = customBgColor   ? *customBgColor   : th.backgroundColor;
    SDL_Color baseText   = customTextColor ? *customTextColor : st.text;
    SDL_Color baseBorder = customBorderColor? *customBorderColor: st.border;

    Uint8 globalAlpha = enabled ? 255 : 128;

    SDL_Color bg = baseBg;
    if (enabled) {
        if (pressed)      bg = UIHelpers::Darken(baseBg, 10);
        else if (hovered) bg = UIHelpers::PickHoverColor(baseBg);
    }
    bg.a = globalAlpha;

    int effRadius   = (cornerRadius > 0 ? cornerRadius : st.radius);
    int effBorderPx = (borderPx     > 0 ? borderPx     : st.borderPx);

    if (focusable && focused) {
        SDL_Color ring = st.borderFocus;
        ring.a = (Uint8)std::min<int>(178, globalAlpha);
        UIHelpers::StrokeRoundedRectOutside(renderer, bounds, effRadius, effBorderPx + 1, ring, bg);
    }

    SDL_Rect dst = bounds;
    if (pressed) {
        dst.y += pressOffset;
        dst.x += 1;
        dst.w -= 2;
        dst.h -= 2;
    }

    if (effBorderPx > 0) {
        UIHelpers::FillRoundedRect(renderer, dst.x, dst.y, dst.w, dst.h, effRadius, baseBorder);
        SDL_Rect inner{ dst.x + effBorderPx, dst.y + effBorderPx, dst.w - 2*effBorderPx, dst.h - 2*effBorderPx };
        UIHelpers::FillRoundedRect(renderer, inner.x, inner.y, inner.w, inner.h,
                                   std::max(0, effRadius - effBorderPx), bg);
        dst = inner;
    } else {
        UIHelpers::FillRoundedRect(renderer, dst.x, dst.y, dst.w, dst.h, effRadius, bg);
    }

    TTF_Font* activeFont = font ? font
                                : (th.font ? th.font : UIConfig::getDefaultFont());
    if (!activeFont) return;

    SDL_Color txt = baseText;
    txt.a = globalAlpha;

    auto surface = UIHelpers::MakeSurface(
        TTF_RenderUTF8_Blended(activeFont, label.c_str(), txt)
    );

    if (!surface) return;

    auto texture = UIHelpers::MakeTexture(
        SDL_CreateTextureFromSurface(renderer, surface.get())
    );

    if (!texture) return;

    SDL_Rect r = {
        dst.x + (dst.w - surface->w)/2,
        dst.y + (dst.h - surface->h)/2,
        surface->w,
        surface->h
    };

    SDL_RenderCopy(renderer, texture.get(), nullptr, &r);
}


void UIButton::setFont(TTF_Font* f) {
    font = f;
}

bool UIButton::isHovered() const {
    return hovered;
}

UIButton* UIButton::setTextColor(SDL_Color c) {
    customTextColor = c;
    return this;
}

UIButton* UIButton::setBackgroundColor(SDL_Color c) {
    customBgColor = c;
    return this;
}

UIButton* UIButton::setBorderColor(SDL_Color c) {
    customBorderColor = c;
    return this;
}


UILabel::UILabel(const std::string& text, int x, int y, int w, int h, TTF_Font* font)
    : text(text), font(font) {
    bounds = { x, y, w, h };
}

UILabel::~UILabel() {
    if (cachedTexture) {
        SDL_DestroyTexture(cachedTexture);
        cachedTexture = nullptr;
    }
}

void UILabel::invalidateCache() const {
    if (cachedTexture) {
        SDL_DestroyTexture(cachedTexture);
        cachedTexture = nullptr;
    }
    cachedText.clear();
    cachedColor = {0, 0, 0, 0};
    cachedFont = nullptr;
    cachedWidth = 0;
    cachedHeight = 0;
}

void UILabel::render(SDL_Renderer* renderer) {
    const UITheme& th = getTheme();
    const UIStyle& ds = getStyle();
    const auto st = MakeLabelStyle(th, ds);
    TTF_Font* activeFont = font ? font : getThemeFont(th);
    if (!activeFont) return;

    SDL_Color txtCol = (color.a != 0) ? color : st.fg;

    bool needsRebuild = !cachedTexture ||
                        cachedText != text ||
                        cachedFont != activeFont ||
                        std::memcmp(&cachedColor, &txtCol, sizeof(SDL_Color)) != 0;

    if (needsRebuild) {
        if (cachedTexture) {
            SDL_DestroyTexture(cachedTexture);
            cachedTexture = nullptr;
        }

        auto surface = UIHelpers::MakeSurface(
            TTF_RenderUTF8_Blended(activeFont, text.c_str(), txtCol)
        );

        if (!surface) return;

        cachedTexture = SDL_CreateTextureFromSurface(renderer, surface.get());
        if (!cachedTexture) return;

        cachedText = text;
        cachedColor = txtCol;
        cachedFont = activeFont;
        cachedWidth = surface->w;
        cachedHeight = surface->h;

    }

    SDL_Rect dstRect = {
        bounds.x,
        bounds.y + (bounds.h - cachedHeight) / 2,
        cachedWidth,
        cachedHeight
    };

    SDL_RenderCopy(renderer, cachedTexture, nullptr, &dstRect);
}

UILabel* UILabel::setColor(SDL_Color c) {
    if (std::memcmp(&color, &c, sizeof(SDL_Color)) != 0) {
        color = c;
        invalidateCache();
    }
    return this;
}

SDL_Color UILabel::getColor() const {
    return color;
}

void UILabel::setText(const std::string& newText) {
    if (text != newText) {
        text = newText;
        invalidateCache();
    }
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
    if (!enabled) return;

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        int mx = e.button.x, my = e.button.y;
        if (mx >= bounds.x && mx <= bounds.x + bounds.w &&
            my >= bounds.y && my <= bounds.y + bounds.h) {
            linkedValue.get() = !linkedValue.get();
            if (focusable) focused = true;
        } else if (focusable) {
            focused = false;
        }
    }

    if (!focused || !enabled) return;

    if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
            case SDLK_SPACE:
            case SDLK_RETURN:
            case SDLK_KP_ENTER:
                linkedValue.get() = !linkedValue.get();
                break;
            case SDLK_ESCAPE:
                focused = false;
                break;
            default: break;
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
    const UITheme& th = getTheme();
    const UIStyle& ds = getStyle();
    const auto st = MakeCheckboxStyle(th, ds);

    TTF_Font* activeFont = font ? font : (th.font ? th.font : UIConfig::getDefaultFont());
    if (!activeFont) return;

    const bool isChecked = linkedValue.get();

    SDL_Color textCol   = hasCustomTextColor    ? customTextColor    : st.text;
    SDL_Color borderCol = hasCustomBorderColor  ? customBorderColor  : (hovered ? st.borderHover : st.border);
    SDL_Color fillCol   = hasCustomBoxBgColor   ? customBoxBgColor   : st.boxBg;
    SDL_Color tickCol   = hasCustomCheckedColor ? customCheckedColor : st.tick;

    Uint8 globalAlpha = enabled ? 255 : 160;
    textCol.a   = globalAlpha;
    borderCol.a = globalAlpha;
    fillCol.a   = globalAlpha;
    tickCol.a   = globalAlpha;

    SDL_Rect box = { bounds.x, bounds.y + (bounds.h - st.boxSize)/2, st.boxSize, st.boxSize };

    int stroke = std::max<int>(0, (borderPx > 0 ? borderPx : st.borderPx));
    int radius = st.radius;
    int innerRadius = std::max(0, radius - stroke);
    SDL_Rect inner = { box.x + stroke, box.y + stroke, box.w - 2*stroke, box.h - 2*stroke };

    if (focusable && focused) {
        SDL_Color ring = st.borderFocus;
        ring.a = std::min<int>(ring.a, globalAlpha);
        UIHelpers::StrokeRoundedRectOutside(renderer, inner, innerRadius, stroke + 1, ring, fillCol);
    }

    if (stroke > 0) {
        if (fillCol.a == 0) {
            UIHelpers::StrokeRoundedRectOutside(renderer, inner, innerRadius, stroke, borderCol, th.backgroundColor);
        } else {
            UIHelpers::FillRoundedRect(renderer, inner.x, inner.y, inner.w, inner.h, innerRadius, fillCol);
            UIHelpers::StrokeRoundedRectOutside(renderer, inner, innerRadius, stroke, borderCol, fillCol);
        }
    } else {
        if (fillCol.a > 0)
            UIHelpers::FillRoundedRect(renderer, inner.x, inner.y, inner.w, inner.h, innerRadius, fillCol);
    }

    if (isChecked) {
        float pad   = 3.5f;
        float thick = std::clamp(box.w * 0.16f, 1.5f, 3.0f);
        SDL_Rect markBox = box;
        markBox.y -= 1;
        UIHelpers::DrawCheckmark(renderer, markBox, thick, tickCol, pad);
    }

    const int textLeft = box.x + box.w + st.spacingPx;
    auto surface = UIHelpers::MakeSurface(
        TTF_RenderUTF8_Blended(activeFont, label.c_str(), textCol)
    );

    if (!surface) return;

    auto texture = UIHelpers::MakeTexture(
        SDL_CreateTextureFromSurface(renderer, surface.get())
    );

    SDL_Rect tr = { textLeft, bounds.y + (bounds.h - surface->h)/2, surface->w, surface->h };
    SDL_RenderCopy(renderer, texture.get(), nullptr, &tr);
}


void UITextField::clearRedo() { redoStack.clear(); }

static inline bool hasSelRangeInt(const int a, const int b) { return b > a; }

void UITextField::applyReplaceNoHistory(size_t a, size_t b, std::string_view repl,
                                        size_t newCursor, int newSelA, int newSelB) {
    auto& txt = linkedText.get();
    a = std::min(a, txt.size());
    b = std::min(b, txt.size());
    if (b < a) std::swap(a, b);

    txt.replace(a, b - a, repl);

    caret = (int)std::min(newCursor, txt.size());

    if (hasSelRangeInt((int)newSelA, (int)newSelB)) {
        selAnchor = (int)newSelA;
        caret     = (int)newSelB;
    } else {
        clearSelection();
    }

    cursorVisible   = true;
    lastBlinkTicks  = SDL_GetTicks();
}

void UITextField::pushEdit(EditRec e, bool tryCoalesce) {
    clearRedo();

    if (tryCoalesce && !undoStack.empty()) {
        EditRec& p = undoStack.back();

        if (e.kind == EditRec::Typing && p.kind == EditRec::Typing &&
            p.pos + p.after.size() == e.pos && p.before.empty() && e.before.empty() &&
            (e.time - p.time) <= coalesceMs)
        {
            p.after        += e.after;
            p.cursorAfter   = e.cursorAfter;
            p.selAAfter     = e.selAAfter;
            p.selBAfter     = e.selBAfter;
            p.time          = e.time;
            return;
        }

        if (e.kind == EditRec::Backspace && p.kind == EditRec::Backspace &&
            e.pos + e.before.size() == p.pos && e.after.empty() && p.after.empty() &&
            (e.time - p.time) <= coalesceMs)
        {
            p.pos         = e.pos;
            p.before      = e.before + p.before;
            p.cursorAfter = e.cursorAfter;
            p.selAAfter   = e.selAAfter;
            p.selBAfter   = e.selBAfter;
            p.time        = e.time;
            return;
        }
    }

    undoStack.push_back(std::move(e));
}

void UITextField::replaceRange(size_t a, size_t b, std::string_view repl, EditRec::Kind kind,
                               bool tryCoalesce) {
    auto& txt = linkedText.get();
    a = std::min(a, txt.size());
    b = std::min(b, txt.size());
    if (b < a) std::swap(a, b);

    EditRec e;
    e.pos          = a;
    e.before       = txt.substr(a, b - a);
    e.after        = std::string(repl);
    e.cursorBefore = (size_t)caret;

    if (hasSelection()) {
        auto [sa, sb] = selRange();
        e.selABefore = sa; e.selBBefore = sb;
    } else {
        e.selABefore = caret; e.selBBefore = caret;
    }
    e.kind = kind;
    e.time = SDL_GetTicks();

    const size_t newCursor = a + e.after.size();

    applyReplaceNoHistory(a, b, repl, newCursor, newCursor, newCursor);

    e.cursorAfter = (size_t)caret;
    if (hasSelection()) {
        auto [sa, sb] = selRange();
        e.selAAfter = sa; e.selBAfter = sb;
    } else {
        e.selAAfter = caret; e.selBAfter = caret;
    }

    if (historyEnabled) pushEdit(std::move(e), tryCoalesce);
}

void UITextField::undo()
{
    if (undoStack.empty()) return;
    EditRec e = undoStack.back(); undoStack.pop_back();

    size_t a = e.pos;
    size_t b = e.pos + e.after.size();

    redoStack.push_back(e);
    applyReplaceNoHistory(a, b, e.before, e.cursorBefore, e.selABefore, e.selBBefore);
}

void UITextField::redo()
{
    if (redoStack.empty()) return;
    EditRec e = redoStack.back(); redoStack.pop_back();

    size_t a = e.pos;
    size_t b = e.pos + e.before.size();

    undoStack.push_back(e);
    applyReplaceNoHistory(a, b, e.after, e.cursorAfter, e.selAAfter, e.selBAfter);
}

static int clampi(int v, int lo, int hi) { return v < lo ? lo : (v > hi ? hi : v); }

static int textWidth(TTF_Font* font, const std::string& s) {
    int w = 0, h = 0;
    if (font && !s.empty()){ TTF_SizeUTF8(font, s.c_str(), &w, &h);}
    return w;
}

UITextField::UITextField(const std::string& label, int x, int y, int w, int h, std::string& bind, int maxLen)
    : label(label), linkedText(bind), maxLength(maxLen)
{
    bounds = { x, y, w, h };
    lastBlinkTicks = SDL_GetTicks();
}

UITextField* UITextField::setPlaceholder(const std::string& text) {
    placeholder = text;
    return this;
}

UITextField* UITextField::setFont(TTF_Font* f) {
    if (font != f) {
        font = f;
        cacheFont = nullptr;
        measuredTextCache.clear();
        glyphX.clear();
    }
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
    if (e.type == SDL_USEREVENT) {
        if (e.user.code == 0xF001) {
            if (!focused) { focused = true; SDL_StartTextInput(); }
            return;
        }
        if (e.user.code == 0xF002) {
            if (focused) { focused = false; SDL_StopTextInput(); preedit.clear(); clearSelection(); cursorVisible = false; }
            return;
        }
    }
    auto activeFont = font ? font : UIConfig::getDefaultFont();
    auto& textRef = linkedText.get();

    auto isInside = [&](int x, int y) {
        return x >= bounds.x && x < bounds.x + bounds.w && y >= bounds.y && y < bounds.y + bounds.h;
    };
    auto innerR = [&]() {
        if (borderPx <= 0) return bounds;
        SDL_Rect r{bounds.x + borderPx, bounds.y + borderPx, bounds.w - 2 * borderPx, bounds.h - 2 * borderPx};
        if (r.w < 0) r.w = 0;
        if (r.h < 0) r.h = 0;
        return r;
    }();

    auto isCont = [](unsigned char c) { return (c & 0xC0) == 0x80; };
    auto nextCP = [&](const std::string& s, int i) {
        int n = (int)s.size();
        if (i < 0) return 0;
        if (i >= n) return n;
        i++;
        while (i < n && isCont((unsigned char)s[i])) i++;
        return i;
    };
    auto prevCP = [&](const std::string& s, int i) {
        if (i <= 0) return 0;
        i--;
        while (i > 0 && isCont((unsigned char)s[i])) i--;
        return i;
    };
    auto codepointCountUpTo = [&](const std::string& s, int byteIndex) {
        int i = 0, count = 0, n = (int)s.size();
        while (i < n && i < byteIndex) { i = nextCP(s, i); count++; }
        return count;
    };
    auto maskedPrefixForWidth = [&](int byteCount) {
        if (inputType == InputType::PASSWORD) {
            int cps = codepointCountUpTo(textRef, byteCount);
            return std::string(cps, '*');
        } else {
            return textRef.substr(0, byteCount);
        }
    };
    auto caretByteFromX = [&](int mx) {
        int pad = 8;
        int xLocal = mx - (innerR.x + pad) + scrollX;
        if (!activeFont || xLocal <= 0) return 0;
        int n = (int)textRef.size();
        int i = 0, lastGood = 0;
        while (i <= n) {
            std::string pref = maskedPrefixForWidth(i);
            rebuildGlyphX(activeFont);
            int wPref = prefixWidth(pref.size());
            if (wPref > xLocal) break;
            lastGood = i;
            if (i == n) break;
            i = nextCP(textRef, i);
        }
        return lastGood;
    };
    auto ensureCaretVisibleLocal = [&]() {
        if (!activeFont) return;

        const int pad = 8;
        int innerW = innerR.w - 2 * pad;
        if (innerW < 0) innerW = 0;

        rebuildGlyphX(activeFont);
        const int w = prefixWidth(std::min<size_t>(caret, glyphX.size() ? glyphX.size()-1 : 0));

        const int margin = 2;
        const int left  = scrollX + margin;
        const int right = scrollX + innerW - margin;

        if (w < left) {
            scrollX = std::max(0, w - margin);
        } else if (w > right) {
            scrollX = std::max(0, w - innerW + margin);
        }

        const int contentW = glyphX.empty() ? 0 : glyphX.back();
        const int maxScroll = std::max(0, contentW - innerW);
        if (scrollX > maxScroll) scrollX = maxScroll;
    };
    auto updateImeRect = [&]() {
        if (!activeFont) return;
        const int pad = 8;
        const int cursorH = TTF_FontHeight(activeFont);

        rebuildGlyphX(activeFont);
        int wCaret = prefixWidth(std::min<size_t>(caret, glyphX.size() ? glyphX.size()-1 : 0));

        if (!preedit.empty()) {
            auto isContB = [](unsigned char c){ return (c & 0xC0) == 0x80; };
            int preByte = 0, cpLeft = std::max(0, preeditCursor);
            while (preByte < (int)preedit.size() && cpLeft-- > 0) {
                preByte++;
                while (preByte < (int)preedit.size() && isContB((unsigned char)preedit[preByte])) preByte++;
            }

            std::string preSub = (inputType == InputType::PASSWORD)
                ? std::string((int)std::count_if(preedit.begin(), preedit.begin() + preByte,
                    [&](unsigned char ch){ return !isContB(ch); }), '*')
                : preedit.substr(0, preByte);

            int wPre = 0, h = 0;
            if (!preSub.empty()) TTF_SizeUTF8(activeFont, preSub.c_str(), &wPre, &h);
            wCaret += wPre;
        }

        SDL_Rect r{ innerR.x + pad + wCaret - scrollX, innerR.y + (innerR.h - cursorH) / 2, 1, cursorH };
        SDL_SetTextInputRect(&r);
    };
    auto postEditAdjust = [&](){
        ensureCaretVisibleLocal();
        updateImeRect();
        cursorVisible  = true;
        lastBlinkTicks = SDL_GetTicks();
    };
    auto moveLeft = [&](bool word, bool withSel) {
        int oldCaret = caret;
        int c;
        if (!word) c = prevCP(textRef, caret);
        else {
            int i = prevCP(textRef, caret);
            while (i > 0 && std::isalnum((unsigned char)textRef[prevCP(textRef, i)])) i = prevCP(textRef, i);
            c = i;
        }
        if (withSel) { if (selAnchor < 0) selAnchor = oldCaret; }
        else { clearSelection(); }
        caret = c;
    };
    auto moveRight = [&](bool word, bool withSel) {
        int oldCaret = caret;
        int c;
        if (!word) c = nextCP(textRef, caret);
        else {
            int n = (int)textRef.size();
            int i = nextCP(textRef, caret);
            while (i < n && std::isalnum((unsigned char)textRef[i])) i = nextCP(textRef, i);
            c = i;
        }
        if (withSel) { if (selAnchor < 0) selAnchor = oldCaret; }
        else { clearSelection(); }
        caret = c;
    };

    switch (e.type) {
        case SDL_MOUSEBUTTONDOWN: {
            if (e.button.button == SDL_BUTTON_LEFT) {
                if (isInside(e.button.x, e.button.y)) {
                    if (!focused) { focused = true; SDL_StartTextInput(); }
                    int oldCaret = caret;
                    caret = caretByteFromX(e.button.x);
                    Uint32 now = SDL_GetTicks();
                    bool near = std::abs(e.button.x - lastClickX) <= 4 && std::abs(e.button.y - lastClickY) <= 4;
                    if (now - lastClickTicks <= 350 && near) clickCount++;
                    else clickCount = 1;
                    lastClickTicks = now; lastClickX = e.button.x; lastClickY = e.button.y;

                    if (clickCount == 2) {
                        int L = caret, R = caret;
                        auto isCont = [](unsigned char c){ return (c & 0xC0) == 0x80; };
                        auto prevCP = [&](const std::string& s, int i){ if(i<=0) return 0; i--; while(i>0&&isCont((unsigned char)s[i])) i--; return i; };
                        auto nextCP = [&](const std::string& s, int i){ int n=(int)s.size(); if(i>=n) return n; i++; while(i<n&&isCont((unsigned char)s[i])) i++; return i; };

                        while (L > 0) {
                            int p = prevCP(textRef, L);
                            unsigned char ch = (unsigned char)textRef[p];
                            if (!std::isalnum(ch) && ch != '_') break;
                            L = p;
                        }
                        while (R < (int)textRef.size()) {
                            unsigned char ch = (unsigned char)textRef[R];
                            if (!std::isalnum(ch) && ch != '_') break;
                            R = nextCP(textRef, R);
                        }
                        selAnchor = L;
                        caret     = R;
                        selectingDrag = false;
                        ensureCaretVisibleLocal();
                        updateImeRect();
                        return;
                    }

                    if (clickCount == 3) {
                        selAnchor = 0;
                        caret     = (int)textRef.size();
                        selectingDrag = false;
                        ensureCaretVisibleLocal();
                        updateImeRect();
                        return;
                    }
                    const bool shiftHeld = (SDL_GetModState() & KMOD_SHIFT) != 0;
                    if (shiftHeld) { if (selAnchor < 0) selAnchor = oldCaret; }
                    else { clearSelection(); selAnchor = caret; }
                    selectingDrag = true;
                    SDL_CaptureMouse(SDL_TRUE);
                    ensureCaretVisibleLocal();
                    updateImeRect();
                    cursorVisible = true;
                    lastBlinkTicks = SDL_GetTicks();
                    return;
                } else {
                    if (focused) { focused = false; SDL_StopTextInput(); preedit.clear(); clearSelection(); }
                }
            }
        } break;

        case SDL_MOUSEMOTION: {
            if (selectingDrag) {
                Uint32 btns = SDL_GetMouseState(nullptr, nullptr);
                if ((btns & SDL_BUTTON(SDL_BUTTON_LEFT)) == 0) {
                    selectingDrag = false;
                    SDL_CaptureMouse(SDL_FALSE);
                    if (!hasSelection()) clearSelection();
                    return;
                }
            }
            if (focused && selectingDrag) {
                int pad = 8;
                int leftEdge  = innerR.x + pad;
                int rightEdge = innerR.x + innerR.w - pad;

                const int scrollStep = 12;
                if (e.motion.x < leftEdge)  scrollX = std::max(0, scrollX - scrollStep);
                if (e.motion.x > rightEdge) scrollX = scrollX + scrollStep;

                caret = caretByteFromX(e.motion.x);

                ensureCaretVisibleLocal();
                updateImeRect();
                lastInputTicks = SDL_GetTicks();
                cursorVisible  = true;
                lastBlinkTicks = lastInputTicks;
                return;
            }
        } break;

        case SDL_MOUSEBUTTONUP: {
            if (e.button.button == SDL_BUTTON_LEFT) {
                selectingDrag = false;
                SDL_CaptureMouse(SDL_FALSE);
                if (!hasSelection()) clearSelection();
                return;
            }
        } break;

        case SDL_KEYDOWN: {
            if (!focused) break;
            SDL_Keycode key = e.key.keysym.sym;
            bool ctrl  = (e.key.keysym.mod & KMOD_CTRL) != 0;
            bool gui   = (e.key.keysym.mod & KMOD_GUI)  != 0;
            bool shift = (e.key.keysym.mod & KMOD_SHIFT)!= 0;

            if ((ctrl && e.key.keysym.sym == SDLK_z && !shift) ||
                (gui  && e.key.keysym.sym == SDLK_z && !shift)) { undo(); postEditAdjust(); return; }

            if ((ctrl && (e.key.keysym.sym == SDLK_y || (shift && e.key.keysym.sym == SDLK_z))) ||
                (gui  && (shift && e.key.keysym.sym == SDLK_z))) { redo(); postEditAdjust(); return; }

            if (ctrl && (key == SDLK_a)) {
                selAnchor = 0;
                caret = (int)textRef.size();
                ensureCaretVisibleLocal();
                updateImeRect();
                return;
            }
            if (ctrl && (key == SDLK_c)) {
                if (hasSelection()) {
                    auto [a, b] = selRange();
                    if (b < a) std::swap(a, b);
                    SDL_SetClipboardText(textRef.substr(a, b - a).c_str());
                }
                return;
            }
            if (ctrl && e.key.keysym.sym == SDLK_x) {
                if (hasSelection()) {
                    auto [a,b] = selRange();
                    SDL_SetClipboardText(linkedText.get().substr(a, b-a).c_str());
                    replaceRange((size_t)a, (size_t)b, "", EditRec::Cut, false);
                    postEditAdjust();
                }
                return;
            }
            if (ctrl && e.key.keysym.sym == SDLK_v) {
                if (SDL_HasClipboardText()) {
                    char* txt = SDL_GetClipboardText();
                    if (txt) {
                        std::string paste = txt; SDL_free(txt);

                        const auto& cur = linkedText.get();
                        size_t a = hasSelection() ? (size_t)selRange().first  : (size_t)caret;
                        size_t b = hasSelection() ? (size_t)selRange().second : (size_t)caret;

                        size_t maxLen = (maxLength > 0) ? (size_t)maxLength : SIZE_MAX;
                        size_t room   = (cur.size() - (b - a) < maxLen) ? (maxLen - (cur.size() - (b - a))) : 0;

                        if (room > 0) {
                            if (paste.size() > room) paste.resize(room);
                            replaceRange(a, b, paste, EditRec::Paste, false);
                            postEditAdjust();
                        }
                    }
                }
                return;
            }

            if (key == SDLK_LEFT || key == SDLK_RIGHT) {
                if (shift) {
                    if (key == SDLK_LEFT)  moveLeft(ctrl, true);
                    else                   moveRight(ctrl, true);
                } else {
                    if (hasSelection()) {
                        auto [a, b] = selRange();
                        caret = (key == SDLK_LEFT) ? a : b;
                        clearSelection();
                    } else {
                        if (key == SDLK_LEFT)  moveLeft(ctrl, false);
                        else                   moveRight(ctrl, false);
                    }
                }
                ensureCaretVisibleLocal();
                updateImeRect();
                cursorVisible = true;
                lastBlinkTicks = SDL_GetTicks();
                return;
            }
            if (key == SDLK_HOME) {
                int before = caret;
                caret = 0;
                if (shift) { if (selAnchor < 0) selAnchor = before; }
                else { clearSelection(); selAnchor = caret; }
                ensureCaretVisibleLocal(); updateImeRect(); cursorVisible = true; lastBlinkTicks = SDL_GetTicks(); return;
            }
            if (key == SDLK_END) {
                int before = caret;
                caret = (int)textRef.size();
                if (shift) { if (selAnchor < 0) selAnchor = before; }
                else { clearSelection(); selAnchor = caret; }
                ensureCaretVisibleLocal(); updateImeRect(); cursorVisible = true; lastBlinkTicks = SDL_GetTicks(); return;
            }
            if (e.key.keysym.sym == SDLK_BACKSPACE) {
                auto& s = linkedText.get();
                if (hasSelection()) {
                    auto [a,b] = selRange();
                    replaceRange((size_t)a, (size_t)b, "", EditRec::Backspace, false);
                } else if (ctrl) {
                    auto prevCP = [&](int i){ if (i<=0) return 0; i--; while(i>0 && ((unsigned char)s[i]&0xC0)==0x80) i--; return i; };
                    int L = caret;
                    while (L > 0 && !std::isalnum((unsigned char)s[L-1]) && s[L-1] != '_') L = prevCP(L);
                    while (L > 0 && (std::isalnum((unsigned char)s[L-1]) || s[L-1] == '_')) L = prevCP(L);
                    if (L < caret) replaceRange((size_t)L, (size_t)caret, "", EditRec::Backspace, true);
                } else if (caret > 0) {
                    auto prevCP = [&](int i){ if (i<=0) return 0; i--; while(i>0 && ((unsigned char)s[i]&0xC0)==0x80) i--; return i; };
                    int p = prevCP(caret);
                    replaceRange((size_t)p, (size_t)caret, "", EditRec::Backspace, true);
                }
                postEditAdjust();
                return;
            }

            if (e.key.keysym.sym == SDLK_DELETE) {
                auto& s = linkedText.get();
                if (hasSelection()) {
                    auto [a,b] = selRange();
                    replaceRange((size_t)a, (size_t)b, "", EditRec::DeleteKey, false);
                } else if (ctrl) {
                    auto nextCP = [&](int i){ int n=(int)s.size(); if (i>=n) return n; i++; while(i<n && ((unsigned char)s[i]&0xC0)==0x80) i++; return i; };
                    int R = caret, n = (int)s.size();
                    while (R < n && !std::isalnum((unsigned char)s[R]) && s[R] != '_') R = nextCP(R);
                    while (R < n && (std::isalnum((unsigned char)s[R]) || s[R] == '_')) R = nextCP(R);
                    if (R > caret) replaceRange((size_t)caret, (size_t)R, "", EditRec::DeleteKey, true);
                } else if (caret < (int)s.size()) {
                    auto nextCP = [&](int i){ int n=(int)s.size(); if (i>=n) return n; i++; while(i<n && ((unsigned char)s[i]&0xC0)==0x80) i++; return i; };
                    int n2 = nextCP(caret);
                    replaceRange((size_t)caret, (size_t)n2, "", EditRec::DeleteKey, true);
                }
                postEditAdjust();
                return;
            }
        } break;
        case SDL_TEXTINPUT: {
            if (!focused) break;

            std::string in = e.text.text;
            bool valid = true;
            switch (inputType) {
                case InputType::NUMERIC: valid = std::all_of(in.begin(), in.end(), ::isdigit); break;
                case InputType::EMAIL:   valid = std::all_of(in.begin(), in.end(), [](char c){ return std::isalnum((unsigned char)c) || c=='@'||c=='.'||c=='-'||c=='_';}); break;
                default: break;
            }
            if (valid && !in.empty()) {
                const auto& cur = linkedText.get();
                size_t a = hasSelection() ? (size_t)selRange().first  : (size_t)caret;
                size_t b = hasSelection() ? (size_t)selRange().second : (size_t)caret;

                size_t maxLen = (maxLength > 0) ? (size_t)maxLength : SIZE_MAX;
                size_t room   = (cur.size() - (b - a) < maxLen) ? (maxLen - (cur.size() - (b - a))) : 0;
                if (room > 0) {
                    if (in.size() > room) in.resize(room);
                    replaceRange(a, b, in, EditRec::Typing, true);
                    postEditAdjust();
                }
            }
            return;
        }

        case SDL_TEXTEDITING: {
            if (!focused) break;
            std::string newText = e.edit.text;
            bool changed = (newText != preedit) || (preeditCursor != e.edit.start);
            preedit.assign(newText);
            preeditCursor = e.edit.start;
            updateImeRect();
            if (changed) {
                lastInputTicks = SDL_GetTicks();
                cursorVisible  = true;
                lastBlinkTicks = lastInputTicks;
            }
            return;
        } break;

#ifdef SDL_TEXTEDITING_EXT
        case SDL_TEXTEDITING_EXT: {
            if (!focused) break;
            std::string newText = e.edit.text ? e.edit.text : "";
            bool changed = (newText != preedit) || (preeditCursor != e.edit.start);
            preedit.assign(newText);
            preeditCursor = e.edit.start;
            updateImeRect();
            if (changed) {
                lastInputTicks = SDL_GetTicks();
                cursorVisible  = true;
                lastBlinkTicks = lastInputTicks;
            }
            return;
        } break;
#endif

        case SDL_WINDOWEVENT: {
            if (e.window.event == SDL_WINDOWEVENT_FOCUS_LOST) {
                if (focused) { focused = false; SDL_StopTextInput(); preedit.clear(); clearSelection(); }
            }
        } break;
    }
}




void UITextField::update(float) {
    int mx, my; SDL_GetMouseState(&mx, &my);
    Uint32 btns = SDL_GetMouseState(nullptr, nullptr);
    if (selectingDrag && (btns & SDL_BUTTON(SDL_BUTTON_LEFT)) == 0) {
        selectingDrag = false;
        SDL_CaptureMouse(SDL_FALSE);
        if (!hasSelection()) clearSelection();
    }

    hovered = (mx >= bounds.x && mx < bounds.x + bounds.w &&
               my >= bounds.y && my < bounds.y + bounds.h);

    if (focused && selectingDrag) {
        TTF_Font* activeFont = font ? font : UIConfig::getDefaultFont();
        if (activeFont) {
            SDL_Rect innerR = (borderPx <= 0)
                ? bounds
                : SDL_Rect{ bounds.x + borderPx, bounds.y + borderPx,
                            bounds.w - 2*borderPx, bounds.h - 2*borderPx };

            const int pad = 8;
            const int leftEdge  = innerR.x + pad;
            const int rightEdge = innerR.x + innerR.w - pad;

            int distL = leftEdge  - mx;
            int distR = mx        - rightEdge;

            int step = 0;
            if (distL > 0) step = (distL / 6) + 6;
            else if (distR > 0) step = (distR / 6) + 6;

            if (distL > 0)      scrollX = std::max(0, scrollX - step);
            else if (distR > 0) scrollX = scrollX + step;

            rebuildGlyphX(activeFont);
            int innerW    = std::max(0, innerR.w - 2*pad);
            int contentW  = glyphX.empty() ? 0 : glyphX.back();
            int maxScroll = std::max(0, contentW - innerW);
            if (scrollX > maxScroll) scrollX = maxScroll;

            auto isContB = [](unsigned char c){ return (c & 0xC0) == 0x80; };
            auto nextCPi = [&](const std::string& s, int i){
                int n = (int)s.size();
                if (i < 0) return 0;
                if (i >= n) return n;
                i++;
                while (i < n && isContB((unsigned char)s[i])) i++;
                return i;
            };

            const std::string& s = linkedText.get();
            int xLocal = mx - (innerR.x + pad) + scrollX;
            int n = (int)s.size();
            int i = 0, lastGood = 0;

            while (i <= n) {
                int wPref = prefixWidth(i);
                if (wPref > xLocal) break;
                lastGood = i;
                if (i == n) break;
                i = nextCPi(s, i);
            }

            if (lastGood != caret) {
                caret = lastGood;
                lastInputTicks = SDL_GetTicks();
                cursorVisible  = true;
                lastBlinkTicks = lastInputTicks;
            }
        }
    }

    if (!enabled) { cursorVisible = false; return; }
    if (!focused) { cursorVisible = false; return; }

    const Uint32 now          = SDL_GetTicks();
    const Uint32 typingHoldMs = 300;
    const Uint32 blinkMs      = 530;

    if (now - lastInputTicks < typingHoldMs) {
        cursorVisible  = true;
        lastBlinkTicks = now;
    } else {
        if (now - lastBlinkTicks >= blinkMs) {
            cursorVisible  = !cursorVisible;
            lastBlinkTicks = now;
        }
    }
}


void UITextField::render(SDL_Renderer* renderer) {
    TTF_Font* activeFont = font ? font : UIConfig::getDefaultFont();
    if (!activeFont) return;

    const UITheme& th = getTheme();
    const UIStyle& ds = getStyle();
    const auto st = MakeTextFieldStyle(th, ds);

    const int effRadius   = (cornerRadius > 0 ? cornerRadius : st.radius);
    const int effBorderPx = (borderPx     > 0 ? borderPx     : st.borderPx);

    const SDL_Color borderNow = focused ? st.borderFocus : (hovered ? st.borderHover : st.border);

    if (focused) {
        UIHelpers::StrokeRoundedRectOutside(renderer, bounds, effRadius, effBorderPx + 1, th.focusRing, st.bg);
    }

    SDL_Rect dst = bounds;
    rebuildGlyphX(activeFont);

    if (effBorderPx > 0) {
        UIHelpers::FillRoundedRect(renderer, dst.x, dst.y, dst.w, dst.h, effRadius, borderNow);
        SDL_Rect inner = { dst.x + effBorderPx, dst.y + effBorderPx,
                           dst.w - 2*effBorderPx, dst.h - 2*effBorderPx };
        UIHelpers::FillRoundedRect(renderer, inner.x, inner.y, inner.w, inner.h,
                                   std::max(0, effRadius - effBorderPx), st.bg);
        dst = inner;
    } else {
        UIHelpers::FillRoundedRect(renderer, dst.x, dst.y, dst.w, dst.h, effRadius, st.bg);
    }

    std::string toRender = (inputType == InputType::PASSWORD)
        ? std::string(linkedText.get().size(), '*')
        : linkedText.get();

    SDL_Color drawCol = st.fg;
    if (toRender.empty() && !focused && !placeholder.empty()) {
        toRender = placeholder;
        drawCol  = st.placeholder;
    }

    int cursorX = dst.x + 8;
    int cursorH = TTF_FontHeight(activeFont);
    int cursorY = dst.y + (dst.h - cursorH) / 2;

    SDL_Rect clip = { dst.x + 4, dst.y + 2, dst.w - 8, dst.h - 4 };
    SDL_RenderSetClipRect(renderer, &clip);

    if (!toRender.empty()) {
        auto textSurface = UIHelpers::MakeSurface(
            TTF_RenderUTF8_Blended(activeFont, toRender.c_str(), drawCol)
        );

        if (textSurface) {
            auto textTexture = UIHelpers::MakeTexture(
                SDL_CreateTextureFromSurface(renderer, textSurface.get())
            );

            SDL_Rect textRect = {
                dst.x + 8 - scrollX,
                dst.y + (dst.h - textSurface->h) / 2,
                textSurface->w,
                textSurface->h
            };

            if (focused && hasSelection()) {
                auto [a, b] = selRange();
                if (b > a) {
                    const std::string& full = linkedText.get();
                    std::string left = (inputType == InputType::PASSWORD) ? std::string(a, '*') : full.substr(0, a);
                    std::string mid  = (inputType == InputType::PASSWORD) ? std::string(b - a, '*') : full.substr(a, b - a);
                    int leftW = prefixWidth(a);
                    int midW  = prefixWidth(b) - prefixWidth(a);

                    SDL_SetRenderDrawColor(renderer, st.selectionBg.r, st.selectionBg.g, st.selectionBg.b, st.selectionBg.a);
                    SDL_Rect selRect{ textRect.x + leftW, textRect.y, midW, textSurface->h };
                    SDL_RenderFillRect(renderer, &selRect);
                }
            }

            SDL_RenderCopy(renderer, textTexture.get(), nullptr, &textRect);

            cursorH = textSurface->h;
            cursorY = textRect.y;

        }
    }

    if (focused && !preedit.empty()) {
        std::string preToDraw = (inputType == InputType::PASSWORD) ? std::string(preedit.size(), '*') : preedit;

        const std::string& full = linkedText.get();
        std::string prefixMeasure = (inputType == InputType::PASSWORD)
            ? std::string(caret, '*')
            : full.substr(0, std::min<int>(caret, (int)full.size()));

        int prefixW = textWidth(activeFont, prefixMeasure);

        SDL_Color preCol = st.fg;

        auto preSurf = UIHelpers::MakeSurface(
            TTF_RenderUTF8_Blended(activeFont, preToDraw.c_str(), preCol)
        );

        if (preSurf) {
            auto preTex = UIHelpers::MakeTexture(
                SDL_CreateTextureFromSurface(renderer, preSurf.get())
            );

            SDL_Rect preRect = {
                dst.x + 8 + prefixW - scrollX,
                dst.y + (dst.h - preSurf->h) / 2,
                preSurf->w,
                preSurf->h
            };

            SDL_SetRenderDrawColor(renderer, preCol.r, preCol.g, preCol.b, preCol.a);
            SDL_Rect underline = { preRect.x, preRect.y + preRect.h - 1, preRect.w, 1 };
            SDL_RenderFillRect(renderer, &underline);

            SDL_RenderCopy(renderer, preTex.get(), nullptr, &preRect);

            auto isContB = [](unsigned char c){ return (c & 0xC0) == 0x80; };
            int preByte = 0, cpLeft = std::max(0, preeditCursor);
            while (preByte < (int)preedit.size() && cpLeft-- > 0) {
                preByte++;
                while (preByte < (int)preedit.size() && isContB((unsigned char)preedit[preByte])) preByte++;
            }
            std::string preCaretSub = (inputType == InputType::PASSWORD)
                ? std::string((int)std::count_if(preedit.begin(), preedit.begin() + preByte,
                    [&](unsigned char ch){ return !isContB(ch); }), '*')
                : preedit.substr(0, preByte);

            int preCaretW = 0, preCaretH = 0;
            if (!preCaretSub.empty()) TTF_SizeUTF8(activeFont, preCaretSub.c_str(), &preCaretW, &preCaretH);

            if (cursorVisible && !hasSelection()) {
                SDL_SetRenderDrawColor(renderer, st.caret.r, st.caret.g, st.caret.b, st.caret.a);
                SDL_Rect preCaret = { preRect.x + preCaretW, preRect.y, 1, preRect.h };
                SDL_RenderFillRect(renderer, &preCaret);
            }

        }
    }

    {
        const std::string& full = linkedText.get();
        std::string prefixMeasure = (inputType == InputType::PASSWORD)
            ? std::string(caret, '*')
            : full.substr(0, clampi(caret, 0, (int)full.size()));

        int wPrefix = prefixWidth(std::min<size_t>(caret, glyphX.size() ? glyphX.size()-1 : 0));
        cursorX = dst.x + 8 + wPrefix - scrollX;
    }

    if (focused && cursorVisible && preedit.empty() && !hasSelection()) {
        SDL_SetRenderDrawColor(renderer, st.caret.r, st.caret.g, st.caret.b, st.caret.a);
        SDL_Rect cursorRect = { cursorX, cursorY, 1, cursorH };
        SDL_RenderFillRect(renderer, &cursorRect);
    }

    SDL_RenderSetClipRect(renderer, nullptr);
}

void UITextField::rebuildGlyphX(TTF_Font* f) {
    const std::string& s = linkedText.get();

    if (!f) {
        glyphX.clear();
        cacheFont = nullptr;
        measuredTextCache.clear();
        return;
    }

    if (measuredTextCache == s && !glyphX.empty() && cacheFont == f) {
        return;
    }

    cacheFont = f;
    measuredTextCache = s;

    glyphX.assign(s.size() + 1, 0);
    int w = 0, h = 0;
    for (size_t i = 1; i <= s.size(); ++i) {
        std::string sub = s.substr(0, i);
        TTF_SizeUTF8(f, sub.c_str(), &w, &h);
        glyphX[i] = w;
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
    if (e.type == SDL_USEREVENT) {
        if (e.user.code == 0xF001) { focused = true; return; }
        if (e.user.code == 0xF002) { focused = false; expanded = false; return; }
    }
    if (!enabled) return;

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        SDL_Point p{ e.button.x, e.button.y };

        if (SDL_PointInRect(&p, &bounds)) {
            expanded = !expanded;
            if (expanded) {
                const int hi = (int)options.size() - 1;
                if (hi >= 0) hoveredIndex = std::clamp((int)selectedIndex.get(), 0, hi);
            }
            return;
        }

        if (expanded) {
            const int ih = bounds.h;
            for (int i = 0; i < (int)options.size(); ++i) {
                SDL_Rect itemRect{ bounds.x, bounds.y + (i + 1) * ih, bounds.w, ih };
                if (SDL_PointInRect(&p, &itemRect)) {
                    selectedIndex.get() = i;
                    if (onSelect) onSelect(i);
                    expanded = false;
                    return;
                }
            }
        }
    }

    if (!focused) return;

    if (e.type == SDL_KEYDOWN) {
        switch (e.key.keysym.sym) {
            case SDLK_ESCAPE:
                if (expanded) { expanded = false; return; }
                break;

            case SDLK_SPACE:
            case SDLK_RETURN:
                if (expanded) {
                    if (!options.empty() && hoveredIndex >= 0 && hoveredIndex < (int)options.size()) {
                        selectedIndex.get() = hoveredIndex;
                        if (onSelect) onSelect(hoveredIndex);
                    }
                    expanded = false;
                } else {
                    expanded = true;
                    const int hi = (int)options.size() - 1;
                    if (hi >= 0) hoveredIndex = std::clamp((int)selectedIndex.get(), 0, hi);
                }
                return;

            case SDLK_UP:
                if (!expanded) {
                    if (!options.empty()) {
                        expanded = true;
                        const int hi = (int)options.size() - 1;
                        hoveredIndex = std::clamp((int)selectedIndex.get(), 0, hi);
                    }
                    return;
                }
                if (!options.empty()) {
                    hoveredIndex = std::max(0, hoveredIndex - 1);
                }
                return;

            case SDLK_DOWN:
                if (!expanded) {
                    if (!options.empty()) {
                        expanded = true;
                        const int hi = (int)options.size() - 1;
                        hoveredIndex = std::clamp((int)selectedIndex.get(), 0, hi);
                    }
                    return;
                }
                if (!options.empty()) {
                    hoveredIndex = std::min((int)options.size() - 1, hoveredIndex + 1);
                }
                return;
        }
    }
}

void UIComboBox::update(float) {
    int mx, my; SDL_GetMouseState(&mx, &my);
    SDL_Point p{ mx, my };
    hovered = SDL_PointInRect(&p, &bounds);

    if (!expanded || options.empty()) return;

    const int ih = bounds.h;
    SDL_Rect listRect{ bounds.x, bounds.y + bounds.h, bounds.w, ih * (int)options.size() };

    if (SDL_PointInRect(&p, &listRect)) {
        int relY = my - listRect.y;
        int idx = relY / ih;
        idx = std::clamp(idx, 0, (int)options.size() - 1);
        hoveredIndex = idx;
    }
}

void UIComboBox::render(SDL_Renderer* renderer) {
    const UITheme& th = getTheme();
    const UIStyle& ds = getStyle();
    const auto st = MakeComboBoxStyle(th, ds);
    TTF_Font* activeFont = font ? font : (th.font ? th.font : UIConfig::getDefaultFont());
    if (!activeFont) return;

    const int effRadius   = (cornerRadius > 0 ? cornerRadius : st.radius);
    const int effBorderPx = st.borderPx;

    SDL_Rect field = bounds;
    if (focusable && focused) {
        UIHelpers::StrokeRoundedRectOutside(renderer, field, effRadius, effBorderPx + 1, st.borderFocus, st.fieldBg);
    }

    SDL_Color borderNow = focused ? st.borderFocus : (hovered ? st.borderHover : st.border);
    if (effBorderPx > 0) {
        UIHelpers::FillRoundedRect(renderer, field.x, field.y, field.w, field.h, effRadius, borderNow);
        SDL_Rect inner = { field.x + effBorderPx, field.y + effBorderPx, field.w - 2*effBorderPx, field.h - 2*effBorderPx };
        UIHelpers::FillRoundedRect(renderer, inner.x, inner.y, inner.w, inner.h, std::max(0, effRadius - effBorderPx), st.fieldBg);
        field = inner;
    } else {
        UIHelpers::FillRoundedRect(renderer, field.x, field.y, field.w, field.h, effRadius, st.fieldBg);
    }

    int sel = selectedIndex.get();
    std::string display = (sel >= 0 && sel < (int)options.size()) ? options[sel] : placeholder;
    SDL_Color textCol   = (sel >= 0) ? st.fieldFg : st.placeholder;
    if (customTextColor) textCol = *customTextColor;

    auto surface = UIHelpers::MakeSurface(
        TTF_RenderUTF8_Blended(activeFont, display.c_str(), textCol)
    );

    if (surface) {
        auto texture = UIHelpers::MakeTexture(
            SDL_CreateTextureFromSurface(renderer, surface.get())
        );

        SDL_Rect tr = { field.x + st.padX, field.y + (field.h - surface->h)/2, surface->w, surface->h };
        SDL_RenderCopy(renderer, texture.get(), nullptr, &tr);
    }

    const int caretW = 12;
    const int caretH = 7;
    int cx = field.x + field.w - st.padX;
    int cy = field.y + field.h/2;
    SDL_Color caretCol = st.caret;
    if (!enabled) caretCol.a = 160;
    float thick = std::max(1.5f, float(st.borderPx) + 0.5f);
    UIHelpers::DrawChevronDown(renderer, cx, cy, caretW, caretH, thick, caretCol);

    if (expanded && !options.empty()) {
        const int ih = bounds.h;
        SDL_Rect menu = { bounds.x, bounds.y + bounds.h, bounds.w, ih * (int)options.size() };
        SDL_Color mb = st.menuBg;
        SDL_Color mborder = st.menuBorder;

        if (effBorderPx > 0) {
            UIHelpers::FillRoundedRect(renderer, menu.x, menu.y, menu.w, menu.h, effRadius, mborder);
            SDL_Rect inner = { menu.x + effBorderPx, menu.y + effBorderPx, menu.w - 2*effBorderPx, menu.h - 2*effBorderPx };
            UIHelpers::FillRoundedRect(renderer, inner.x, inner.y, inner.w, inner.h, std::max(0, effRadius - effBorderPx), mb);
            menu = inner;
        } else {
            UIHelpers::FillRoundedRect(renderer, menu.x, menu.y, menu.w, menu.h, effRadius, mb);
        }

        int y = menu.y;
        for (int i = 0; i < (int)options.size(); ++i) {
            SDL_Rect row{ menu.x + 4, y, menu.w - 8, ih };
            bool isSel = (i == sel);
            bool isHot = (i == hoveredIndex);

            if (isSel) {
                UIHelpers::FillRoundedRect(renderer, row.x, row.y + 2, row.w, row.h - 4, 6, st.itemSelectedBg);
            } else if (isHot) {
                UIHelpers::FillRoundedRect(renderer, row.x, row.y + 2, row.w, row.h - 4, 6, st.itemHoverBg);
            }

            SDL_Color ic = isSel ? st.itemSelectedFg : st.itemFg;

            auto itemSurface = UIHelpers::MakeSurface(
                TTF_RenderUTF8_Blended(activeFont, options[i].c_str(), ic)
            );

            if (itemSurface) {
                auto itemTexture = UIHelpers::MakeTexture(
                    SDL_CreateTextureFromSurface(renderer, itemSurface.get())
                );

                SDL_Rect ir = { row.x + 8, row.y + (row.h - itemSurface->h)/2, itemSurface->w, itemSurface->h };
                SDL_RenderCopy(renderer, itemTexture.get(), nullptr, &ir);
            }

            y += ih;
        }
    }
}


bool UIComboBox::isInside(int x, int y) const {
    SDL_Point p{ x, y };
    if (SDL_PointInRect(&p, &bounds)) return true;
    if (expanded) {
        const int ih = bounds.h;
        SDL_Rect listRect{ bounds.x, bounds.y + bounds.h, bounds.w, ih * (int)options.size() };
        if (SDL_PointInRect(&p, &listRect)) return true;
    }
    return false;
}

UIComboBox* UIComboBox::setTextColor(SDL_Color c) {
    customTextColor = c;
    return this;
}




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
    const UIStyle& ds = getStyle();
    const auto st = MakeSliderStyle(th, ds);

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
            int newValue = value.get() + step;
            if (newValue <= maxValue) {
                value.get() = newValue;
                if (onChange) onChange(value.get());
            }
        } else if (heldButton == HeldButton::DECREMENT && value.get() > minValue) {
            int newValue = value.get() - step;
            if (newValue >= minValue) {
                value.get() = newValue;
                if (onChange) onChange(value.get());
            }
        }
        lastStepTime = now;
    }
}

void UISpinner::render(SDL_Renderer* renderer) {
    const UITheme& th = getTheme();
    const UIStyle& ds = getStyle();
    const auto st = MakeSpinnerStyle(th, ds);
    TTF_Font* activeFont = font ? font : (th.font ? th.font : UIConfig::getDefaultFont());
    if (!activeFont) return;

    const int effRadius   = st.radius;
    const int effBorderPx = st.borderPx;

    SDL_Rect minusRect = { bounds.x, bounds.y, bounds.h, bounds.h };
    SDL_Rect plusRect  = { bounds.x + bounds.w - bounds.h, bounds.y, bounds.h, bounds.h };
    SDL_Rect centerRect{ bounds.x + bounds.h, bounds.y, bounds.w - 2*bounds.h, bounds.h };

    UIHelpers::FillRoundedRect(renderer, bounds.x, bounds.y, bounds.w, bounds.h, effRadius, st.fieldBorder);
    SDL_Rect inner = { bounds.x + effBorderPx, bounds.y + effBorderPx, bounds.w - 2*effBorderPx, bounds.h - 2*effBorderPx };
    UIHelpers::FillRoundedRect(renderer, inner.x, inner.y, inner.w, inner.h, std::max(0, effRadius - effBorderPx), st.fieldBg);

    SDL_SetRenderDrawColor(renderer, st.fieldBorder.r, st.fieldBorder.g, st.fieldBorder.b, st.fieldBorder.a);
    SDL_RenderDrawLine(renderer, centerRect.x, bounds.y, centerRect.x, bounds.y + bounds.h - 1);
    SDL_RenderDrawLine(renderer, centerRect.x + centerRect.w, bounds.y, centerRect.x + centerRect.w, bounds.y + bounds.h - 1);

    if (hoveredMinus) {
        SDL_Color hb = st.btnBgHover;
        SDL_SetRenderDrawColor(renderer, hb.r, hb.g, hb.b, hb.a);
        SDL_RenderFillRect(renderer, &minusRect);
    }
    if (hoveredPlus) {
        SDL_Color hb = st.btnBgHover;
        SDL_SetRenderDrawColor(renderer, hb.r, hb.g, hb.b, hb.a);
        SDL_RenderFillRect(renderer, &plusRect);
    }

    SDL_Color glyph = st.btnGlyph;
    SDL_SetRenderDrawColor(renderer, glyph.r, glyph.g, glyph.b, glyph.a);
    SDL_RenderDrawLine(renderer, minusRect.x + minusRect.w/4, minusRect.y + minusRect.h/2, minusRect.x + 3*minusRect.w/4, minusRect.y + minusRect.h/2);
    SDL_RenderDrawLine(renderer, plusRect.x + plusRect.w/4, plusRect.y + plusRect.h/2, plusRect.x + 3*plusRect.w/4, plusRect.y + plusRect.h/2);
    SDL_RenderDrawLine(renderer, plusRect.x + plusRect.w/2, plusRect.y + plusRect.h/4, plusRect.x + plusRect.w/2, plusRect.y + 3*plusRect.h/4);

    std::ostringstream oss;
    oss << value.get();
    SDL_Color txtCol = st.text;

    auto surface = UIHelpers::MakeSurface(
        TTF_RenderUTF8_Blended(activeFont, oss.str().c_str(), txtCol)
    );

    if (surface) {
        auto texture = UIHelpers::MakeTexture(
            SDL_CreateTextureFromSurface(renderer, surface.get())
        );

        SDL_Rect textRect = {
            centerRect.x + (centerRect.w - surface->w)/2,
            centerRect.y + (centerRect.h - surface->h)/2,
            surface->w,
            surface->h
        };

        SDL_RenderCopy(renderer, texture.get(), nullptr, &textRect);
    }
}


void UITextArea::clearRedo() { redoStack.clear(); }

static inline bool hasSelRange(const size_t a, const size_t b) { return b > a; }

void UITextArea::applyReplaceNoHistory(size_t a, size_t b, std::string_view repl,
                                       size_t newCursor, size_t newSelA, size_t newSelB)
{
    auto& txt = linkedText.get();
    a = std::min(a, txt.size());
    b = std::min(b, txt.size());
    if (b < a) std::swap(a, b);

    txt.replace(a, b - a, repl);
    cursorPos = std::min(newCursor, txt.size());

    if (hasSelRange(newSelA, newSelB)) {
        selStart = newSelA; selEnd = newSelB;
    } else {
        clearSelection();
    }
    selectAnchor = cursorPos;

    preferredXpx = -1; preferredColumn = -1;
    updateCursorPosition(); setIMERectAtCaret();
    lastBlinkTime = SDL_GetTicks(); cursorVisible = true;
}

void UITextArea::pushEdit(EditRec e, bool tryCoalesce)
{
    clearRedo();

    if (tryCoalesce && !undoStack.empty()) {
        EditRec& p = undoStack.back();

        if (e.kind == EditRec::Typing && p.kind == EditRec::Typing &&
            p.pos + p.after.size() == e.pos && p.before.empty() && e.before.empty() &&
            (e.time - p.time) <= coalesceMs)
        {
            p.after += e.after;
            p.cursorAfter = e.cursorAfter;
            p.selAAfter   = e.selAAfter;
            p.selBAfter   = e.selBAfter;
            p.time        = e.time;
            return;
        }

        if (e.kind == EditRec::Backspace && p.kind == EditRec::Backspace &&
            e.pos + e.before.size() == p.pos && e.after.empty() && p.after.empty() &&
            (e.time - p.time) <= coalesceMs)
        {
            p.pos     = e.pos;
            p.before  = e.before + p.before;
            p.cursorAfter = e.cursorAfter;
            p.selAAfter   = e.selAAfter;
            p.selBAfter   = e.selBAfter;
            p.time        = e.time;
            return;
        }
    }

    undoStack.push_back(std::move(e));

    if (undoStack.size() > MAX_UNDO_STACK) {
        undoStack.erase(undoStack.begin());
    }
}

void UITextArea::replaceRange(size_t a, size_t b, std::string_view repl, EditRec::Kind kind,
                              bool tryCoalesce)
{
    auto& txt = linkedText.get();
    a = std::min(a, txt.size());
    b = std::min(b, txt.size());
    if (b < a) std::swap(a, b);

    EditRec e;
    e.pos          = a;
    e.before       = txt.substr(a, b - a);
    e.after        = std::string(repl);
    e.cursorBefore = cursorPos;
    e.selABefore   = hasSelection() ? selRange().first  : cursorPos;
    e.selBBefore   = hasSelection() ? selRange().second : cursorPos;
    e.kind         = kind;
    e.time         = SDL_GetTicks();

    size_t newCursor = a + e.after.size();
    size_t newSelA = newCursor, newSelB = newCursor;

    applyReplaceNoHistory(a, b, repl, newCursor, newSelA, newSelB);

    e.cursorAfter = cursorPos;
    e.selAAfter   = hasSelection() ? selRange().first  : cursorPos;
    e.selBAfter   = hasSelection() ? selRange().second : cursorPos;

    if (historyEnabled) pushEdit(std::move(e), tryCoalesce);
}

void UITextArea::undo()
{
    if (undoStack.empty()) return;
    EditRec e = undoStack.back(); undoStack.pop_back();

    size_t a = e.pos;
    size_t b = e.pos + e.after.size();
    redoStack.push_back(e);
    applyReplaceNoHistory(a, b, e.before, e.cursorBefore, e.selABefore, e.selBBefore);
}

void UITextArea::redo()
{
    if (redoStack.empty()) return;
    EditRec e = redoStack.back(); redoStack.pop_back();

    size_t a = e.pos;
    size_t b = e.pos + e.before.size();
    undoStack.push_back(e);
    applyReplaceNoHistory(a, b, e.after, e.cursorAfter, e.selAAfter, e.selBAfter);
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
    const UITheme& th = getTheme();
    const UIStyle& ds = getStyle();
    if (e.type == SDL_USEREVENT) {
        if (e.user.code == 0xF001) { if (!focused) { focused = true; SDL_StartTextInput();preferredXpx = -1; preferredColumn = -1; } return; }
        if (e.user.code == 0xF002) {
            if (focused) {
                focused = false;
                SDL_StopTextInput();
                clearSelection();
            }
            if (selectingMouse) {
                SDL_CaptureMouse(SDL_FALSE);
                selectingMouse = false;
            }
            imeText.clear(); imeStart = imeLength = 0; imeActive = false;
            preferredXpx = -1; preferredColumn = -1;
            return;
        }
    }

    if (focused && e.type == SDL_TEXTEDITING) {
        imeText = e.edit.text;
        imeStart  = e.edit.start;
        imeLength = e.edit.length;
        imeActive = !imeText.empty();
        setIMERectAtCaret();
        lastBlinkTime = SDL_GetTicks();
        cursorVisible = true;
        return;
    }

    if (e.type == SDL_MOUSEBUTTONDOWN && e.button.button == SDL_BUTTON_LEFT) {
        SDL_Point p{ e.button.x, e.button.y };
        bool wasFocused = focused;
        focused = SDL_PointInRect(&p, &bounds);

        if (focused) {
            size_t idx = indexFromMouse(e.button.x, e.button.y);
            const bool shift = (SDL_GetModState() & KMOD_SHIFT) != 0;

            Uint32 now = SDL_GetTicks();
            bool near = std::abs(e.button.x - lastClickX) <= 4 && std::abs(e.button.y - lastClickY) <= 4;
            if (now - lastClickTicks <= 350 && near) clickCount++;
            else clickCount = 1;
            lastClickTicks = now; lastClickX = e.button.x; lastClickY = e.button.y;

            auto& s = linkedText.get();
            auto isCont = [](unsigned char c){ return (c & 0xC0) == 0x80; };
            auto prevCP = [&](int i){ if(i<=0) return 0; i--; while(i>0 && isCont((unsigned char)s[i])) i--; return i; };
            auto nextCP = [&](int i){ int n=(int)s.size(); if(i>=n) return n; i++; while(i<n && isCont((unsigned char)s[i])) i++; return i; };
            auto isWord = [&](unsigned char ch){ return std::isalnum(ch) || ch=='_'; };

            if (clickCount == 3) {
                selectAll();
                cursorPos = s.size();
                preferredColumn = -1;
                preferredXpx    = -1;
                updateCursorPosition();
                setIMERectAtCaret();
                SDL_StartTextInput();
                lastBlinkTime = SDL_GetTicks(); cursorVisible = true;
                selectingMouse = false;
                return;
            }

            if (clickCount == 2) {
                int L = (int)idx, R = (int)idx, n=(int)s.size();
                while (L > 0) {
                    int pB = prevCP(L);
                    unsigned char ch = (unsigned char)s[pB];
                    if (!isWord(ch)) break;
                    L = pB;
                }
                while (R < n) {
                    unsigned char ch = (unsigned char)s[R];
                    if (!isWord(ch)) break;
                    R = nextCP(R);
                }
                setSelection((size_t)L, (size_t)R);
                cursorPos = (size_t)R;
                preferredColumn = -1;
                preferredXpx    = -1;
                updateCursorPosition(); setIMERectAtCaret();
                SDL_StartTextInput();
                lastBlinkTime = SDL_GetTicks(); cursorVisible = true;
                selectingMouse = false;
                return;
            }

            if (shift) {
                if (!hasSelection()) selectAnchor = cursorPos;
                cursorPos = idx;
                preferredColumn = -1;
                preferredXpx    = -1;
                setSelection(std::min(selectAnchor, cursorPos), std::max(selectAnchor, cursorPos));
            } else {
                cursorPos = idx;
                preferredColumn = -1;
                preferredXpx    = -1;
                clearSelection();
                selectAnchor = cursorPos;
                selectingMouse = true;
            }
            updateCursorPosition(); setIMERectAtCaret();
            SDL_StartTextInput();
            lastBlinkTime = SDL_GetTicks(); cursorVisible = true;
            selectingMouse = true;
            SDL_CaptureMouse(SDL_TRUE);
        } else if (wasFocused && !focused) {
            SDL_StopTextInput();
            clearSelection();
            if (selectingMouse) {
                SDL_CaptureMouse(SDL_FALSE);
                selectingMouse = false;
            }
            preferredXpx    = -1;
            preferredColumn = -1;
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
        return;
    }

    if (e.type == SDL_MOUSEBUTTONUP && e.button.button == SDL_BUTTON_LEFT) {
        scrollbarDragging = false;
        if (selectingMouse) {
            SDL_CaptureMouse(SDL_FALSE);
            selectingMouse = false;
        }
        return;
    }

    if (e.type == SDL_MOUSEMOTION) {
        if (selectingMouse && !(e.motion.state & SDL_BUTTON_LMASK)) {
            SDL_CaptureMouse(SDL_FALSE);
            selectingMouse = false;
            return;
        }
        if (scrollbarDragging && !(e.motion.state & SDL_BUTTON_LMASK)) {
            scrollbarDragging = false;
        }
        if (scrollbarDragging) {
            int dy = e.motion.y - scrollbarDragStartY;
            const auto st = MakeTextAreaStyle(th, ds);
            const int viewH = std::max(0, bounds.h - 2*st.borderPx - 2*paddingPx);
            const float maxScroll = std::max(0.0f, contentHeight - float(viewH));
            scrollOffsetY = scrollbarThumbStartOffset + dy * (maxScroll) / float(viewH);
            scrollOffsetY = std::clamp(scrollOffsetY, 0.0f, maxScroll);
            if (focused) setIMERectAtCaret();
        }

        if (selectingMouse && focused) {
            const auto st = MakeTextAreaStyle(th, ds);
            const int borderPx = st.borderPx;
            const int innerY0  = bounds.y + borderPx + paddingPx;
            const int innerH   = std::max(0, bounds.h - 2*borderPx - 2*paddingPx);

            int yForHit = std::clamp(e.motion.y, innerY0, innerY0 + innerH - 1);
            size_t idx  = indexFromMouse(e.motion.x, yForHit);

            if (idx != cursorPos) {
                cursorPos = idx;
                setSelection(std::min(selectAnchor, cursorPos), std::max(selectAnchor, cursorPos));
                preferredColumn = -1;
                preferredXpx    = -1;
                updateCursorPosition();
                setIMERectAtCaret();
                lastBlinkTime = SDL_GetTicks();
                cursorVisible = true;
            }
            return;
        }
        const auto st = MakeTextAreaStyle(th, ds);
        const int viewH = std::max(0, bounds.h - 2*st.borderPx - 2*paddingPx);
        if (contentHeight > bounds.h) {
            SDL_Point p{ e.motion.x, e.motion.y };
            SDL_Rect sb = getScrollbarRect();
            float vr = float(viewH) / contentHeight;
            int th = std::max(int(viewH * vr), 20);
            int maxThumb = viewH - th;
            int ty = sb.y + int((scrollOffsetY / std::max(0.0f, contentHeight - float(viewH))) * maxThumb);
            SDL_Rect thumb{ sb.x, ty, sb.w, th };
            scrollbarHovered = SDL_PointInRect(&p, &thumb);
        } else {
            scrollbarHovered = false;
        }
        return;
    }

    if (e.type == SDL_MOUSEWHEEL) {
        int mx, my; SDL_GetMouseState(&mx, &my);
        if (mx >= bounds.x && mx <= bounds.x + bounds.w && my >= bounds.y && my <= bounds.y + bounds.h) {
            int lh = TTF_FontHeight(font ? font : UIConfig::getDefaultFont());
            scrollOffsetY -= e.wheel.y * lh;
            const auto st = MakeTextAreaStyle(th, ds);
            const int viewH = std::max(0, bounds.h - 2*st.borderPx - 2*paddingPx);
            scrollOffsetY = std::clamp(scrollOffsetY, 0.0f, std::max(0.0f, contentHeight - float(viewH)));
            if (focused) setIMERectAtCaret();
        }
        return;
    }

    if (focused && e.type == SDL_TEXTINPUT) {
        imeText.clear(); imeStart = imeLength = 0; imeActive = false;

        std::string in = e.text.text;
        bool valid = true;
        switch (inputType) {
            case InputType::NUMERIC: valid = std::all_of(in.begin(), in.end(), ::isdigit); break;
            case InputType::EMAIL:   valid = std::all_of(in.begin(), in.end(), [](char c){ return std::isalnum(c) || c=='@' || c=='.' || c=='-' || c=='_'; }); break;
            default: break;
        }
        if (valid && !in.empty()) {
            size_t a = hasSelection() ? selRange().first  : cursorPos;
            size_t b = hasSelection() ? selRange().second : cursorPos;
            size_t curLen = linkedText.get().size();
            size_t maxLen = (maxLength > 0) ? (size_t)maxLength : SIZE_MAX;
            size_t room   = (curLen - (b - a) < maxLen) ? (maxLen - (curLen - (b - a))) : 0;
            if (room > 0) {
                if (in.size() > room) in.resize(room);
                replaceRange(a, b, in, EditRec::Typing, true);
            }
        }
        return;
    }

    if (focused && e.type == SDL_KEYDOWN) {
        const bool ctrl  = (e.key.keysym.mod & KMOD_CTRL)  != 0;
        const bool gui   = (e.key.keysym.mod & KMOD_GUI)   != 0;
        const bool shift = (e.key.keysym.mod & KMOD_SHIFT) != 0;

        if ( (ctrl && (e.key.keysym.sym == SDLK_y || (shift && e.key.keysym.sym == SDLK_z))) ||
            (gui  &&  shift && e.key.keysym.sym == SDLK_z) ) {
            redo();
            return;
        }

        if ( ((ctrl && !shift) || (gui && !shift)) && e.key.keysym.sym == SDLK_z ) {
            undo();
            return;
        }
        if (ctrl && e.key.keysym.sym == SDLK_a) {
            selectAll();
            preferredColumn = -1; preferredXpx = -1;
            updateCursorPosition(); setIMERectAtCaret();
            lastBlinkTime = SDL_GetTicks(); cursorVisible = true;
            return;
        }
        if (ctrl && e.key.keysym.sym == SDLK_c) {
            if (hasSelection()) { auto [a,b] = selRange(); SDL_SetClipboardText(linkedText.get().substr(a,b-a).c_str()); }
            return;
        }
        if (ctrl && e.key.keysym.sym == SDLK_x) {
            if (hasSelection()) { auto [a,b] = selRange(); SDL_SetClipboardText(linkedText.get().substr(a,b-a).c_str()); replaceRange(a,b,"", EditRec::Cut, false); }
            return;
        }
        if (ctrl && e.key.keysym.sym == SDLK_v) {
            char* txt = SDL_GetClipboardText();
            if (txt) {
                std::string paste = txt; SDL_free(txt);
                size_t a = hasSelection() ? selRange().first  : cursorPos;
                size_t b = hasSelection() ? selRange().second : cursorPos;
                size_t curLen = linkedText.get().size();
                size_t maxLen = (maxLength > 0) ? (size_t)maxLength : SIZE_MAX;
                size_t room   = (curLen - (b - a) < maxLen) ? (maxLen - (curLen - (b - a))) : 0;
                if (room > 0) {
                    if (paste.size() > room) paste.resize(room);
                    replaceRange(a, b, paste, EditRec::Paste, false);
                }
            }
            return;
        }

        if (e.key.keysym.sym == SDLK_RETURN || e.key.keysym.sym == SDLK_KP_ENTER) {
            size_t a = hasSelection() ? selRange().first  : cursorPos;
            size_t b = hasSelection() ? selRange().second : cursorPos;
            size_t curLen = linkedText.get().size();
            size_t maxLen = (maxLength > 0) ? (size_t)maxLength : SIZE_MAX;
            if (curLen - (b - a) + 1 <= maxLen) { replaceRange(a, b, "\n", EditRec::Typing, true); }
            return;
        }

        if (e.key.keysym.sym == SDLK_BACKSPACE) {
            if (hasSelection()) { auto [a,b] = selRange(); replaceRange(a, b, "", EditRec::Backspace, false); }
            else if (cursorPos > 0) { replaceRange(cursorPos - 1, cursorPos, "", EditRec::Backspace, true); }
            return;
        }

        if (e.key.keysym.sym == SDLK_DELETE) {
            if (hasSelection()) { auto [a,b] = selRange(); replaceRange(a, b, "", EditRec::DeleteKey, false); }
            else if (cursorPos < linkedText.get().size()) { replaceRange(cursorPos, cursorPos + 1, "", EditRec::DeleteKey, true); }
            return;
        }

        if (e.key.keysym.sym == SDLK_LEFT || e.key.keysym.sym == SDLK_RIGHT) {
            if (shift) {
                size_t newPos = cursorPos;
                if (e.key.keysym.sym == SDLK_LEFT)  { if (newPos > 0) newPos--; }
                else                                { newPos = std::min(newPos + 1, linkedText.get().size()); }

                if (!hasSelection()) selectAnchor = cursorPos;
                cursorPos = newPos;
                setSelection(std::min(selectAnchor, cursorPos), std::max(selectAnchor, cursorPos));
            } else {
                if (hasSelection()) {
                    auto [a, b] = selRange();
                    cursorPos = (e.key.keysym.sym == SDLK_LEFT) ? a : b;
                    clearSelection();
                } else {
                    size_t newPos = cursorPos;
                    if (e.key.keysym.sym == SDLK_LEFT)  { if (newPos > 0) newPos--; }
                    else                                { newPos = std::min(newPos + 1, linkedText.get().size()); }
                    cursorPos = newPos;
                }
                selectAnchor = cursorPos;
            }

            preferredColumn = -1; preferredXpx = -1;
            lastBlinkTime = SDL_GetTicks(); cursorVisible = true;
            updateCursorPosition(); setIMERectAtCaret();
            return;
        }

        if (e.key.keysym.sym == SDLK_UP || e.key.keysym.sym == SDLK_DOWN) {
            const bool goDown = (e.key.keysym.sym == SDLK_DOWN);
            const bool shiftHeld = shift;

            const std::string& full = linkedText.get();
            const size_t N = full.size();
            size_t i = std::min(cursorPos, N);

            int currentLine = 0;
            size_t currentCol = 0;
            size_t charsCounted = 0;
            bool found = false;

            for (size_t li = 0; li < lines.size() && !found; ++li) {
                const auto& line = lines[li];
                size_t lineLength = line.size();
                if (i >= charsCounted && i <= charsCounted + lineLength) {
                    currentLine = (int)li;
                    currentCol = i - charsCounted;
                    found = true; break;
                }
                charsCounted += lineLength;
                if (charsCounted < N && full[charsCounted] == '\n') {
                    if (i == charsCounted) { currentLine = (int)li; currentCol = lineLength; found = true; break; }
                    charsCounted++;
                }
            }
            if (!found) { currentLine = (int)lines.size() - 1; currentCol = lines.back().size(); }

            if (preferredColumn < 0) preferredColumn = (int)currentCol;

            int targetLine = currentLine + (goDown ? 1 : -1);
            size_t newPos = cursorPos;

            if (targetLine >= 0 && targetLine < (int)lines.size()) {
                const std::string& targetLineText = lines[(size_t)targetLine];
                size_t targetCol = (size_t)preferredColumn;
                if (targetCol > targetLineText.size()) targetCol = targetLineText.size();

                charsCounted = 0;
                for (int li = 0; li < targetLine; ++li) {
                    charsCounted += lines[(size_t)li].size();
                    if (charsCounted < N && full[charsCounted] == '\n') charsCounted++;
                }
                newPos = std::min(charsCounted + targetCol, N);
            } else {
                newPos = (targetLine < 0) ? 0 : N;
            }

            if (shiftHeld) {
                if (!hasSelection()) selectAnchor = cursorPos;
                cursorPos = newPos;
                setSelection(std::min(selectAnchor, cursorPos), std::max(selectAnchor, cursorPos));
            } else {
                cursorPos = newPos;
                clearSelection();
                selectAnchor = cursorPos;
            }

            lastBlinkTime = SDL_GetTicks(); cursorVisible = true;
            updateCursorPosition(); setIMERectAtCaret();

            i = std::min(cursorPos, N);
            charsCounted = 0; found = false;
            for (size_t li = 0; li < lines.size() && !found; ++li) {
                const auto& line = lines[li];
                size_t lineLength = line.size();
                if (i >= charsCounted && i <= charsCounted + lineLength) {
                    preferredColumn = (int)(i - charsCounted);
                    found = true; break;
                }
                charsCounted += lineLength;
                if (charsCounted < N && full[charsCounted] == '\n') {
                    if (i == charsCounted) { preferredColumn = (int)lineLength; found = true; break; }
                    charsCounted++;
                }
            }
            if (!found && !lines.empty()) preferredColumn = (int)lines.back().size();

            return;
        }

        lastBlinkTime = SDL_GetTicks(); cursorVisible = true;
        updateCursorPosition(); setIMERectAtCaret();
        return;
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
        preferredXpx    = -1;
        preferredColumn = -1;
    }
    TTF_Font* fnt = font ? font : UIConfig::getDefaultFont();
    const UITheme& th = getTheme();
    const UIStyle& ds = getStyle();
    const auto st = MakeTextAreaStyle(th, ds);
    const int innerW = std::max(0, bounds.w - 2*st.borderPx - 2*paddingPx);

    rebuildLayout(fnt, innerW);
    const int viewH = std::max(0, bounds.h - 2*st.borderPx - 2*paddingPx);
    contentHeight = float(std::max<size_t>(1, lines.size())) * float(TTF_FontHeight(fnt));
    scrollOffsetY = std::clamp(scrollOffsetY, 0.0f, std::max(0.0f, contentHeight - float(viewH)));
    if (focused) setIMERectAtCaret();

    {
        Uint32 btns = SDL_GetMouseState(nullptr, nullptr);
        if (selectingMouse && !(btns & SDL_BUTTON(SDL_BUTTON_LEFT))) {
            SDL_CaptureMouse(SDL_FALSE);
            selectingMouse = false;
        }
        if (scrollbarDragging && !(btns & SDL_BUTTON(SDL_BUTTON_LEFT))) {
            scrollbarDragging = false;
        }
    }

    if (focused && selectingMouse) {
        const int borderPx = st.borderPx;
        const int innerY0  = bounds.y + borderPx + paddingPx;
        const int innerH   = std::max(0, bounds.h - 2*borderPx - 2*paddingPx);

        if (my < innerY0 || my >= innerY0 + innerH) {
            int dist = (my < innerY0) ? (innerY0 - my)
                                      : (my - (innerY0 + innerH - 1));
            float step = std::max(1.0f, dist * 0.25f);
            float maxScroll = std::max(0.0f, contentHeight - float(innerH));
            if (my < innerY0)  scrollOffsetY = std::max(0.0f, scrollOffsetY - step);
            else               scrollOffsetY = std::min(maxScroll, scrollOffsetY + step);
        }

        int yForHit = std::clamp(my, innerY0, innerY0 + innerH - 1);

        size_t idx = indexFromMouse(mx, yForHit);
        cursorPos = idx;
        setSelection(std::min(selectAnchor, cursorPos), std::max(selectAnchor, cursorPos));

        preferredColumn = -1;
        preferredXpx    = -1;
        updateCursorPosition();
        setIMERectAtCaret();
        lastBlinkTime = SDL_GetTicks();
        cursorVisible = true;
    }
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

std::vector<std::string> UITextArea::wrapTextToLines(const std::string& text, TTF_Font* font, int maxWidth) const  {
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
        TTF_SizeUTF8(font, temp.c_str(), &w, &h);
        if (w > maxWidth) {
            if (currentLine.empty()) {
                std::string part;
                for (size_t i = 0; i < currentWord.size(); ++i) {
                    std::string test = currentWord.substr(0, i+1);
                    TTF_SizeUTF8(font, test.c_str(), &w, &h);
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
    TTF_Font* fnt = font ? font : (getTheme().font ? getTheme().font : UIConfig::getDefaultFont());
    if (!fnt) return;

    const UITheme& th = getTheme();
    const UIStyle& ds = getStyle();
    const auto st = MakeTextAreaStyle(th, ds);

    SDL_Rect dst = bounds;
    const int effRadius   = st.radius;
    const int effBorderPx = st.borderPx;

    if (focused) {
        UIHelpers::StrokeRoundedRectOutside(renderer, dst, effRadius, effBorderPx + 1, st.borderFocus, st.bg);
    }

    SDL_Color borderNow = focused ? st.borderFocus : st.border;
    if (effBorderPx > 0) {
        UIHelpers::FillRoundedRect(renderer, dst.x, dst.y, dst.w, dst.h, effRadius, borderNow);
        SDL_Rect inner = { dst.x + effBorderPx, dst.y + effBorderPx, dst.w - 2*effBorderPx, dst.h - 2*effBorderPx };
        UIHelpers::FillRoundedRect(renderer, inner.x, inner.y, inner.w, inner.h, std::max(0, effRadius - effBorderPx), st.bg);
        dst = inner;
    } else {
        UIHelpers::FillRoundedRect(renderer, dst.x, dst.y, dst.w, dst.h, effRadius, st.bg);
    }

    SDL_Rect clip = { dst.x + 2, dst.y + 2, dst.w - 4, dst.h - 4 };
    SDL_RenderSetClipRect(renderer, &clip);

    const bool showPlaceholder = linkedText.get().empty() && !focused && !placeholder.empty();
    const int lh = TTF_FontHeight(fnt);
    const int innerX = dst.x + paddingPx;
    const int innerY = dst.y + paddingPx;
    const int innerW = std::max(0, dst.w - 2*paddingPx);
    const int viewH = std::max(0, dst.h - 2*paddingPx);

    if (showPlaceholder) {
        auto surface = UIHelpers::MakeSurface(
            TTF_RenderUTF8_Blended(fnt, placeholder.c_str(), st.placeholder)
        );

        if (surface) {
            auto texture = UIHelpers::MakeTexture(
                SDL_CreateTextureFromSurface(renderer, surface.get())
            );

            SDL_Rect tr{ innerX, innerY, surface->w, surface->h };
            SDL_RenderCopy(renderer, texture.get(), nullptr, &tr);
        }

        contentHeight = float(lh);
        scrollOffsetY = std::clamp(scrollOffsetY, 0.0f, std::max(0.0f, contentHeight - float(viewH)));
        SDL_RenderSetClipRect(renderer, nullptr);
        if (contentHeight > dst.h) renderScrollbar(renderer);
        return;
    }

    rebuildLayout(fnt, innerW);
    contentHeight = float(std::max(1, (int)lines.size()) * lh);
    scrollOffsetY = std::clamp(scrollOffsetY, 0.0f, std::max(0.0f, contentHeight - float(viewH)));

    const std::string& full = linkedText.get();

    size_t selA_orig = 0, selB_orig = 0;
    bool drawSelection = hasSelection();
    if (drawSelection) {
        std::tie(selA_orig, selB_orig) = selectionRange();
    }

    const size_t N = full.size();
    size_t selA = mapOrigToNoNL[std::min(selA_orig, N)];
    size_t selB = mapOrigToNoNL[std::min(selB_orig, N)];

    std::vector<SDL_Rect> selectionRects;
    if (drawSelection) {
        selectionRects.reserve(lines.size());
    }

    int y = innerY - (int)scrollOffsetY;
    for (size_t li = 0; li < lines.size(); ++li) {
        const auto& line = lines[li];

        if (drawSelection) {
            size_t Lg = std::max(selA, lineStart[li]);
            size_t Rg = std::min(selB, lineStart[li] + line.size());
            if (Rg > Lg) {
                size_t Lcol = Lg - lineStart[li];
                size_t Rcol = Rg - lineStart[li];
                int wLeft = prefixX[li][(int)Lcol];
                int wMid  = prefixX[li][(int)Rcol] - prefixX[li][(int)Lcol];

                selectionRects.push_back({innerX + wLeft, y, wMid, lh});
            }

            if (line.empty()) {
                const size_t boundaryNoNL = lineStart[li];
                bool isLineSelected = false;

                if (boundaryNoNL < mapNoNLToOrig.size()) {
                    size_t newlinePos = mapNoNLToOrig[boundaryNoNL];
                    if (newlinePos < full.size() && full[newlinePos] == '\n') {
                        isLineSelected = (selA_orig <= newlinePos && newlinePos < selB_orig);
                    } else {
                        isLineSelected = (selA_orig <= newlinePos && newlinePos <= selB_orig);
                    }
                }

                if (isLineSelected) {
                    int tickW = std::max(2, lh / 8);
                    selectionRects.push_back({innerX, y, tickW, lh});
                }
            }
        }

        if (!line.empty()) {
            auto surface = UIHelpers::MakeSurface(
                TTF_RenderUTF8_Blended(fnt, line.c_str(), st.fg)
            );

            if (surface) {
                auto texture = UIHelpers::MakeTexture(
                    SDL_CreateTextureFromSurface(renderer, surface.get())
                );

                SDL_Rect tr{ innerX, y, surface->w, surface->h };
                SDL_RenderCopy(renderer, texture.get(), nullptr, &tr);
            }
        }
        y += lh;
    }

    if (!selectionRects.empty()) {
        SDL_SetRenderDrawColor(renderer, th.selectionBg.r, th.selectionBg.g,
                               th.selectionBg.b, th.selectionBg.a);
        SDL_RenderFillRects(renderer, selectionRects.data(),
                           static_cast<int>(selectionRects.size()));
    }

    if (focused && cursorVisible && !hasSelection()) {
        const size_t N = full.size();
        const size_t i = std::min(cursorPos, N);

        int visualLine = 0;
        size_t charsCounted = 0;
        bool found = false;

        for (size_t li = 0; li < lines.size() && !found; ++li) {
            const auto& line = lines[li];
            size_t lineLength = line.size();

            if (i >= charsCounted && i <= charsCounted + lineLength) {
                visualLine = li;
                found = true;
                break;
            }
            charsCounted += lineLength;

            if (charsCounted < N && full[charsCounted] == '\n') {
                if (i == charsCounted) {
                    visualLine = li;
                    found = true;
                    break;
                }
                charsCounted++;
            }
        }

        if (!found) {
            visualLine = (int)lines.size() - 1;
        }

        size_t visualCol = 0;
        if (visualLine < (int)lines.size()) {
            size_t lineStartPos = 0;
            charsCounted = 0;

            for (int li = 0; li < visualLine; ++li) {
                charsCounted += lines[li].size();
                if (charsCounted < N && full[charsCounted] == '\n') {
                    charsCounted++;
                }
            }

            lineStartPos = charsCounted;
            visualCol = i - lineStartPos;

            if (visualCol > lines[visualLine].size()) {
                visualCol = lines[visualLine].size();
            }
        }

        const int cx = innerX + prefixX[visualLine][visualCol];
        const int lh = TTF_FontHeight(fnt);
        int cy = innerY + visualLine * lh - (int)scrollOffsetY;

        const int minY = dst.y + paddingPx;
        const int maxY = dst.y + dst.h - paddingPx - lh;
        cy = std::clamp(cy, minY, maxY);

        SDL_SetRenderDrawColor(renderer, st.caret.r, st.caret.g, st.caret.b, st.caret.a);
        SDL_Rect caretR{ cx, cy, 1, lh };
        SDL_RenderFillRect(renderer, &caretR);
    }

    SDL_RenderSetClipRect(renderer, nullptr);
    if (contentHeight > dst.h) renderScrollbar(renderer);
}

void UITextArea::updateCursorPosition() {
    TTF_Font* fnt = font ? font : UIConfig::getDefaultFont();
    if (!fnt) return;

    const UITheme& th = getTheme();
    const UIStyle& ds = getStyle();
    const auto st = MakeTextAreaStyle(th, ds);
    const int borderPx = st.borderPx;
    const int innerX0  = bounds.x + borderPx + paddingPx;
    const int innerY0  = bounds.y + borderPx + paddingPx;
    const int innerW   = std::max(0, bounds.w - 2*borderPx - 2*paddingPx);
    const int viewH    = std::max(0, bounds.h - 2*borderPx - 2*paddingPx);

    rebuildLayout(fnt, innerW);

    const std::string& full = linkedText.get();
    const size_t N = full.size();
    const int lh = TTF_FontHeight(fnt);

    size_t i = std::min(cursorPos, N);

    int visualLine = 0;
    size_t charsCounted = 0;
    bool found = false;

    for (size_t li = 0; li < lines.size() && !found; ++li) {
        const auto& line = lines[li];
        size_t lineLength = line.size();

        if (i >= charsCounted && i <= charsCounted + lineLength) {
            visualLine = li;
            found = true;
            break;
        }
        charsCounted += lineLength;

        if (charsCounted < N && full[charsCounted] == '\n') {
            if (i == charsCounted) {
                visualLine = li;
                found = true;
                break;
            }
            charsCounted++;
        }
    }

    if (!found) {
        visualLine = (int)lines.size() - 1;
    }

    size_t visualCol = 0;
    if (visualLine < (int)lines.size()) {
        size_t lineStartPos = 0;
        charsCounted = 0;

        for (int li = 0; li < visualLine; ++li) {
            charsCounted += lines[li].size();
            if (charsCounted < N && full[charsCounted] == '\n') {
                charsCounted++;
            }
        }

        lineStartPos = charsCounted;
        visualCol = i - lineStartPos;

        if (visualCol > lines[visualLine].size()) {
            visualCol = lines[visualLine].size();
        }
    }

    cursorX = innerX0 + prefixX[visualLine][visualCol];
    cursorY = innerY0 + visualLine * lh;

    const float layoutH = float(std::max<size_t>(1, lines.size())) * float(lh);
    contentHeight = layoutH;

    float top = float(cursorY - innerY0);
    float bot = top + lh;
    if (top < scrollOffsetY) scrollOffsetY = top;
    else if (bot > scrollOffsetY + viewH)  scrollOffsetY = bot - viewH;

    scrollOffsetY = std::clamp(scrollOffsetY, 0.0f, std::max(0.0f, layoutH - float(viewH)));
}




SDL_Rect UITextArea::getScrollbarRect() const {
    const int W = 10;
    return { bounds.x + bounds.w - W - 2, bounds.y + 2, W, bounds.h - 4 };
}

void UITextArea::renderScrollbar(SDL_Renderer* renderer) {
    const UITheme& theme = getTheme();
    const UIStyle& ds = getStyle();
    SDL_Rect sb = getScrollbarRect();
    const auto st = MakeTextAreaStyle(theme, ds);
    const int viewH = std::max(0, bounds.h - 2*st.borderPx - 2*paddingPx);
    float vr = float(viewH)/contentHeight;
    int th = std::max(int(viewH*vr), 20);
    float maxScroll = std::max(0.0f, contentHeight - float(viewH));
    int maxThumb = viewH - th;
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

size_t UITextArea::indexFromMouse(int mx, int my) const {
    TTF_Font* fnt = font ? font : UIConfig::getDefaultFont();
    if (!fnt) return cursorPos;
    const UITheme& th = getTheme();
    const UIStyle& ds = getStyle();
    const auto st = MakeTextAreaStyle(th, ds);
    const int borderPx = st.borderPx;
    const int innerX0  = bounds.x + borderPx + paddingPx;
    const int innerY0  = bounds.y + borderPx + paddingPx;
    const int innerW   = std::max(0, bounds.w - 2*borderPx - 2*paddingPx);

    rebuildLayout(fnt, innerW);

    const int lh = TTF_FontHeight(fnt);
    const std::string& full = linkedText.get();

    int yLocal = my - innerY0 + (int)scrollOffsetY;
    int visualLine = std::clamp(yLocal / std::max(1, lh), 0, (int)lines.size() - 1);


    if (lines[visualLine].empty()) {
        size_t noNLPos = lineStart[visualLine];
        size_t result = (noNLPos < mapNoNLToOrig.size()) ? mapNoNLToOrig[noNLPos] : full.size();
        return result;
    }

    int xLocal = mx - innerX0;
    xLocal = std::clamp(xLocal, 0, std::max(0, innerW - 1));

    const auto& P = prefixX[visualLine];
    const auto& line = lines[visualLine];

    int bestCol = 0;
    if (!P.empty()) {
        if (xLocal >= P.back()) {
            bestCol = (int)line.size();
        } else {
            auto it = std::lower_bound(P.begin(), P.end(), xLocal);
            size_t hi = size_t(it - P.begin());
            size_t lo = (hi == 0) ? 0 : (hi - 1);
            bestCol = (xLocal - P[lo] <= P[hi] - xLocal) ? (int)lo : (int)hi;
        }
    }

    size_t noNLPos = lineStart[visualLine] + bestCol;

    if (noNLPos < mapNoNLToOrig.size()) {
        return mapNoNLToOrig[noNLPos];
    } else {
        return full.size();
    }
}


void UITextArea::setIMERectAtCaret() {
    TTF_Font* fnt = font ? font : UIConfig::getDefaultFont();
    if (!fnt) return;
    const UITheme& th = getTheme();
    const UIStyle& ds = getStyle();
    const auto st = MakeTextAreaStyle(th, ds);
    const int borderPx = st.borderPx;
    const int innerX0  = bounds.x + borderPx + paddingPx;
    const int innerY0  = bounds.y + borderPx + paddingPx;
    const int innerW   = std::max(0, bounds.w - 2*borderPx - 2*paddingPx);
    const int innerH   = std::max(0, bounds.h - 2*borderPx - 2*paddingPx);
    const int lh = TTF_FontHeight(fnt);

    int x = cursorX;
    int y = cursorY - (int)scrollOffsetY;

    x = std::clamp(x, innerX0, innerX0 + innerW - 1);
    y = std::clamp(y, innerY0, innerY0 + innerH - lh);

    SDL_Rect r{ x, y, 1, lh };
    SDL_SetTextInputRect(&r);
}

void UITextArea::setSelection(size_t a, size_t b) {
    if (a > b) std::swap(a,b);

    if (a == b) {
        selStart = selEnd = std::string::npos;
    } else {
        selStart = a;
        selEnd   = b;
    }
}

void UITextArea::rebuildLayout(TTF_Font* fnt, int maxWidthPx) const {
    const std::string& full = linkedText.get();
    if (cacheFont == fnt && cacheWidthPx == maxWidthPx && cacheText == full && !lines.empty())
        return;

    cacheFont = fnt; cacheWidthPx = maxWidthPx; cacheText = full;

    lines.clear();
    lineStart.clear();
    prefixX.clear();

    mapOrigToNoNL.assign(full.size() + 1, 0);
    mapNoNLToOrig.clear();

    size_t noNLIndex = 0;

    std::string para;
    size_t paraStartOrig = 0;

    auto ensure_size = [&](size_t want){
        if (mapNoNLToOrig.size() <= want) mapNoNLToOrig.resize(want + 1);
    };

    auto flushPara = [&](size_t end_i){
        auto wrapped = wrapTextToLines(para, fnt, maxWidthPx);

        const size_t MAX_INDEX = 1000000;
        if (noNLIndex > MAX_INDEX) {
            SDL_LogWarn(SDL_LOG_CATEGORY_APPLICATION,
                        "TextArea layout exceeded index limit");
            return;
        }

        size_t offsetInPara = 0;
        if (wrapped.empty()) {
            lines.push_back("");
            lineStart.push_back(noNLIndex);
            prefixX.emplace_back(1, 0);

            ensure_size(noNLIndex);

            if (end_i < full.size() && full[end_i] == '\n') {
                mapOrigToNoNL[end_i] = noNLIndex;
                mapNoNLToOrig[noNLIndex] = end_i;
            } else {
                mapNoNLToOrig[noNLIndex] = end_i;
            }

            noNLIndex += 1;

        } else {
            for (const std::string& wline : wrapped) {
                const size_t L = wline.size();
                const size_t noNLLineStart = noNLIndex;

                lines.push_back(wline);
                lineStart.push_back(noNLLineStart);

                auto& P = prefixX.emplace_back();
                P.assign(L + 1, 0);
                int w=0,h=0;
                for (size_t j = 1; j <= L; ++j) {
                    std::string sub = wline.substr(0, j);
                    TTF_SizeUTF8(fnt, sub.c_str(), &w, &h);
                    P[j] = w;
                }

                for (size_t j = 0; j < L; ++j) {
                    size_t origPos = paraStartOrig + offsetInPara + j;
                    mapOrigToNoNL[origPos] = noNLLineStart + j;
                }

                ensure_size(noNLLineStart + L);
                for (size_t j = 0; j < L; ++j) {
                    size_t origPos = paraStartOrig + offsetInPara + j;
                    mapNoNLToOrig[noNLLineStart + j] = origPos;
                }

                ensure_size(noNLLineStart + L);
                mapNoNLToOrig[noNLLineStart + L] = paraStartOrig + offsetInPara + L;

                noNLIndex    += L;
                offsetInPara += L;
            }

            if (end_i < full.size() && full[end_i] == '\n') {
                mapOrigToNoNL[end_i] = noNLIndex;
                noNLIndex += 1;
            }
        }
    };

    for (size_t i = 0; i <= full.size(); ++i) {
        const bool atEnd = (i == full.size());
        const char c = atEnd ? '\n' : full[i];
        if (c == '\n') {
            flushPara(i);
            para.clear();
            paraStartOrig = i + 1;
        } else {
            if (para.empty()) paraStartOrig = i;
            para.push_back(c);
        }
    }

    mapOrigToNoNL[full.size()] = noNLIndex;
    if (mapNoNLToOrig.size() <= noNLIndex) mapNoNLToOrig.resize(noNLIndex + 1);
    mapNoNLToOrig[noNLIndex] = full.size();
}




int UITextArea::lineOfIndex(size_t pos) const {
    if (lines.empty() || lineStart.empty()) return 0;

    int lo = 0, hi = (int)lineStart.size() - 1;
    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        if (pos < lineStart[mid]) {
            hi = mid - 1;
        } else if (mid + 1 < (int)lineStart.size() && pos >= lineStart[mid + 1]) {
            lo = mid + 1;
        } else {
            return mid;
        }
    }

    return std::clamp(lo, 0, (int)lineStart.size() - 1);
}



int UITextArea::xAtIndex(size_t pos) const {
    if (lines.empty()) return 0;
    int li = lineOfIndex(pos);
    size_t st = lineStart[li];
    size_t col = (pos > st) ? std::min(pos - st, lines[li].size()) : 0;
    return prefixX[li][col];
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
    const UITheme& th = getTheme();
    const UIStyle& ds = getStyle();
    const auto st = MakeGroupBoxStyle(th, ds);

    TTF_Font* fnt = font ? font : (th.font ? th.font : UIConfig::getDefaultFont());
    if (!fnt) return;

    int titleW = 0, titleH = 0;
    UIHelpers::UniqueTexture titleTex = nullptr;

    if (!title.empty()) {
        auto ts = UIHelpers::MakeSurface(
            TTF_RenderUTF8_Blended(fnt, title.c_str(), st.title)
        );

        if (ts) {
            titleW = ts->w;
            titleH = ts->h;
            titleTex = UIHelpers::MakeTexture(
                SDL_CreateTextureFromSurface(renderer, ts.get())
            );
        }
    }

    SDL_Rect frame = bounds;

    if (st.bg.a > 0) {
        UIHelpers::FillRoundedRect(renderer, frame.x, frame.y, frame.w, frame.h, st.radius, st.bg);
    }

    const int b = std::max<int>(1, st.borderPx);
    SDL_Color frameCol = st.border;

    UIHelpers::FillRoundedRect(renderer, frame.x, frame.y, b, frame.h, st.radius, frameCol);
    UIHelpers::FillRoundedRect(renderer, frame.x + frame.w - b, frame.y, b, frame.h, st.radius, frameCol);
    UIHelpers::FillRoundedRect(renderer, frame.x, frame.y + frame.h - b, frame.w, b, st.radius, frameCol);

    int titleStartX = frame.x + st.titlePadX;
    int titleEndX   = titleStartX + titleW + st.titlePadX;
    int topY        = frame.y;

    if (titleTex)
        UIHelpers::FillRoundedRect(renderer, frame.x, topY, std::max(0, titleStartX - frame.x), b, st.radius, frameCol);
    else
        UIHelpers::FillRoundedRect(renderer, frame.x, topY, frame.w, b, st.radius, frameCol);

    if (titleTex)
        UIHelpers::FillRoundedRect(renderer, titleEndX, topY, std::max(0, frame.x + frame.w - titleEndX), b, st.radius, frameCol);

    if (titleTex) {
        SDL_Rect td { titleStartX, frame.y + st.titlePadY, titleW, titleH };
        SDL_RenderCopy(renderer, titleTex.get(), nullptr, &td);
    }

    for (auto& child : children) {
        if (child) child->render(renderer);
    }
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



namespace {
constexpr int FOCUS_GAIN = 0xF001;
constexpr int FOCUS_LOST = 0xF002;

inline bool isMouseEvent(const SDL_Event& e) {
    switch (e.type) {
        case SDL_MOUSEMOTION:
        case SDL_MOUSEBUTTONDOWN:
        case SDL_MOUSEBUTTONUP:
        case SDL_MOUSEWHEEL:
            return true;
        default: return false;
    }
}
inline bool isKey(const SDL_Event& e, SDL_Keycode k) {
    return e.type == SDL_KEYDOWN && e.key.keysym.sym == k;
}
inline bool modsEqual(Uint16 desired) {
    const Uint16 mask = (KMOD_CTRL | KMOD_ALT | KMOD_SHIFT | KMOD_GUI);
    return (SDL_GetModState() & mask) == desired;
}
}

static void sendFocusEvent(UIElement* el, int code) {
    if (!el) return;
    SDL_Event ev{};
    ev.type = SDL_USEREVENT;
    ev.user.code = code;
    ev.user.data1 = el;
    el->handleEvent(ev);
}

UIManager::~UIManager() { cleanupCursors_(); }

void UIManager::addElement(std::shared_ptr<UIElement> el) {
    elements.push_back(el);
    if (el && el->isFocusable()) registerElement(el.get(), true);
}
void UIManager::showPopup(std::shared_ptr<UIPopup> popup) {
    if (activePopup) closePopup();
    activePopup = std::move(popup);

    savedFocusOrder_  = focusOrder_;
    savedFocusedIndex_ = focusedIndex_;

    focusOrder_.clear();
    if (activePopup) {
        for (auto& ch : activePopup->children) {
            if (ch && ch->isFocusable()) focusOrder_.push_back(ch.get());
        }
    }
    if (!focusOrder_.empty()) setFocusedIndex_(0);
    else clearFocus();
}
std::shared_ptr<UIPopup> UIManager::GetActivePopup() { return activePopup; }
void UIManager::closePopup() { pendingPopupClose = true; }

void UIManager::initCursors()    { ensureCursorsInit_(); }
void UIManager::cleanupCursors() { cleanupCursors_(); }

void UIManager::ensureCursorsInit_() {
    if (cursorsReady) return;
    if ((SDL_WasInit(SDL_INIT_VIDEO) & SDL_INIT_VIDEO) == 0) return;

    arrowCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_ARROW);
    handCursor  = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_HAND);
    ibeamCursor = SDL_CreateSystemCursor(SDL_SYSTEM_CURSOR_IBEAM);

    if (arrowCursor) SDL_SetCursor(arrowCursor);
    cursorsReady = true;
}

void UIManager::cleanupCursors_() {
    if (arrowCursor) { SDL_FreeCursor(arrowCursor); arrowCursor = nullptr; }
    if (handCursor)  { SDL_FreeCursor(handCursor);  handCursor  = nullptr; }
    if (ibeamCursor) { SDL_FreeCursor(ibeamCursor); ibeamCursor = nullptr; }
    cursorsReady = false;
}

void UIManager::checkCursorForElement(const std::shared_ptr<UIElement>& el, SDL_Cursor*& cursorToUse) {
    if (!el->visible) return;

    if (auto ta = dynamic_cast<UITextArea*>(el.get())) {
        if (ta->isScrollbarHovered() || ta->isScrollbarDragging()) return;
        if (ta->isHovered()) { cursorToUse = ibeamCursor; return; }
    }
    if (el->isHovered()) {
        if (dynamic_cast<UITextField*>(el.get())) { cursorToUse = ibeamCursor; return; }
        if (cursorToUse != ibeamCursor &&
            (dynamic_cast<UIButton*>(el.get())   ||
             dynamic_cast<UICheckbox*>(el.get()) ||
             dynamic_cast<UISlider*>(el.get())   ||
             dynamic_cast<UIComboBox*>(el.get()) ||
             dynamic_cast<UISpinner*>(el.get()))) {
            cursorToUse = handCursor;
        }
    }
    if (auto group = dynamic_cast<UIGroupBox*>(el.get())) {
        for (auto& child : group->getChildren()) {
            checkCursorForElement(child, cursorToUse);
            if (cursorToUse == ibeamCursor) return;
        }
    }
    if (auto combo = dynamic_cast<UIComboBox*>(el.get())) {
        if (combo->isExpanded()) {
            int mx, my; SDL_GetMouseState(&mx, &my);
            int itemHeight = combo->getItemHeight();
            int baseY = combo->getBounds().y;
            int itemCount = combo->getItemCount();
            for (int i = 0; i < itemCount; ++i) {
                SDL_Rect itemRect = { combo->getBounds().x, baseY + (i + 1) * itemHeight, combo->getBounds().w, itemHeight };
                SDL_Point pt{ mx, my };
                if (SDL_PointInRect(&pt, &itemRect)) {
                    if (cursorToUse != ibeamCursor) cursorToUse = handCursor;
                    return;
                }
            }
        }
    }
}

void UIManager::registerElement(UIElement* e, bool focusable) {
    if (e && focusable) focusOrder_.push_back(e);
}
void UIManager::setFocusOrder(const std::vector<UIElement*>& order) {
    focusOrder_ = order;
    if (focusedIndex_ >= (int)focusOrder_.size()) focusedIndex_ = -1;
}
void UIManager::focusNext() {
    if (focusOrder_.empty()) return;
    int n = (int)focusOrder_.size();
    int next = ((focusedIndex_ < 0 ? -1 : focusedIndex_) + 1) % n;
    setFocusedIndex_(next);
}
void UIManager::focusPrev() {
    if (focusOrder_.empty()) return;
    int n = (int)focusOrder_.size();
    int prev = ((focusedIndex_ < 0 ? 0 : focusedIndex_) - 1 + n) % n;
    setFocusedIndex_(prev);
}
void UIManager::clearFocus() { setFocusedIndex_(-1); }

void UIManager::captureMouse(UIElement* e) {
    mouseCaptured_ = e;
    SDL_CaptureMouse(SDL_TRUE);
}
void UIManager::releaseMouse() {
    mouseCaptured_ = nullptr;
    SDL_CaptureMouse(SDL_FALSE);
}
void UIManager::setActiveModal(UIElement* m) { activeModal_ = m; }
UIElement* UIManager::activeModal() const { return activeModal_; }
void UIManager::registerShortcut(SDL_Keycode key, Uint16 mods, ShortcutScope scope, std::function<void()> cb) {
    shortcuts_.push_back({key, mods, (int)scope, std::move(cb)});
}

bool UIManager::tryShortcuts_(const SDL_Event& e) {
    if (e.type != SDL_KEYDOWN) return false;
    const auto sym = e.key.keysym.sym;
    for (auto& s : shortcuts_) {
        if (s.key == sym && modsEqual(s.mods)) {
            if (s.scope == ModalOnly && !activeModal_) continue;
            if (s.cb) s.cb();
            return true;
        }
    }
    return false;
}

UIElement* UIManager::hitTestTopMost_(int x, int y) {
    for (int i = (int)elements.size() - 1; i >= 0; --i) {
        const auto& el = elements[i];
        if (!el || !el->visible) continue;
        if (el->isInside(x, y)) return el.get();
    }
    return nullptr;
}

void UIManager::handleEvent(const SDL_Event& e) {
    if (e.type == SDL_MOUSEMOTION) ensureCursorsInit_();
    if (activePopup) {
        if (e.type == SDL_KEYDOWN && e.key.keysym.sym == SDLK_TAB) {
            const bool shift = (SDL_GetModState() & KMOD_SHIFT) != 0;
            if (shift) focusPrev(); else focusNext();
            return;
        }
        activePopup->handleEvent(e);
        return;
    }
    if (activeModal_) { activeModal_->handleEvent(e); return; }

    if (isMouseEvent(e) && mouseCaptured_) { mouseCaptured_->handleEvent(e); return; }

    if (e.type == SDL_KEYDOWN) {
        const bool shift = (SDL_GetModState() & KMOD_SHIFT) != 0;
        if (isKey(e, SDLK_TAB)) { if (shift) focusPrev(); else focusNext(); return; }
        if (isKey(e, SDLK_ESCAPE)) {
            if (activePopup) { activePopup->handleEvent(e); return; }
            if (activeModal_) { activeModal_->handleEvent(e); return; }
            if (focusedIndex_ >= 0 && focusedIndex_ < (int)focusOrder_.size()) { focusOrder_[focusedIndex_]->handleEvent(e); return; }
            clearFocus(); return;
        }
    }

    if (e.type == SDL_TEXTINPUT || e.type == SDL_TEXTEDITING || e.type == SDL_KEYDOWN || e.type == SDL_KEYUP) {
        if (focusedIndex_ >= 0 && focusedIndex_ < (int)focusOrder_.size()) { focusOrder_[focusedIndex_]->handleEvent(e); return; }
        if (tryShortcuts_(e)) return;
    }

    if (isMouseEvent(e)) {
        int mx = 0, my = 0;
        if (e.type == SDL_MOUSEMOTION) { mx = e.motion.x; my = e.motion.y; }
        else if (e.type == SDL_MOUSEBUTTONDOWN || e.type == SDL_MOUSEBUTTONUP) { mx = e.button.x; my = e.button.y; }
        else if (e.type == SDL_MOUSEWHEEL) { SDL_GetMouseState(&mx, &my); }

        if (UIElement* hit = hitTestTopMost_(mx, my)) {
            if (e.type == SDL_MOUSEBUTTONDOWN) {
                int idx = findFocusIndex_(hit);
                setFocusedIndex_(idx);
            }
            hit->handleEvent(e);
            return;
        }
        if (e.type == SDL_MOUSEBUTTONDOWN) {
            clearFocus();
        }
    }
}


void UIManager::update(float dt) {
    ensureCursorsInit_();
    if (pendingPopupClose) {
        activePopup.reset();
        setFocusOrder(savedFocusOrder_);
        if (savedFocusedIndex_ >= 0) setFocusedIndex_(savedFocusedIndex_); else clearFocus();
        savedFocusOrder_.clear();
        savedFocusedIndex_ = -1;
        pendingPopupClose = false;
    }
    if (activePopup && !activePopup->visible) {
        activePopup.reset();
        setFocusOrder(savedFocusOrder_);
        if (savedFocusedIndex_ >= 0) setFocusedIndex_(savedFocusedIndex_); else clearFocus();
        savedFocusOrder_.clear();
        savedFocusedIndex_ = -1;
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
                if (SDL_GetCursor() != cursorToUse) SDL_SetCursor(cursorToUse);
                return;
            }
        }
        for (const auto& el : elements) {
            el->update(dt);
            checkCursorForElement(el, cursorToUse);
        }
    }
    if (SDL_GetCursor() != cursorToUse) SDL_SetCursor(cursorToUse);
}

void UIManager::render(SDL_Renderer* renderer) {
    for (auto& el : elements) if (el->visible) el->render(renderer);
    if (activePopup && activePopup->visible) {
        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_BLEND);
        SDL_SetRenderDrawColor(renderer, 0, 0, 0, 150);

        int rw = 0, rh = 0;
        if (SDL_GetRendererOutputSize(renderer, &rw, &rh) == 0) {
            SDL_Rect fullscreen = { 0, 0, rw, rh };
            SDL_RenderFillRect(renderer, &fullscreen);
        }

        SDL_SetRenderDrawBlendMode(renderer, SDL_BLENDMODE_NONE);
        activePopup->render(renderer);
    }
}

int UIManager::findFocusIndex_(UIElement* e) {
    for (int i = 0; i < (int)focusOrder_.size(); ++i)
        if (focusOrder_[i] == e) return i;
    return -1;
}
static void sendFocusEvent(UIElement* el, int code);

void UIManager::setFocusedIndex_(int idx) {
    if (idx == focusedIndex_) return;
    if (focusedIndex_ >= 0 && focusedIndex_ < (int)focusOrder_.size())
        sendFocusEvent(focusOrder_[focusedIndex_], 0xF002);
    focusedIndex_ = -1;
    if (idx >= 0 && idx < (int)focusOrder_.size()) {
        focusedIndex_ = idx;
        sendFocusEvent(focusOrder_[focusedIndex_], 0xF001);
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

#endif // SDLFORMUI_HPP
#endif // SDLFORMUI_HPP