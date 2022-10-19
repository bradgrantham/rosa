#include "events.h"
#include "rocinante.h"

// TODO don't repeat modifier keys!
void RoKeyRepeatPress(RoKeyRepeatManager *mgr, int pressed)
{
    if(mgr->key != pressed) {
        mgr->key = pressed;
        mgr->state = RoKeyRepeatManager::PRESSED;
        mgr->lastMilli = RoGetMillis();
    }
}

void RoKeyRepeatRelease(RoKeyRepeatManager *mgr, int released)
{
    if(mgr->key == released) {
        mgr->state = RoKeyRepeatManager::NONE;
        mgr->key = KEYCAP_NONE;
    }
}

int RoKeyRepeatUpdate(RoKeyRepeatManager *mgr, int haveEvent, RoEvent* ev)
{
    int now = RoGetMillis();

    if(haveEvent) {
        switch(ev->eventType) {

            case RoEvent::KEYBOARD_RAW: {
                const KeyboardRawEvent& raw = ev->u.keyboardRaw;
                if(raw.isPress) {
                    RoKeyRepeatPress(mgr, raw.key);
                } else {
                    RoKeyRepeatRelease(mgr, raw.key);
                }
                // printf("received raw %s for %d\n", raw.isPress ? "press" : "release", raw.key);
                break;
            }

            default:
                // pass;
                break;
        }
    } else {
        switch(mgr->state) {
            case RoKeyRepeatManager::PRESSED:
                if(now - mgr->lastMilli > 500) {
                    mgr->state = RoKeyRepeatManager::REPEATING;
                    mgr->lastMilli = now;
                    ev->eventType = RoEvent::KEYBOARD_RAW;
                    ev->u.keyboardRaw.isPress = 1;
                    ev->u.keyboardRaw.key = mgr->key;
                    haveEvent = 1;
                }
                break;
            case RoKeyRepeatManager::REPEATING:
                if(now - mgr->lastMilli > 20) {
                    mgr->lastMilli = now;
                    ev->eventType = RoEvent::KEYBOARD_RAW;
                    ev->u.keyboardRaw.isPress = 1;
                    ev->u.keyboardRaw.key = mgr->key;
                    haveEvent = 1;
                }
                break;
            default:
                // pass;
                break;
        }
    }

    return haveEvent;
}
