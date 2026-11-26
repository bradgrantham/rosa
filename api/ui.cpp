#include <cstring>
#include <cstdio>
#include <cctype>
#include <algorithm>
#include <string>
#include <vector>

#include "rocinante.h"
#include "events.h"
#include "text-mode.h"
#include "ui.h"

//----------------------------------------------------------------------------
// Input shared by every prompt.
//
// Every platform has at least a joystick with four directions and fire, so
// the prompts are driven by that; a keyboard is an alternative when one
// exists.  North and south repeat while held.  Fire acts on release so the
// app that is launched doesn't see it still held.

namespace {

enum UiAction {
    UI_NONE,
    UI_UP,
    UI_DOWN,
    UI_LEFT,
    UI_RIGHT,
    UI_SELECT,
    UI_BACK,
    UI_PAGE_UP,
    UI_PAGE_DOWN,
};

constexpr uint32_t UiDebounceMillis = 20;
constexpr uint32_t UiRepeatDelayMillis = 400;
constexpr uint32_t UiRepeatIntervalMillis = 100;
constexpr uint8_t UiDirectionBits = CONTROLLER_NORTH_BIT | CONTROLLER_SOUTH_BIT | CONTROLLER_WEST_BIT | CONTROLLER_EAST_BIT;
constexpr uint8_t UiAllBits = UiDirectionBits | CONTROLLER_FIRE_BIT;

struct UiInput
{
    RoKeyRepeatManager keyRepeat;
    uint8_t repeatMask;     // direction bits that repeat while held
    uint8_t raw;            // most recent joystick sample
    uint32_t rawSince;      // when the sample last changed
    uint8_t stable;         // debounced joystick state
    bool armed;             // false until everything has been seen released
    uint8_t repeatBit;      // direction currently repeating, or 0
    uint32_t nextRepeat;
};

void UiInputInit(UiInput *in, uint8_t repeatMask)
{
    RoKeyRepeatInit(&in->keyRepeat);
    in->repeatMask = repeatMask;
    in->raw = RoGetJoystickState(CONTROLLER_1) & UiAllBits;
    in->rawSince = RoGetMillis();
    in->stable = in->raw;
    // Ignore anything still held from before the prompt opened
    in->armed = (in->raw == 0);
    in->repeatBit = 0;
    in->nextRepeat = 0;
}

UiAction UiDirectionAction(uint8_t bit)
{
    switch(bit) {
        case CONTROLLER_NORTH_BIT: return UI_UP;
        case CONTROLLER_SOUTH_BIT: return UI_DOWN;
        case CONTROLLER_WEST_BIT: return UI_LEFT;
        case CONTROLLER_EAST_BIT: return UI_RIGHT;
        default: return UI_NONE;
    }
}

UiAction UiInputPollJoystick(UiInput *in)
{
    uint32_t now = RoGetMillis();
    uint8_t sample = RoGetJoystickState(CONTROLLER_1) & UiAllBits;

    if(sample != in->raw) {
        in->raw = sample;
        in->rawSince = now;
    }

    UiAction action = UI_NONE;

    if((in->raw != in->stable) && ((now - in->rawSince) >= UiDebounceMillis)) {

        uint8_t pressed = in->raw & ~in->stable;
        uint8_t released = in->stable & ~in->raw;
        in->stable = in->raw;

        if(!in->armed) {

            if(in->stable == 0) {
                in->armed = true;
            }

        } else if(released & CONTROLLER_FIRE_BIT) {

            action = UI_SELECT;

        } else {

            static const uint8_t order[] = {CONTROLLER_NORTH_BIT, CONTROLLER_SOUTH_BIT, CONTROLLER_WEST_BIT, CONTROLLER_EAST_BIT};
            for(uint8_t bit : order) {
                if(pressed & bit) {
                    action = UiDirectionAction(bit);
                    in->repeatBit = (bit & in->repeatMask) ? bit : 0;
                    in->nextRepeat = now + UiRepeatDelayMillis;
                    break;
                }
            }
        }

        if(!(in->stable & in->repeatBit)) {
            in->repeatBit = 0;
        }

    } else if(in->armed && in->repeatBit && (static_cast<int32_t>(now - in->nextRepeat) >= 0)) {

        action = UiDirectionAction(in->repeatBit);
        in->nextRepeat = now + UiRepeatIntervalMillis;
    }

    return action;
}

UiAction UiInputPollKeyboard(UiInput *in)
{
    RoEvent ev;
    int haveEvent = RoEventPoll(&ev);

    haveEvent = RoKeyRepeatUpdate(&in->keyRepeat, haveEvent, &ev);

    if(haveEvent && (ev.eventType == RoEvent::KEYBOARD_RAW) && ev.u.keyboardRaw.isPress) {
        switch(ev.u.keyboardRaw.key) {
            case KEYCAP_UP: return UI_UP;
            case KEYCAP_DOWN: return UI_DOWN;
            case KEYCAP_LEFT: return UI_LEFT;
            case KEYCAP_RIGHT: return UI_RIGHT;
            case KEYCAP_ENTER: return UI_SELECT;
            case KEYCAP_ESCAPE: return UI_BACK;
            case KEYCAP_PAGEUP: return UI_PAGE_UP;
            case KEYCAP_PAGEDOWN: return UI_PAGE_DOWN;
            default: break;
        }
    }
    return UI_NONE;
}

UiAction UiInputPoll(UiInput *in)
{
    UiAction action = UiInputPollJoystick(in);
    if(action == UI_NONE) {
        action = UiInputPollKeyboard(in);
    }
    return action;
}

// Break a message into lines no wider than "width", at newlines and spaces.
std::vector<std::string> UiWrapText(const char *message, size_t width)
{
    std::vector<std::string> lines;
    std::string line;
    std::string word;

    auto flushWord = [&]() {
        while(word.size() > width) {
            // A single word wider than the display; split it
            if(!line.empty()) {
                lines.push_back(line);
                line.clear();
            }
            lines.push_back(word.substr(0, width));
            word.erase(0, width);
        }
        if(word.empty()) {
            return;
        }
        if(line.empty()) {
            line = word;
        } else if(line.size() + 1 + word.size() <= width) {
            line += ' ';
            line += word;
        } else {
            lines.push_back(line);
            line = word;
        }
        word.clear();
    };

    for(const char *p = message; *p; p++) {
        if(*p == '\n') {
            flushWord();
            lines.push_back(line);
            line.clear();
        } else if(*p == ' ') {
            flushWord();
        } else {
            word += *p;
        }
    }
    flushWord();
    if(!line.empty()) {
        lines.push_back(line);
    }
    return lines;
}

};

