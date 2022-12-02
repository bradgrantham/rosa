#include <numeric>
#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <thread>
#include <deque>
#include <chrono>
#include <array>
#include <dirent.h>
#include <unistd.h>
#include "SDL.h"

#include "rocinante.h"
#include "events.h"
#include "events_internal.h"

SDL_AudioDeviceID audio_device;
bool audio_needs_start = true;
SDL_AudioFormat actual_audio_format;

void EnqueueStereoU8AudioSamples(uint8_t *buf, size_t sz)
{
    if(audio_needs_start) {
        audio_needs_start = false;
        SDL_PauseAudioDevice(audio_device, 0);
        /* give a little data to avoid gaps and to avoid a pop */
        std::array<uint8_t, 2048> lead_in;
        size_t sampleCount = lead_in.size() / 2;
        for(size_t i = 0; i < sampleCount; i++) {
            lead_in[i * 2 + 0] = static_cast<uint8_t>(128 + (buf[0] - 128) * i / sampleCount);
            lead_in[i * 2 + 1] = static_cast<uint8_t>(128 + (buf[0] - 128) * i / sampleCount);
        }
        SDL_QueueAudio(audio_device, lead_in.data(), static_cast<uint32_t>(lead_in.size()));
    }

    if(actual_audio_format == AUDIO_U8) {
        uint32_t queued;
        while((queued = SDL_GetQueuedAudioSize(audio_device)) > 1024 * 2);
        SDL_QueueAudio(audio_device, buf, static_cast<uint32_t>(sz));
    }
}


SDL_Window *window;
SDL_Renderer *renderer;
SDL_Surface *surface;

const static int SCREEN_X = 704;
const static int SCREEN_Y = 480;
const static int SCREEN_SCALE = 1;

void Start(uint32_t& stereoU8SampleRate, size_t& preferredAudioBufferSizeBytes)
{
#if defined(EMSCRIPTEN)

    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        exit(1);
    }

#else /* ! EMSCRIPTEN */

    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK | SDL_INIT_EVENTS) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        exit(1);
    }

