#include <string.h>
#include <stdio.h>

#include "rocinante.h"
#include "events.h"
#include "text-mode.h"
#include "ui.h"

extern "C" {
uint32_t HAL_GetTick(); // XXX
};

void RoShowListOfItems(const char *title, const char* const* items, size_t itemsSize, int whichAtTop, int whichSelected)
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

    static const char* prompt = "ESC - Cancel, ENTER - Choose";
    int promptIndent = (w - strlen(prompt)) / 2;
    RoTextModeSetLine(h - 1, promptIndent, TEXT_NO_ATTRIBUTES, prompt);
}

void RoDisplayStringCentered(const char *message)
{
    int w, h;
    RoTextMode();
    RoTextModeGetSize(&w, &h);
    RoTextModeClearDisplay();

    RoTextModeSetLine(h / 2, (w - strlen(message)) / 2, TEXT_NO_ATTRIBUTES, message);
}

extern "C" {
    void main_iterate(void);
};

void RoDisplayStringAndWaitForEnter(const char *message)
{
    RoTextMode();
    RoDisplayStringCentered(message);
    int done = 0;
    while(!done) {
        RoEvent ev;
        int haveEvent = RoEventPoll(&ev);
        
        if(haveEvent) {
            switch(ev.eventType) {
                case RoEvent::KEYBOARD_RAW: {
                    // const struct KeyboardRawEvent raw = ev.u.keyboardRaw;
                    done = 1;
                    break;
                }
                default:
                    // pass;
                    break;
            }
        }
        main_iterate(); // XXX
    }
}

Status RoPromptUserToChooseFromList(const char *title, const char* const* items, size_t itemCount, int *itemChosen)
{
    int whichItemAtTop = 0;
    int whichItemSelected = 0;
    int done = 0;
    int redraw = 1;
    Status status = RO_SUCCESS;

    RoTextMode();

    RoKeyRepeatManager keyRepeat;
    uint8_t wasPressed = 0;
    uint32_t debounceStart = 0;

    while(!done) {

        int moveUpOne = 0;
        int moveDownOne = 0;
        int selectCurrentLine = 0;

        if(redraw) {
            RoShowListOfItems(title, items, itemCount, whichItemAtTop, whichItemSelected);
            redraw = 0;
        }

        uint8_t joystick1 = RoGetJoystickState(CONTROLLER_1);
        int northPressed = joystick1 & CONTROLLER_NORTH_BIT;
        int southPressed = joystick1 & CONTROLLER_SOUTH_BIT;
        int firePressed = joystick1 & CONTROLLER_FIRE_BIT;
        if(northPressed || southPressed || firePressed) {
            wasPressed = joystick1;
            debounceStart = HAL_GetTick();
        }
        uint32_t now = HAL_GetTick();
        if(wasPressed && (joystick1 == 0) && ((now - debounceStart) > 20)) {
            if(wasPressed & CONTROLLER_NORTH_BIT) {
                moveUpOne = 1;
            }
            if(wasPressed & CONTROLLER_SOUTH_BIT) {
                moveDownOne = 1;
            }
            if(wasPressed & CONTROLLER_FIRE_BIT) {
                selectCurrentLine = 1;
            }
            wasPressed = 0;
        }

        RoEvent ev;
        int haveEvent = RoEventPoll(&ev);

        haveEvent = RoKeyRepeatUpdate(&keyRepeat, haveEvent, &ev);

        if(haveEvent) {
            switch(ev.eventType) {
                case RoEvent::KEYBOARD_RAW: {
                    const struct KeyboardRawEvent raw = ev.u.keyboardRaw;

                    if(raw.isPress) {

                        if(raw.key == KEYCAP_ESCAPE) {

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

        main_iterate(); // XXX
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

Status RoPromptUserToChooseFile(const char *title, const char *dirName, uint32_t flags, const char *optionalFilterSuffix, char** fileChosen)
{
    char *filenames[256];
    size_t filenamesCount = 0;

    RoTextMode();

    Status result = RoFillFilenameList(dirName, flags, optionalFilterSuffix, 256, filenames, &filenamesCount);
    if(RO_FAILURE(result)) {
        // XXX show some kind of failure
        RoDisplayStringAndWaitForEnter("Filename chooser failure!");
        return RO_RESOURCE_NOT_FOUND;
    }

    int whichItemSelected;
    result = RoPromptUserToChooseFromList(title, filenames, filenamesCount, &whichItemSelected);

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