void RoShowListOfItems(const char *title, const char* const* items, size_t itemsSize, int whichAtTop, int whichSelected, int can_cancel)
{
    int w, h;
    RoTextMode();
    RoTextModeGetSize(&w, &h);
    RoTextModeClearDisplay();

    int titleIndent = (w - strlen(title)) / 2;
    RoTextModeSetLine(0, titleIndent, TEXT_NO_ATTRIBUTES, title);

    bool tooManyRows = (itemsSize - whichAtTop) > static_cast<size_t>((h - 4));
    int rowsToDisplay = tooManyRows ? (h - 4) : (itemsSize - whichAtTop);
    for(int i = 0; i < rowsToDisplay; i++) {
        if(i == whichSelected - whichAtTop) {
            RoTextModeClearArea(0, w, i + 2, 1, TEXT_INVERSE);
            RoTextModeSetLine(i + 2, 0, TEXT_INVERSE, items[whichAtTop + i]);
        } else {
            RoTextModeClearArea(0, w, i + 2, 1, TEXT_NO_ATTRIBUTES);
            RoTextModeSetLine(i + 2, 0, TEXT_NO_ATTRIBUTES, items[whichAtTop + i]);
        }
    }

    if(can_cancel) {
        // 0x11 is a left-pointing triangle in the 8x16 font
        RoTextModeSetLine(h - 1, 0, TEXT_NO_ATTRIBUTES, "\x11 Back");
    }
}