#endif /* EMSCRIPTEN */

    window = SDL_CreateWindow("Rocinante Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_X * SCREEN_SCALE, SCREEN_Y * SCREEN_SCALE, SDL_WINDOW_RESIZABLE);
    if(!window) {
        printf("could not open window\n");
        exit(1);
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(!renderer) {
        printf("could not create renderer\n");
        exit(1);
    }
    surface = SDL_CreateRGBSurface(0, SCREEN_X, SCREEN_Y, 24, 0, 0, 0, 0);
    if(!surface) {
        printf("could not create surface\n");
        exit(1);
    }

    SDL_AudioSpec audiospec;
    audiospec.freq = 44100;
    audiospec.format = AUDIO_U8;
    audiospec.channels = 2;
    audiospec.samples = 1024;
    audiospec.callback = nullptr;
    SDL_AudioSpec obtained;

    audio_device = SDL_OpenAudioDevice(nullptr, 0, &audiospec, &obtained, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);
    assert(audio_device > 0);
    assert(obtained.channels == audiospec.channels);

    switch(obtained.format) {
        case AUDIO_U8: {
            /* okay, native format */
            break;
        }
        default:
            printf("unknown audio format chosen: %X\n", obtained.format);
            exit(1);
    }

    stereoU8SampleRate = obtained.freq;
    preferredAudioBufferSizeBytes = obtained.samples * 2;
    actual_audio_format = obtained.format;

    SDL_PumpEvents();
}

bool shift_pressed = false;
bool quit_requested = false;

static void HandleEvents(void)
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_WINDOWEVENT:
                // printf("window event %d\n", event.window.event);
                // switch(event.window.event) {
                // }
                break;
            case SDL_QUIT:
                quit_requested = true;
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.scancode) {
                    case SDL_SCANCODE_A: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_A }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_B: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_B }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_C: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_C }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_D: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_D }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_E: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_E }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_F: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_F }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_G: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_G }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_H: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_H }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_I: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_I }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_J: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_J }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_K: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_K }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_L: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_L }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_M: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_M }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_N: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_N }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_O: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_O }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_P: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_P }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_Q: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_Q }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_R: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_R }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_S: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_S }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_T: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_T }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_U: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_U }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_V: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_V }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_W: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_W }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_X: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_X }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_Y: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_Y }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_Z: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_Z }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_1: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_1_EXCLAMATION }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_2: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_2_AT }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_3: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_3_NUMBER }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_4: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_4_DOLLAR }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_5: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_5_PERCENT }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_6: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_6_CARET }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_7: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_7_AMPERSAND }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_8: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_8_ASTERISK }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_9: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_9_OPAREN }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_0: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_0_CPAREN }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_RETURN: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_ENTER }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_ESCAPE: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_ESCAPE }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_BACKSPACE: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_BACKSPACE }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_TAB: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_TAB }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_SPACE: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_SPACE }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_MINUS: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_HYPHEN_UNDER }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_EQUALS: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_EQUAL_PLUS }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_LEFTBRACKET: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_OBRACKET_OBRACE }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_RIGHTBRACKET: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_CBRACKET_CBRACE }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_BACKSLASH: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_BACKSLASH_PIPE }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_SEMICOLON: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_SEMICOLON_COLON }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_APOSTROPHE: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_SINGLEQUOTE_DOUBLEQUOTE }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_GRAVE: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_GRAVE_TILDE }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_COMMA: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_COMMA_LESS }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_PERIOD: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_PERIOD_GREATER }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_SLASH: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_SLASH_QUESTION }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_CAPSLOCK: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_CAPSLOCK }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_F1: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_F1 }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_F2: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_F2 }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_F3: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_F3 }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_F4: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_F4 }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_F5: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_F5 }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_F6: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_F6 }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_F7: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_F7 }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_F8: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_F8 }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_F9: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_F9 }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_F10: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_F10 }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_F11: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_F11 }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_F12: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_F12 }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_PRINTSCREEN: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_PRTSCN }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_SCROLLLOCK: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_SCRLK }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_PAUSE: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_PAUSE }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_INSERT: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_INSERT }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_HOME: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_HOME }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_PAGEUP: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_PAGEUP }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_DELETE: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_DELETE }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_END: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_END }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_PAGEDOWN: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_PGDN }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_RIGHT: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_RIGHT }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_LEFT: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_LEFT }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_DOWN: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_DOWN }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_UP: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_UP }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_LCTRL: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_LEFTCONTROL }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_LSHIFT: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_LEFTSHIFT }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_LALT: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_LEFTALT }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_LGUI: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_LEFTGUI }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_RCTRL: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_RIGHTCONTROL }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_RSHIFT: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_RIGHTSHIFT }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_RALT: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_RIGHTALT }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_RGUI: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {1, KEYCAP_RIGHTGUI }}}; SystemEventEnqueue(e); } break;
                    default: break;
                }
                break;
            case SDL_KEYUP:
                switch (event.key.keysym.scancode) {
                    case SDL_SCANCODE_A: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_A }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_B: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_B }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_C: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_C }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_D: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_D }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_E: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_E }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_F: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_F }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_G: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_G }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_H: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_H }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_I: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_I }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_J: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_J }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_K: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_K }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_L: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_L }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_M: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_M }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_N: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_N }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_O: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_O }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_P: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_P }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_Q: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_Q }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_R: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_R }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_S: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_S }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_T: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_T }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_U: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_U }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_V: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_V }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_W: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_W }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_X: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_X }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_Y: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_Y }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_Z: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_Z }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_1: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_1_EXCLAMATION }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_2: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_2_AT }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_3: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_3_NUMBER }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_4: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_4_DOLLAR }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_5: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_5_PERCENT }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_6: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_6_CARET }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_7: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_7_AMPERSAND }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_8: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_8_ASTERISK }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_9: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_9_OPAREN }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_0: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_0_CPAREN }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_RETURN: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_ENTER }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_ESCAPE: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_ESCAPE }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_BACKSPACE: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_BACKSPACE }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_TAB: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_TAB }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_SPACE: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_SPACE }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_MINUS: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_HYPHEN_UNDER }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_EQUALS: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_EQUAL_PLUS }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_LEFTBRACKET: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_OBRACKET_OBRACE }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_RIGHTBRACKET: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_CBRACKET_CBRACE }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_BACKSLASH: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_BACKSLASH_PIPE }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_SEMICOLON: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_SEMICOLON_COLON }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_APOSTROPHE: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_SINGLEQUOTE_DOUBLEQUOTE }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_GRAVE: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_GRAVE_TILDE }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_COMMA: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_COMMA_LESS }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_PERIOD: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_PERIOD_GREATER }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_SLASH: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_SLASH_QUESTION }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_CAPSLOCK: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_CAPSLOCK }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_F1: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_F1 }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_F2: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_F2 }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_F3: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_F3 }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_F4: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_F4 }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_F5: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_F5 }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_F6: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_F6 }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_F7: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_F7 }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_F8: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_F8 }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_F9: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_F9 }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_F10: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_F10 }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_F11: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_F11 }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_F12: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_F12 }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_PRINTSCREEN: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_PRTSCN }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_SCROLLLOCK: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_SCRLK }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_PAUSE: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_PAUSE }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_INSERT: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_INSERT }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_HOME: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_HOME }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_PAGEUP: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_PAGEUP }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_DELETE: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_DELETE }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_END: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_END }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_PAGEDOWN: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_PGDN }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_RIGHT: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_RIGHT }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_LEFT: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_LEFT }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_DOWN: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_DOWN }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_UP: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_UP }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_LCTRL: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_LEFTCONTROL }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_LSHIFT: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_LEFTSHIFT }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_LALT: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_LEFTALT }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_LGUI: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_LEFTGUI }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_RCTRL: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_RIGHTCONTROL }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_RSHIFT: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_RIGHTSHIFT }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_RALT: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_RIGHTALT }}}; SystemEventEnqueue(e); } break;
                    case SDL_SCANCODE_RGUI: { RoEvent e { .eventType = RoEvent::KEYBOARD_RAW, .u = {.keyboardRaw = {0, KEYCAP_RIGHTGUI }}}; SystemEventEnqueue(e); } break;
                    default: break;
                }
                break;
            default:
                break;
        }
    }
}

