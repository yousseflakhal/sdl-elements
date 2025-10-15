#include "UIStyles.hpp"
#include "UIConfig.hpp"

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