void RoDisplayStringCentered(const char *message)
{
    int w, h;
    RoTextMode();
    RoTextModeGetSize(&w, &h);
    RoTextModeClearDisplay();
    RoTextModeSetLine(h / 2, (w - strlen(message)) / 2, TEXT_NO_ATTRIBUTES, message);
}

void RoDisplayStringAndWaitForEnter(const char *message)
{
    int w, h;
    RoTextMode();
    RoTextModeGetSize(&w, &h);
    RoDisplayStringCentered(message);
    static const char *prompt = "(Press FIRE or ENTER)";
    RoTextModeSetLine(h / 2 + 1, (w - strlen(prompt)) / 2, TEXT_NO_ATTRIBUTES, prompt);

    UiInput input;
    UiInputInit(&input, 0);

    while(UiInputPoll(&input) != UI_SELECT) {
        RoDoHousekeeping(); // XXX
    }
}

Status RoPromptUserToChooseFromList(const char *title, const char* const* items, size_t itemCount, int *itemChosen, int can_cancel)
{
    int whichItemAtTop = 0;
    int whichItemSelected = 0;
    int done = 0;
    int redraw = 1;
    Status status = RO_SUCCESS;

    RoTextMode();

    int w, h;
    RoTextModeGetSize(&w, &h);
    int availableLines = h - 4;
    int lastItem = static_cast<int>(itemCount) - 1;

    UiInput input;
    UiInputInit(&input, CONTROLLER_NORTH_BIT | CONTROLLER_SOUTH_BIT);

    while(!done) {

        if(redraw) {
            RoShowListOfItems(title, items, itemCount, whichItemAtTop, whichItemSelected, can_cancel);
            redraw = 0;
        }

        int moveBy = 0;

        switch(UiInputPoll(&input)) {
            case UI_UP:
                moveBy = -1;
                break;
            case UI_DOWN:
                moveBy = 1;
                break;
            case UI_PAGE_UP:
                moveBy = -(availableLines - 1);
                break;
            case UI_PAGE_DOWN:
                moveBy = availableLines - 1;
                break;
            case UI_SELECT:
                if(lastItem >= 0) {
                    *itemChosen = whichItemSelected;
                    status = RO_SUCCESS;
                    done = 1;
                }
                break;
            case UI_LEFT:
            case UI_BACK:
                if(can_cancel) {
                    *itemChosen = -1;
                    status = RO_USER_DECLINED;
                    done = 1;
                }
                break;
            default:
                break;
        }

        if(moveBy != 0) {
            int selected = std::clamp(whichItemSelected + moveBy, 0, std::max(lastItem, 0));
            if(selected != whichItemSelected) {
                whichItemSelected = selected;
                if(whichItemAtTop > whichItemSelected) {
                    whichItemAtTop = whichItemSelected;
                }
                if(whichItemSelected > whichItemAtTop + (availableLines - 1)) {
                    whichItemAtTop = whichItemSelected - (availableLines - 1);
                }
                redraw = 1;
            }
        }

        RoDoHousekeeping(); // XXX
    }

    return status;
}