std::array<float, 4> carrierIQ;

void InitColorDecodeTables(float degrees)
{
    float theta = (degrees / 360.0f) * M_PI * 2; // [0..359] => [0..Math.PI*2)

    carrierIQ = {
        sinf(theta),
        sinf(theta + M_PI / 2),
        sinf(theta + M_PI),
        sinf(theta + M_PI * 3 / 2),
    };
}

void colorHSVToRGB3f(float h, float s, float v, float *r, float *g, float *b)
{
    if(s < .00001) {
        *r = v; *g = v; *b = v;
    } else {
        int i;
        float p, q, t, f;

        h = fmod(h, M_PI * 2);  /* wrap just in case */

        i = floor(h / (M_PI / 3));

        /*
         * would have used "f = fmod(h, M_PI / 3);", but fmod seems to have
         * a bug under Linux.
         */

        f = h / (M_PI / 3) - floor(h / (M_PI / 3));

        p = v * (1 - s);
        q = v * (1 - s * f);
        t = v * (1 - s * (1 - f));
        switch(i) {
            case 0: *r = v; *g = t; *b = p; break;
            case 1: *r = q; *g = v; *b = p; break;
            case 2: *r = p; *g = v; *b = t; break;
            case 3: *r = p; *g = q; *b = v; break;
            case 4: *r = t; *g = p; *b = v; break;
            case 5: *r = v; *g = p; *b = q; break;
        }
    }
}

template <typename T, int N>
struct Averager
{
    int where{0};
    T sum;
    std::array<T, N> data;

    Averager(T init)
    {
        data.fill(init);
        sum = init * N;
    }

    void update(T val)
    {
        sum -= data.at(where);
        data[where] = val;
        sum += val;
        where = (where + 1) % N;
    }

    T at(int i)
    {
        return data.at(i);
    }

    operator float()
    {
        return sum / N;
    }
};

