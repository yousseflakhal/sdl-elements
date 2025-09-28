#include "UIManager.hpp"
#include <SDL2/SDL.h>
#include <algorithm>

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