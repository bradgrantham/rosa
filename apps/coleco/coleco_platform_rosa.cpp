#include <thread>
#include <deque>
#include <chrono>
#include <cassert>
#include <cstring>

#include "coleco_platform.h"

#include "tms9918.h"

#include "rocinante.h"
#include "events.h"
#include "hid.h"

namespace PlatformInterface
{

//----------------------------------------------------------------------------
// 4-bit 256x192 pixmap mode, can support rasterized TMS9918 screen

/* actually 682.6_ 14MHz clocks because 5.3MHz TMS9918A pixel clock */
/* so the last pixel is extended another 1/3 to fill last clock */
#define Pixmap256_192_4b_MODE_WIDTH 683
#define Pixmap256_192_4b_MODE_LEFT ((704 - Pixmap256_192_4b_MODE_WIDTH) / 2) 
#define Pixmap256_192_4b_MODE_TOP 65 
#define Pixmap256_192_4b_MODE_HEIGHT 192 

uint8_t Pixmap256_192_4b_Framebuffer[256 / 2 * 192];

uint8_t Pixmap256_192_4b_ColorsToNTSC[16][4];

// XXX TODO: convert original patent waveforms into YIQ
// May not be able to get the real YIQ values because resistor
// values are not listed in patent
void Pixmap256_192_4b_SetPaletteEntry(int color, uint8_t r, uint8_t g, uint8_t b)
{
    float y, i, q;
    RoRGBToYIQ(r / 255.0f, g / 255.0f, b / 255.0f, &y, &i, &q);

    Pixmap256_192_4b_ColorsToNTSC[color][0] = RoNTSCYIQToDAC(y, i, q,  .0f);
    Pixmap256_192_4b_ColorsToNTSC[color][1] = RoNTSCYIQToDAC(y, i, q, .25f);
    Pixmap256_192_4b_ColorsToNTSC[color][2] = RoNTSCYIQToDAC(y, i, q, .50f);
    Pixmap256_192_4b_ColorsToNTSC[color][3] = RoNTSCYIQToDAC(y, i, q, .75f);
}

uint8_t Pixmap256_192_4b_GetColorIndex(int x, uint8_t *rowColors)
{
    if((x & 0b1) == 0) {
        return rowColors[x / 2] & 0xF;
    } else {
        return (rowColors[x / 2] & 0xF0) >> 4;
    }
}

int Pixmap256_192_4b_ModeNeedsColorburst()
{
    return 1;
}


__attribute__((hot,flatten)) void Pixmap256_192_4b_ModeFillRowBuffer(int frameIndex, int rowNumber, size_t maxSamples, uint8_t* rowBuffer)
{
    int rowIndex = (rowNumber - Pixmap256_192_4b_MODE_TOP) / 2;
    if((rowIndex >= 0) && (rowIndex < 192)) {
        uint8_t* rowColors = Pixmap256_192_4b_Framebuffer + rowIndex * 128;

        // convert rowColors to NTSC waveform into rowDst 2 2/3 samples at a time.  8-|
        uint16_t index = Pixmap256_192_4b_MODE_LEFT;

        for(int i = 0; i < 255; i += 3) {
            // First color covers 2 and 2/3 of a pixel
            uint8_t *color = Pixmap256_192_4b_ColorsToNTSC[Pixmap256_192_4b_GetColorIndex(i, rowColors)];
            rowBuffer[index + 0] = color[(index + 0) % 4];
            rowBuffer[index + 1] = color[(index + 1) % 4];
            uint8_t carry_over = color[(index + 2) % 4];

            // second color covers 1/3, 2, and 1/3 of a pixel
            color = Pixmap256_192_4b_ColorsToNTSC[Pixmap256_192_4b_GetColorIndex(i + 1, rowColors)];
            rowBuffer[index + 2] = (carry_over * 6 + color[(index + 2) % 4] * 3) / 9;
            rowBuffer[index + 3] = color[(index + 3) % 4];
            rowBuffer[index + 4] = color[(index + 4) % 4];
            carry_over = color[(index + 5) % 4];

            // third color covers 2/3 and 2 pixels
            color = Pixmap256_192_4b_ColorsToNTSC[Pixmap256_192_4b_GetColorIndex(i + 2, rowColors)];
            rowBuffer[index + 5] = (carry_over * 3 + color[(index + 5) % 4] * 6) / 9;
            rowBuffer[index + 6] = color[(index + 6) % 4];
            rowBuffer[index + 7] = color[(index + 7) % 4];

            index += 8;
        }
        // And last pixel is special case
        uint8_t *color = Pixmap256_192_4b_ColorsToNTSC[Pixmap256_192_4b_GetColorIndex(255, rowColors)];
        rowBuffer[index + 0] = color[(index + 0) % 4];
        rowBuffer[index + 1] = color[(index + 1) % 4];
        rowBuffer[index + 2] = color[(index + 2) % 4];
    }
}


std::deque<Event> event_queue;

bool EventIsWaiting()
{
    return event_queue.size() > 0;
}

Event DequeueEvent()
{
    if(EventIsWaiting()) {
        Event e = event_queue.front();
        event_queue.pop_front();
        return e;
    } else
        return {NONE, 0};
}

constexpr uint8_t CONTROLLER1_NORTH_BIT = 0x01;
constexpr uint8_t CONTROLLER1_EAST_BIT = 0x02;
constexpr uint8_t CONTROLLER1_SOUTH_BIT = 0x04;
constexpr uint8_t CONTROLLER1_WEST_BIT = 0x08;
constexpr uint8_t CONTROLLER1_FIRE_LEFT_BIT = 0x40;

constexpr uint8_t CONTROLLER1_KEYPAD_MASK = 0x0F;
constexpr uint8_t CONTROLLER1_FIRE_RIGHT_BIT = 0x40;
constexpr uint8_t CONTROLLER1_KEYPAD_0 = 0x05;
constexpr uint8_t CONTROLLER1_KEYPAD_1 = 0x02;
constexpr uint8_t CONTROLLER1_KEYPAD_2 = 0x08;
constexpr uint8_t CONTROLLER1_KEYPAD_3 = 0x03;
constexpr uint8_t CONTROLLER1_KEYPAD_4 = 0x0D;
constexpr uint8_t CONTROLLER1_KEYPAD_5 = 0x0C;
constexpr uint8_t CONTROLLER1_KEYPAD_6 = 0x01;
constexpr uint8_t CONTROLLER1_KEYPAD_7 = 0x0A;
constexpr uint8_t CONTROLLER1_KEYPAD_8 = 0x0E;
constexpr uint8_t CONTROLLER1_KEYPAD_9 = 0x04;
constexpr uint8_t CONTROLLER1_KEYPAD_asterisk = 0x06;
constexpr uint8_t CONTROLLER1_KEYPAD_pound = 0x09;

uint8_t keyboard_1_joystick_state = 0;
uint8_t keyboard_2_joystick_state = 0;
uint8_t keyboard_1_keypad_state = 0;
uint8_t keyboard_2_keypad_state = 0;

uint8_t GetJoystickState(ControllerIndex controller)
{
    uint8_t data;
    switch(controller) {
        case CONTROLLER_1:
            data = ~(keyboard_1_joystick_state | RoGetJoystickState(RoControllerIndex::CONTROLLER_1)) & 0x7F;
            break;
        case CONTROLLER_2:
            data = ~(keyboard_2_joystick_state | RoGetJoystickState(RoControllerIndex::CONTROLLER_2)) & 0x7F;
            break;
        default: return 0;
    }
    return data;
}

uint8_t GetKeypadState(ControllerIndex controller)
{
    uint8_t data;
    switch(controller) {
        case CONTROLLER_1:
            data = ~(keyboard_1_keypad_state | RoGetKeypadState(RoControllerIndex::CONTROLLER_1)) & 0x7F;
            break;
        case CONTROLLER_2:
            data = ~(keyboard_2_keypad_state | RoGetKeypadState(RoControllerIndex::CONTROLLER_2)) & 0x7F;
            break;
        default: return 0;
    }
    return data;
}

bool audio_needs_start = true;
float audioSampleRate;
size_t audioChunkLengthBytes;
size_t audioBufferCurrent;

void EnqueueStereoU8AudioSamples(uint8_t *buf, size_t sz)
{
    if(audio_needs_start) {
        RoAudioClear();
        /* give a little data to avoid gaps and to avoid a pop */
        static uint8_t lead_in[1024];
        size_t sampleCount = std::min(sizeof(lead_in), audioChunkLengthBytes) / 2;
        for(uint32_t i = 0; i < sampleCount; i++) {
            lead_in[i * 2 + 0] = 128 + (buf[0] - 128) * i / sampleCount;
            lead_in[i * 2 + 1] = 128 + (buf[0] - 128) * i / sampleCount;
        }
        RoAudioEnqueueSamplesBlocking(sampleCount * 2, lead_in);
        audio_needs_start = false;
    }

    RoAudioEnqueueSamplesBlocking(sz, buf);
}

std::chrono::time_point<std::chrono::system_clock> previous_event_time;
std::chrono::time_point<std::chrono::system_clock> start_of_frame;

void Start(uint32_t& stereoU8SampleRate_, size_t& preferredAudioBufferSizeBytes_)
{
    for(int i = 0; i < 16; i++) {
        const uint8_t *c = TMS9918A::Colors[i];
        Pixmap256_192_4b_SetPaletteEntry(i, c[0], c[1], c[2]);
    }
    RoNTSCSetMode(0, Pixmap256_192_4b_ModeFillRowBuffer, Pixmap256_192_4b_ModeNeedsColorburst, nullptr, nullptr);

    RoAudioGetSamplingInfo(&audioSampleRate, &audioChunkLengthBytes);
    stereoU8SampleRate_ = audioSampleRate;
    preferredAudioBufferSizeBytes_ = audioChunkLengthBytes;

    previous_event_time = std::chrono::system_clock::now();
    start_of_frame = std::chrono::system_clock::now();
}

bool right_shift_pressed = false;
bool left_shift_pressed = false;

void ProcessKey(int press, int key)
{
    auto set_bits = [](uint8_t& data, uint8_t bits) { data = data | bits; };
    auto clear_bits = [](uint8_t& data, uint8_t bits) { data = data & ~bits; };
    auto set_bitfield = [](uint8_t& data, uint8_t mask, uint8_t bits) { data = (data & ~mask) | bits; };

    if(press) {

        switch(key) {
            case KEYCAP_LEFTSHIFT:
                left_shift_pressed = true;
                break;
            case KEYCAP_RIGHTSHIFT:
                right_shift_pressed = true;
                break;
            case KEYCAP_W:
                set_bits(keyboard_1_joystick_state, CONTROLLER1_NORTH_BIT);
                break;
            case KEYCAP_A:
                set_bits(keyboard_1_joystick_state, CONTROLLER1_WEST_BIT);
                break;
            case KEYCAP_S:
                set_bits(keyboard_1_joystick_state, CONTROLLER1_SOUTH_BIT);
                break;
            case KEYCAP_D:
                set_bits(keyboard_1_joystick_state, CONTROLLER1_EAST_BIT);
                break;
            case KEYCAP_SPACE:
                set_bits(keyboard_1_joystick_state, CONTROLLER1_FIRE_LEFT_BIT);
                break;
            case KEYCAP_ENTER:
                set_bits(keyboard_1_keypad_state, CONTROLLER1_FIRE_RIGHT_BIT);
                break;
            case KEYCAP_0_CPAREN:
                set_bitfield(keyboard_1_keypad_state, CONTROLLER1_KEYPAD_MASK, CONTROLLER1_KEYPAD_0);
                break;
            case KEYCAP_1_EXCLAMATION:
                set_bitfield(keyboard_1_keypad_state, CONTROLLER1_KEYPAD_MASK, CONTROLLER1_KEYPAD_1);
                break;
            case KEYCAP_2_AT:
                set_bitfield(keyboard_1_keypad_state, CONTROLLER1_KEYPAD_MASK, CONTROLLER1_KEYPAD_2);
                break;
            case KEYCAP_3_NUMBER:
                if(right_shift_pressed || left_shift_pressed) {
                    set_bitfield(keyboard_1_keypad_state, CONTROLLER1_KEYPAD_MASK, CONTROLLER1_KEYPAD_pound);
                } else {
                    set_bitfield(keyboard_1_keypad_state, CONTROLLER1_KEYPAD_MASK, CONTROLLER1_KEYPAD_3);
                }
                break;
            case KEYCAP_4_DOLLAR:
                set_bitfield(keyboard_1_keypad_state, CONTROLLER1_KEYPAD_MASK, CONTROLLER1_KEYPAD_4);
                break;
            case KEYCAP_5_PERCENT:
                set_bitfield(keyboard_1_keypad_state, CONTROLLER1_KEYPAD_MASK, CONTROLLER1_KEYPAD_5);
                break;
            case KEYCAP_6_CARET:
                set_bitfield(keyboard_1_keypad_state, CONTROLLER1_KEYPAD_MASK, CONTROLLER1_KEYPAD_6);
                break;
            case KEYCAP_7_AMPERSAND:
                set_bitfield(keyboard_1_keypad_state, CONTROLLER1_KEYPAD_MASK, CONTROLLER1_KEYPAD_7);
                break;
            case KEYCAP_8_ASTERISK:
                if(right_shift_pressed || left_shift_pressed) {
                    set_bitfield(keyboard_1_keypad_state, CONTROLLER1_KEYPAD_MASK, CONTROLLER1_KEYPAD_asterisk);
                } else {
                    set_bitfield(keyboard_1_keypad_state, CONTROLLER1_KEYPAD_MASK, CONTROLLER1_KEYPAD_8);
                }
                break;
            case KEYCAP_9_OPAREN:
                set_bitfield(keyboard_1_keypad_state, CONTROLLER1_KEYPAD_MASK, CONTROLLER1_KEYPAD_9);
                break;
            default:
                break;
        }

    } else {

        switch(key) {
            case KEYCAP_LEFTSHIFT:
                left_shift_pressed = false;
                break;
            case KEYCAP_RIGHTSHIFT:
                right_shift_pressed = false;
                break;
            case KEYCAP_R:
                event_queue.push_back({RESET, 0});
                break;
            case KEYCAP_W:
                clear_bits(keyboard_1_joystick_state, CONTROLLER1_NORTH_BIT);
                break;
            case KEYCAP_A:
                clear_bits(keyboard_1_joystick_state, CONTROLLER1_WEST_BIT);
                break;
            case KEYCAP_S:
                clear_bits(keyboard_1_joystick_state, CONTROLLER1_SOUTH_BIT);
                break;
            case KEYCAP_D:
                clear_bits(keyboard_1_joystick_state, CONTROLLER1_EAST_BIT);
                break;
            case KEYCAP_SPACE:
                clear_bits(keyboard_1_joystick_state, CONTROLLER1_FIRE_LEFT_BIT);
                break;
            case KEYCAP_ENTER:
                clear_bits(keyboard_1_keypad_state, CONTROLLER1_FIRE_RIGHT_BIT);
                break;
            case KEYCAP_0_CPAREN:
                clear_bits(keyboard_1_keypad_state, CONTROLLER1_KEYPAD_MASK);
                break;
            case KEYCAP_1_EXCLAMATION:
                clear_bits(keyboard_1_keypad_state, CONTROLLER1_KEYPAD_MASK);
                break;
            case KEYCAP_2_AT:
                clear_bits(keyboard_1_keypad_state, CONTROLLER1_KEYPAD_MASK);
                break;
            case KEYCAP_3_NUMBER:
                clear_bits(keyboard_1_keypad_state, CONTROLLER1_KEYPAD_MASK);
                break;
            case KEYCAP_4_DOLLAR:
                clear_bits(keyboard_1_keypad_state, CONTROLLER1_KEYPAD_MASK);
                break;
            case KEYCAP_5_PERCENT:
                clear_bits(keyboard_1_keypad_state, CONTROLLER1_KEYPAD_MASK);
                break;
            case KEYCAP_6_CARET:
                clear_bits(keyboard_1_keypad_state, CONTROLLER1_KEYPAD_MASK);
                break;
            case KEYCAP_7_AMPERSAND:
                clear_bits(keyboard_1_keypad_state, CONTROLLER1_KEYPAD_MASK);
                break;
            case KEYCAP_8_ASTERISK:
                clear_bits(keyboard_1_keypad_state, CONTROLLER1_KEYPAD_MASK);
                break;
            case KEYCAP_9_OPAREN:
                clear_bits(keyboard_1_keypad_state, CONTROLLER1_KEYPAD_MASK);
                break;
            default:
                break;
        }
    }
}

RoKeyRepeatManager keyRepeat;

void HandleEvents()
{
    RoEvent ev;
    int haveEvent;

    do {
        haveEvent = RoEventPoll(&ev);
        
        haveEvent = RoKeyRepeatUpdate(&keyRepeat, haveEvent, &ev);

        if(haveEvent) {

            switch(ev.eventType) {

                case RoEvent::MOUSE_MOVE: {
                    const MouseMoveEvent& move = ev.u.mouseMove;
                    (void)move;
                    // Enqueue joystick?
                    break;
                }

                case RoEvent::CONSOLE_BUTTONPRESS: {
                    const ButtonPressEvent& press = ev.u.buttonPress;
                    if(press.button == 0) {
                        event_queue.push_back({RESET, 0});
                    }
                }

                case RoEvent::MOUSE_BUTTONPRESS: {
                    const ButtonPressEvent& press = ev.u.buttonPress;
                    if(press.button == 0) {
                        // Enqueue joystick?
                    }
                }

                case RoEvent::KEYBOARD_RAW: {
                    const KeyboardRawEvent& raw = ev.u.keyboardRaw;
                    ProcessKey(raw.isPress, raw.key);
                    break;
                }

                default:
                    // pass;
                    break;
            }
        }
    } while(haveEvent);
}

void Frame(const uint8_t* vdp_registers, const uint8_t* vdp_ram, uint8_t& vdp_status_result, [[maybe_unused]] float megahertz)
{
    using namespace std::chrono_literals;

    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    std::chrono::duration<float> elapsed;
    
    elapsed = now - previous_event_time;
    // if(elapsed.count() > .015)
    // {
        HandleEvents();
        // previous_event_time = now;
    // }

    std::chrono::time_point<std::chrono::system_clock> end_of_frame = std::chrono::system_clock::now();
    std::chrono::duration<float> frame_time = end_of_frame - start_of_frame;

    RoNTSCWaitFrame();
    std::chrono::time_point<std::chrono::system_clock> end_of_wait = std::chrono::system_clock::now();
    std::chrono::duration<float> wait_time = end_of_wait - end_of_frame;
    if(frame_time.count() > .0166) {
        // RoDebugOverlayPrintf("%4d %4d\n", (int)(frame_time.count() * 1000.0), (int)(wait_time.count() * 1000.0));
    }
    vdp_status_result = TMS9918A::Create4BitPixmap(vdp_registers, vdp_ram, Pixmap256_192_4b_Framebuffer);
    start_of_frame = std::chrono::system_clock::now();
}

void MainLoopAndShutdown(MainLoopBodyFunc body)
{
    bool quit_requested = false;
    while(!quit_requested)
    {
        quit_requested = body();
    }
}

};