void DecodeColorToRGB(uint8_t *samples, uint8_t *rgb)
{
    float voltage = RoDACValueToVoltage(samples[0]);
    float value = (voltage - NTSC_SYNC_BLACK_VOLTAGE) / (NTSC_SYNC_WHITE_VOLTAGE - NTSC_SYNC_BLACK_VOLTAGE);
    Averager<float, 4> value_averaged(value);
    Averager<float, 4> y_averaged(0);
    Averager<float, 4> i_averaged(0);
    Averager<float, 4> q_averaged(0);
    [[maybe_unused]] float chrominance;
    [[maybe_unused]] float y;
    float i, q;

    for(int idx = 0; idx < 704; idx++) {
        float saturation;

        float voltage = RoDACValueToVoltage(samples[idx]);
        float value = (voltage - NTSC_SYNC_BLACK_VOLTAGE) / (NTSC_SYNC_WHITE_VOLTAGE - NTSC_SYNC_BLACK_VOLTAGE);
        value_averaged.update(value);

        saturation = 0.0;
        for(int i = 0; i < 4; i++) {
            float y = value_averaged.at(i);
            float difference = y - value_averaged; // -1 to 1
            float magnitude = fabsf(difference); // 0 to 1
            saturation = std::max(saturation, magnitude);
        }

        // saturation = std::min(saturation * 2, 1.0f);

        y = value_averaged;
        float signal = value - y;
        i = signal * carrierIQ.at(idx % 4);
        q = signal * carrierIQ.at((idx + 3) % 4);
        y_averaged.update(y);
        i_averaged.update(i);
        q_averaged.update(q);

        chrominance = atan2f(q, i) + M_PI; // 0..Math.PI*2

        float r, g, b;
        RoYIQToRGB(y_averaged, i_averaged, q_averaged, &r, &g, &b);

        uint8_t *pixelp = rgb + idx * 3;
        pixelp[0] = std::clamp(b, 0.0f, 1.0f) * 255.999f;
        pixelp[1] = std::clamp(g, 0.0f, 1.0f) * 255.999f;
        pixelp[2] = std::clamp(r, 0.0f, 1.0f) * 255.999f;
    }
}

void Frame(unsigned char *samples, bool decodeColor)
{
    using namespace std::chrono_literals;

    if (SDL_MUSTLOCK(surface)) SDL_LockSurface(surface);

    uint8_t* framebuffer = reinterpret_cast<uint8_t*>(surface->pixels);
    for(int y = 0; y < 480; y++) {
        if(decodeColor) {
            uint8_t *rowsamples = samples + y * SCREEN_X;
            uint8_t *rowpixels = framebuffer + 3 * y * SCREEN_X;
            DecodeColorToRGB(rowsamples, rowpixels);
        } else {
            for(int x = 0; x < 704; x++) {
                uint8_t *pixel = framebuffer + 3 * (x + y * SCREEN_X);
                uint8_t *sample = samples + x + y * SCREEN_X;
                pixel[0] = sample[0];
                pixel[1] = sample[0];
                pixel[2] = sample[0];
            }
        }
    }

    if (SDL_MUSTLOCK(surface)) SDL_UnlockSurface(surface);

#if 0
    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    std::chrono::duration<float> elapsed;
    
    elapsed = now - previous_event_time;
    if(elapsed.count() > .05) {
        HandleEvents();
        previous_event_time = now;
    }

    elapsed = now - previous_draw_time;
    if(elapsed.count() > .05) {
#endif
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        if(!texture) {
            printf("could not create texture\n");
            exit(1);
        }
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
        SDL_DestroyTexture(texture);
#if 0
        previous_draw_time = now;
    }
#endif
}

#if defined(EMSCRIPTEN)
void caller(void *f_)
{
    std::function<bool()> *f = (std::function<bool()>*)f_;

    bool quit = (*f)();
    if(quit) {
        emscripten_cancel_main_loop();
    }
}
#endif /* EMSCRIPTEN */ 

uint8_t samples[SCREEN_X * SCREEN_Y];

void RoDebugOverlayPrintf(const char *fmt, ...)
{
    (void)fmt;
    static bool once = false;
    if(!once) {
        printf("called unimplemented %s\n", __func__);
        once = true;
    }
    static char buffer[512];
    va_list args;

    va_start(args, fmt);
    vsnprintf(buffer, sizeof(buffer), fmt, args);
    va_end(args);

    puts(buffer);
}

void RoDebugOverlaySetLine(int line, const char *str, size_t size)
{
    printf("RoDebugOverlaySetLine: line %2d \"%*s\"\n", line, static_cast<int>(size), str);
}

void RoAudioGetSamplingInfo(float *rate, size_t *chunkSize)
{
    *rate = 44100;
    *chunkSize = 1024;
}