Status RoPromptUserWithButtons(const char *title, const char *message, const char* const* buttons, size_t buttonCount, int defaultButton, int *buttonChosen)
{
    RoTextMode();

    int w, h;
    RoTextModeGetSize(&w, &h);

    if((buttonCount < 1) || (defaultButton < 0) || (static_cast<size_t>(defaultButton) >= buttonCount)) {
        return RO_INVALID_PARAMETER_VALUE;
    }

    // Each button is drawn two cells wider than its label, with two cells between buttons
    static constexpr int gap = 2;
    int buttonsWidth = 0;
    for(size_t i = 0; i < buttonCount; i++) {
        buttonsWidth += static_cast<int>(strlen(buttons[i])) + 2 + ((i > 0) ? gap : 0);
    }
    if(buttonsWidth > w) {
        return RO_SIZE_EXCEEDED;
    }

    int buttonRow = h - 2;
    std::vector<std::string> lines = UiWrapText(message, w - 2);

    int selected = defaultButton;
    int redraw = 1;
    Status status = RO_SUCCESS;
    int done = 0;

    UiInput input;
    UiInputInit(&input, 0);

    while(!done) {

        if(redraw) {
            RoTextModeClearDisplay();
            RoTextModeSetLine(0, (w - strlen(title)) / 2, TEXT_NO_ATTRIBUTES, title);

            for(size_t i = 0; (i < lines.size()) && (static_cast<int>(i) + 2 < buttonRow - 1); i++) {
                RoTextModeSetLine(2 + i, 1, TEXT_NO_ATTRIBUTES, lines[i].c_str());
            }

            int column = (w - buttonsWidth) / 2;
            for(size_t i = 0; i < buttonCount; i++) {
                bool isSelected = (static_cast<int>(i) == selected);
                std::string cell = std::string(isSelected ? " " : "[") + buttons[i] + (isSelected ? " " : "]");
                RoTextModeSetLine(buttonRow, column, isSelected ? TEXT_INVERSE : TEXT_NO_ATTRIBUTES, cell.c_str());
                column += static_cast<int>(cell.size()) + gap;
            }
            redraw = 0;
        }

        switch(UiInputPoll(&input)) {
            case UI_LEFT:
                if(selected > 0) {
                    selected--;
                    redraw = 1;
                }
                break;
            case UI_RIGHT:
                if(static_cast<size_t>(selected + 1) < buttonCount) {
                    selected++;
                    redraw = 1;
                }
                break;
            case UI_SELECT:
                *buttonChosen = selected;
                status = RO_SUCCESS;
                done = 1;
                break;
            case UI_BACK:
                *buttonChosen = defaultButton;
                status = RO_USER_DECLINED;
                done = 1;
                break;
            default:
                break;
        }

        RoDoHousekeeping(); // XXX
    }

    return status;
}

char *_strdup(const char *src) // strdup not compiled in with C++ ...?
{
    size_t len = strlen(src) + 1;
    char *cpy = static_cast<char*>(malloc(len));
    memcpy(cpy, src, len);
    return cpy;
}

int RoCompareCaseInsensitive(const char* a, const char* b)
{
    while (*a && *b) {
        unsigned char c1 = static_cast<unsigned char>(std::tolower(*a));
        unsigned char c2 = static_cast<unsigned char>(std::tolower(*b));
        if (c1 != c2) {
            return c1 - c2;
        }
        ++a;
        ++b;
    }
    return static_cast<unsigned char>(*a) - static_cast<unsigned char>(*b);
}

Status RoPromptUserToChooseFile(const char *title, const char *dirName, uint32_t flags, const char *optionalFilterSuffix, char** fileChosen)
{
    char *filenames[256];
    size_t filenamesCount = 0;

    RoTextMode();

    Status result = RoFillFilenameList(dirName, flags, optionalFilterSuffix, 256, filenames, &filenamesCount);
    if(RO_FAILURE(result)) {
        switch(result) {
            case RO_RESOURCE_NOT_FOUND:
                RoDisplayStringAndWaitForEnter(filenames[filenamesCount - 1]);
                break;
            case RO_RESOURCE_EXHAUSTED:
                RoDisplayStringAndWaitForEnter("Too many filenames");
                break;
            default:
                RoDisplayStringAndWaitForEnter("Filename chooser failure");
                break;
        }
        return RO_RESOURCE_NOT_FOUND;
    }
    if(filenamesCount == 0) {
        RoDisplayStringAndWaitForEnter("No .mp3 files found!");
        return RO_RESOURCE_NOT_FOUND;
    }

    std::sort(filenames, filenames + filenamesCount,
              [](const char* lhs, const char* rhs) {
                  return RoCompareCaseInsensitive(lhs, rhs) < 0;
              });

    int whichItemSelected;
    result = RoPromptUserToChooseFromList(title, filenames, filenamesCount, &whichItemSelected, 1);

    if(result == RO_SUCCESS) {
        *fileChosen = _strdup(filenames[whichItemSelected]);
    } else {
        *fileChosen = _strdup("");
    }

    for(size_t i = 0; i < filenamesCount; i++) {
        free(filenames[i]);
    }

    return result;
}
