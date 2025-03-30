#include <cstring>
#include <cstdio>
#include <cctype>
#include <algorithm>

#include "rocinante.h"
#include "events.h"
#include "text-mode.h"
#include "ui.h"

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

    if(false) {
        const char* prompt = can_cancel ? "ENTER/FIRE1 to Choose or ESC/FIRE2" : "ENTER/FIRE1 to Choose";
        int promptIndent = (w - strlen(prompt)) / 2;
        if(promptIndent <= 0) {
            printf("oops, promptIndent == %d\n", promptIndent);
        }
        RoTextModeSetLine(h - 1, promptIndent, TEXT_NO_ATTRIBUTES, prompt);
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
    RoTextModeClearDisplay();
    RoTextModeSetLine(h / 2, (w - strlen(message)) / 2, TEXT_NO_ATTRIBUTES, message);
    RoDisplayStringCentered(message);
    static const char *enter = "(Press ENTER to Continue)";
    RoTextModeSetLine(h / 2 + 1, (w - strlen(enter)) / 2, TEXT_NO_ATTRIBUTES, enter);
    int done = 0;
    while(!done) {
        RoEvent ev;
        int haveEvent = RoEventPoll(&ev);
        
        if(haveEvent) {
            switch(ev.eventType) {
                case RoEvent::KEYBOARD_RAW: {
                    const struct KeyboardRawEvent raw = ev.u.keyboardRaw;
                    if(raw.isPress && (raw.key == RoKeyCap::KEYCAP_ENTER)) {
                        done = 1;
                    }
                    break;
                }
                default:
                    // pass;
                    break;
            }
        }
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

    RoKeyRepeatManager keyRepeat;
    RoKeyRepeatInit(&keyRepeat);
    uint8_t joystick1Was = 0;
    uint8_t keypad1Was = 0;
    uint32_t debounceStart = 0;

    while(!done) {

        int moveUpOne = 0;
        int moveDownOne = 0;
        int selectCurrentLine = 0;

        if(redraw) {
            RoShowListOfItems(title, items, itemCount, whichItemAtTop, whichItemSelected, can_cancel);
            redraw = 0;
        }

        uint8_t joystick1 = RoGetJoystickState(CONTROLLER_1);
        int northPressed = joystick1 & CONTROLLER_NORTH_BIT;
        int southPressed = joystick1 & CONTROLLER_SOUTH_BIT;
        int firePressed = joystick1 & CONTROLLER_FIRE_BIT;
        uint8_t keypad1 = RoGetKeypadState(CONTROLLER_1);
        int fire2Pressed = keypad1 & CONTROLLER_FIRE_BIT;

        // Next two are "Was there a game controller event I care about in this loop?"
        if(northPressed || southPressed || firePressed) {
            joystick1Was = joystick1;
            debounceStart = RoGetMillis();
        }
        if(fire2Pressed) {
            keypad1Was = keypad1;
            debounceStart = RoGetMillis();
        }

        // This code means "Did an event happen but now the event has ended and the debounce delay elapsed between?"
        uint32_t now = RoGetMillis();
        if((keypad1Was | joystick1Was) && ((keypad1 == 0) && (joystick1 == 0)) && ((now - debounceStart) > 20)) {
            if(joystick1Was & CONTROLLER_NORTH_BIT) {
                moveUpOne = 1;
            }
            if(joystick1Was & CONTROLLER_SOUTH_BIT) {
                moveDownOne = 1;
            }
            if(joystick1Was & CONTROLLER_FIRE_BIT) {
                selectCurrentLine = 1;
            }
            if(can_cancel && (keypad1Was & CONTROLLER_FIRE_BIT)) {
                *itemChosen = -1;
                status = RO_USER_DECLINED;
                done = 1;
                break;
            }

            joystick1Was = 0;
            keypad1Was = 0;
        }

        RoEvent ev;
        int haveEvent = RoEventPoll(&ev);

        haveEvent = RoKeyRepeatUpdate(&keyRepeat, haveEvent, &ev);

        if(haveEvent) {
            switch(ev.eventType) {
                case RoEvent::KEYBOARD_RAW: {
                    const struct KeyboardRawEvent raw = ev.u.keyboardRaw;

                    if(raw.isPress) {

                        if(can_cancel && (raw.key == KEYCAP_ESCAPE)) {

                            *itemChosen = -1;
                            status = RO_USER_DECLINED;
                            done = 1;

                        } else if(raw.key == KEYCAP_ENTER) {

                            selectCurrentLine = 1;

                        } else if(raw.key == KEYCAP_UP) {

                            moveUpOne = 1;

                        } else if(raw.key == KEYCAP_DOWN) {

                            moveDownOne = 1;
                        }
                    }
                    break;
                }
                default:
                    // pass;
                    break;
            }
        }

        if(selectCurrentLine) {
            *itemChosen = whichItemSelected;
            status = RO_SUCCESS;
            done = 1;
        }

        if(moveUpOne) {

            whichItemSelected = (whichItemSelected - 1 < 0) ? 0 : (whichItemSelected - 1);
            if(whichItemAtTop > whichItemSelected) {
                whichItemAtTop = whichItemSelected;
            }
            redraw = 1;

        } else if(moveDownOne) {

            int w, h;
            RoTextModeGetSize(&w, &h);
            int availableLines = h - 4;
            whichItemSelected = whichItemSelected + 1;
            if(static_cast<size_t>(whichItemSelected + 1) > itemCount - 1) {
                whichItemSelected = itemCount - 1;
            }
            if(whichItemSelected > whichItemAtTop + (availableLines - 1)) {
                whichItemAtTop = whichItemSelected - (availableLines - 1);
            }
            redraw = 1;
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