size_t RoAudioEnqueueSamplesBlocking(size_t writeSize /* in bytes */, uint8_t* buffer)
{
    EnqueueStereoU8AudioSamples(buffer, writeSize);
    return 0; // XXX ?
}

void RoAudioClear()
{
}

Status RoFillFilenameList(const char* dirName, uint32_t flags, const char* optionalFilterSuffix, size_t maxNames, char **filenames, size_t* filenamesSize)
{
    Status status = RO_SUCCESS;

    DIR* dir;

    dir = opendir(dirName);
    if(dir != nullptr) {

        struct dirent* ent;
        while((ent = readdir(dir)) != nullptr) {

            bool addToList = false;

            if (ent->d_type == DT_DIR) {                    /* It is a directory */
                // XXX Really should have a way to descend into directories.
                addToList = false;
            } else if(ent->d_type == DT_REG) {
                if((ent->d_name[0] == '.') && (flags & CHOOSE_FILE_IGNORE_DOTFILES)) {
                    addToList = false;
                } else if(optionalFilterSuffix && (strcmp(optionalFilterSuffix, ent->d_name + ent->d_namlen - strlen(optionalFilterSuffix)) != 0)) {
                    addToList = false;
                } else {
                    addToList = true;
                }
            }

            if(addToList) {
                if(*filenamesSize > maxNames - 1) {
                    printf("maxNames would be exceeded\n");
                    closedir(dir);
                    return RO_RESOURCE_EXHAUSTED;
                }
                int which = *filenamesSize;
                filenames[which] = (char*)malloc(ent->d_namlen + 1);
                if(filenames[which] == nullptr) {
                    printf("malloc for filenames[which] failed\n");
                    closedir(dir);
                    return RO_RESOURCE_EXHAUSTED;
                }

                memcpy(filenames[which], ent->d_name, ent->d_namlen);
                filenames[which][ent->d_namlen] = '\0';

                (*filenamesSize)++;
            }
        }

        // if(errno != 0) {
            // printf("failed to readdir - %d\n", errno);
            // status = RO_INVALID_PARAMETER_VALUE;
        // }

        closedir(dir);

    } else {

        if(*filenamesSize > maxNames - 1) {
            printf("maxNames would be exceeded\n");
            return RO_RESOURCE_EXHAUSTED;
        }
        filenames[(*filenamesSize)++] = strdup("failed to opendir");
        status = RO_RESOURCE_NOT_FOUND;

    }

    return status;
}

uint8_t RoGetJoystickState(RoControllerIndex which)
{
    (void)which;
    static bool once = false;
    if(!once) {
        printf("called unimplemented %s\n", __func__);
        once = true;
    }
    return 0;
}

uint8_t RoGetKeypadState(RoControllerIndex which)
{
    (void)which;
    static bool once = false;
    if(!once) {
        printf("called unimplemented %s\n", __func__);
        once = true;
    }
    return 0;
}

bool NTSCModeFuncsValid = false;
RoNTSCModeFillRowBufferFunc NTSCModeFillRowBuffer;
RoNTSCModeNeedsColorburstFunc NTSCModeNeedsColorburst;
bool NTSCModeInterlaced = false;
uint8_t VideoMemoryBuffer[65536];

void RoNTSCSetMode(int interlaced_, RoNTSCModeInitVideoMemoryFunc initFunc, RoNTSCModeFillRowBufferFunc fillBufferFunc_, RoNTSCModeNeedsColorburstFunc needsColorBurstFunc_)
{
    // XXX Need to lock here versus any threaded access to these variables
    NTSCModeFuncsValid = false;

    NTSCModeFillRowBuffer = fillBufferFunc_;
    NTSCModeNeedsColorburst = needsColorBurstFunc_;
    NTSCModeInterlaced = interlaced_;

    /* Should black and white be similar to HW values to exercise reduced precision? */
    [[maybe_unused]] int result = initFunc(VideoMemoryBuffer, sizeof(VideoMemoryBuffer), 0, 255);
    assert(result == 1);

    NTSCModeFuncsValid = true;
}

extern void RoNTSCWaitFrame(void)
{
    static bool once = false;
    if(!once) {
        printf("called unimplemented %s\n", __func__);
        once = true;
    }
}

void RoDelayMillis(uint32_t millis)
{
    std::this_thread::sleep_for(std::chrono::milliseconds(millis));
}

std::chrono::time_point<std::chrono::system_clock> systemStart = std::chrono::system_clock::now();

uint32_t RoGetMillis()
{
    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    auto elapsed_millis = std::chrono::duration_cast<std::chrono::milliseconds>(now - systemStart).count();
    return elapsed_millis;
}

int RoDoHousekeeping(void)
{
    static std::chrono::time_point<std::chrono::system_clock> last_event_handling = std::chrono::system_clock::now();
    static std::chrono::time_point<std::chrono::system_clock> last_frame_processing = std::chrono::system_clock::now();

    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_event_handling).count();
    if(elapsed > 20) {
        last_event_handling = now;
        HandleEvents();
        if(quit_requested) {
            SDL_Quit();
            exit(0);
        }
    }

    bool needsColorburst = NTSCModeNeedsColorburst();

    now = std::chrono::system_clock::now();
    elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(now - last_frame_processing).count();
    if(elapsed > 15) {
        last_frame_processing = now;
        if(NTSCModeInterlaced) {
            for(int lineNumber = 0; lineNumber < 480; lineNumber++) {
                if(NTSCModeFuncsValid) {
                    memset(samples + lineNumber * 704, 86, 704);
                    NTSCModeFillRowBuffer(0, lineNumber, 704, samples + lineNumber * 704);
                }
            }
        } else {
            // FILE *fp = fopen("foo.bin", "wb");
            for(int lineNumber = 0; lineNumber < 240; lineNumber++) {
                if(NTSCModeFuncsValid) {
                    memset(samples + (lineNumber * 2 + 0) * 704, 86, 704);
                    NTSCModeFillRowBuffer(0, lineNumber, 704, samples + (lineNumber * 2 + 0) * 704);
                    // fwrite(samples + (lineNumber * 2 + 0) * 704, 704, 1, fp);
                    memcpy(samples + (lineNumber * 2 + 1) * 704, samples + (lineNumber * 2 + 0) * 704, 704);
                }
            }
            // fclose(fp);
        }

        Frame(samples, needsColorburst);
    }

    return 0;
}

extern "C" {
    int launcher_main(int argc, const char **argv);
}

/*
    make thread filling the samples buffer
        like STM32 RowHandler, incrementing linenumber and frame number etc
        track time - as close as possible to 15... KHz
        call row funcs
    implement RoGetMillis and RoDelay
    wrap elapsed time around Frame(samples)
    implement EventPoll
    implement DebugOverlay
*/

void usage(const char *progname) 
{
    printf("usage: %s [options]\n", progname);
    printf("options:\n");
    printf("\t--root-dir DIR     # use DIR as the root directory for emulation\n");
}

int main(int argc, char **argv)
{
    uint32_t stereoU8SampleRate;
    size_t preferredAudioBufferSizeBytes;

    InitColorDecodeTables(getenv("OFFSET") ? atof(getenv("OFFSET")) : 123.0f);

    const char *rootDirName = nullptr;

    const char *progname = argv[0];
    argv++;
    argc--;

    while(argc > 0) {
        if(strcmp(argv[0], "--root-dir") == 0) {
            if(argc < 2) {
                fprintf(stderr, "expected root directory for \"--root-dir\"\n");
                usage(progname);
                exit(EXIT_FAILURE);
            }
            rootDirName = argv[1];
            argv += 2;
            argc -= 2;
        } else {
            fprintf(stderr, "unknown option \"%s\"\n", argv[0]);
            usage(progname);
            exit(0);
        }
    }

    if((rootDirName != nullptr) && (chdir(rootDirName) != 0)) {
        perror("chdir");
        exit(EXIT_FAILURE);
    }

    memset(samples, 128, sizeof(samples));

    Start(stereoU8SampleRate, preferredAudioBufferSizeBytes);

#if defined(EMSCRIPTEN)

    std::function<bool()> body_for_emscripten = [&]()->bool{ return Iterate(); };
    emscripten_set_main_loop_arg(caller, &body_for_emscripten, 0, 1);

#else /* !EMSCRIPTEN */

    const char *args[] = {
        "launcher",
    };
    launcher_main(sizeof(args) / sizeof(args[0]), args);

    SDL_Quit();

#endif /* EMSCRIPTEN */

    return 0;
}
