#include <vector>
#include <array>
#include <set>
#include <map>
#include <algorithm>
#include <cstdio>
#include <cinttypes>
#include <iostream>
#include <deque>

#include "events.h"
#include "rocinante.h"
#include "../launcher/launcher.h"

#include "cpu6502.h"

struct dummyclock
{
    uint64_t cycles = 0;
    void add_cpu_cycles(int N) {
        cycles += N;
    }
};

#define WOZ_MODE_LEFT 72 
#define WOZ_MODE_WIDTH 560 
#define WOZ_MODE_HEIGHT 192 
#define WOZ_MODE_TOP (240 / 2 - WOZ_MODE_HEIGHT / 2) 
#define WOZ_MODE_MIXED_TEXT_ROWS 4
#define WOZ_MODE_FONT_HEIGHT 8
#define WOZ_MODE_FONT_WIDTH 7

static uint8_t NTSCBlack;
static uint8_t NTSCWhite;

static const unsigned char WozModeFontBytes[96 * WOZ_MODE_FONT_HEIGHT] = {
    // 32 :  
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    // 33 : !
    0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 0x08, 0x00, 
    // 34 : "
    0x14, 0x14, 0x14, 0x00, 0x00, 0x00, 0x00, 0x00, 
    // 35 : #
    0x14, 0x14, 0x3E, 0x14, 0x3E, 0x14, 0x14, 0x00, 
    // 36 : $
    0x08, 0x3C, 0x0A, 0x1C, 0x28, 0x1E, 0x08, 0x00, 
    // 37 : %
    0x06, 0x26, 0x10, 0x08, 0x04, 0x32, 0x30, 0x00, 
    // 38 : &
    0x04, 0x0A, 0x0A, 0x04, 0x2A, 0x12, 0x2C, 0x00, 
    // 39 : '
    0x08, 0x08, 0x08, 0x00, 0x00, 0x00, 0x00, 0x00, 
    // 40 : (
    0x08, 0x04, 0x02, 0x02, 0x02, 0x04, 0x08, 0x00, 
    // 41 : )
    0x08, 0x10, 0x20, 0x20, 0x20, 0x10, 0x08, 0x00, 
    // 42 : *
    0x08, 0x2A, 0x1C, 0x08, 0x1C, 0x2A, 0x08, 0x00, 
    // 43 : +
    0x00, 0x08, 0x08, 0x3E, 0x08, 0x08, 0x00, 0x00, 
    // 44 : ,
    0x00, 0x00, 0x00, 0x00, 0x08, 0x08, 0x04, 0x00, 
    // 45 : -
    0x00, 0x00, 0x00, 0x3E, 0x00, 0x00, 0x00, 0x00, 
    // 46 : .
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x08, 0x00, 
    // 47 : /
    0x00, 0x20, 0x10, 0x08, 0x04, 0x02, 0x00, 0x00, 
    // 48 : 0
    0x1C, 0x22, 0x32, 0x2A, 0x26, 0x22, 0x1C, 0x00, 
    // 49 : 1
    0x08, 0x0C, 0x08, 0x08, 0x08, 0x08, 0x1C, 0x00, 
    // 50 : 2
    0x1C, 0x22, 0x20, 0x18, 0x04, 0x02, 0x3E, 0x00, 
    // 51 : 3
    0x3E, 0x20, 0x10, 0x18, 0x20, 0x22, 0x1C, 0x00, 
    // 52 : 4
    0x10, 0x18, 0x14, 0x12, 0x3E, 0x10, 0x10, 0x00, 
    // 53 : 5
    0x3E, 0x02, 0x1E, 0x20, 0x20, 0x22, 0x1C, 0x00, 
    // 54 : 6
    0x38, 0x04, 0x02, 0x1E, 0x22, 0x22, 0x1C, 0x00, 
    // 55 : 7
    0x3E, 0x20, 0x10, 0x08, 0x04, 0x04, 0x04, 0x00, 
    // 56 : 8
    0x1C, 0x22, 0x22, 0x1C, 0x22, 0x22, 0x1C, 0x00, 
    // 57 : 9
    0x1C, 0x22, 0x22, 0x3C, 0x20, 0x10, 0x0E, 0x00, 
    // 58 : :
    0x00, 0x00, 0x08, 0x00, 0x08, 0x00, 0x00, 0x00, 
    // 59 : ;
    0x00, 0x00, 0x08, 0x00, 0x08, 0x08, 0x04, 0x00, 
    // 60 : <
    0x10, 0x08, 0x04, 0x02, 0x04, 0x08, 0x10, 0x00, 
    // 61 : =
    0x00, 0x00, 0x3E, 0x00, 0x3E, 0x00, 0x00, 0x00, 
    // 62 : >
    0x04, 0x08, 0x10, 0x20, 0x10, 0x08, 0x04, 0x00, 
    // 63 : ?
    0x1C, 0x22, 0x10, 0x08, 0x08, 0x00, 0x08, 0x00, 
    // 64 : @
    0x1C, 0x22, 0x2A, 0x3A, 0x1A, 0x02, 0x3C, 0x00, 
    // 65 : A
    0x08, 0x14, 0x22, 0x22, 0x3E, 0x22, 0x22, 0x00, 
    // 66 : B
    0x1E, 0x22, 0x22, 0x1E, 0x22, 0x22, 0x1E, 0x00, 
    // 67 : C
    0x1C, 0x22, 0x02, 0x02, 0x02, 0x22, 0x1C, 0x00, 
    // 68 : D
    0x1E, 0x22, 0x22, 0x22, 0x22, 0x22, 0x1E, 0x00, 
    // 69 : E
    0x3E, 0x02, 0x02, 0x1E, 0x02, 0x02, 0x3E, 0x00, 
    // 70 : F
    0x3E, 0x02, 0x02, 0x1E, 0x02, 0x02, 0x02, 0x00, 
    // 71 : G
    0x3C, 0x02, 0x02, 0x02, 0x32, 0x22, 0x3C, 0x00, 
    // 72 : H
    0x22, 0x22, 0x22, 0x3E, 0x22, 0x22, 0x22, 0x00, 
    // 73 : I
    0x1C, 0x08, 0x08, 0x08, 0x08, 0x08, 0x1C, 0x00, 
    // 74 : J
    0x20, 0x20, 0x20, 0x20, 0x20, 0x22, 0x1C, 0x00, 
    // 75 : K
    0x22, 0x12, 0x0A, 0x06, 0x0A, 0x12, 0x22, 0x00, 
    // 76 : L
    0x02, 0x02, 0x02, 0x02, 0x02, 0x02, 0x3E, 0x00, 
    // 77 : M
    0x22, 0x36, 0x2A, 0x2A, 0x22, 0x22, 0x22, 0x00, 
    // 78 : N
    0x22, 0x22, 0x26, 0x2A, 0x32, 0x22, 0x22, 0x00, 
    // 79 : O
    0x1C, 0x22, 0x22, 0x22, 0x22, 0x22, 0x1C, 0x00, 
    // 80 : P
    0x1E, 0x22, 0x22, 0x1E, 0x02, 0x02, 0x02, 0x00, 
    // 81 : Q
    0x1C, 0x22, 0x22, 0x22, 0x2A, 0x12, 0x2C, 0x00, 
    // 82 : R
    0x1E, 0x22, 0x22, 0x1E, 0x0A, 0x12, 0x22, 0x00, 
    // 83 : S
    0x1C, 0x22, 0x02, 0x1C, 0x20, 0x22, 0x1C, 0x00, 
    // 84 : T
    0x3E, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x00, 
    // 85 : U
    0x22, 0x22, 0x22, 0x22, 0x22, 0x22, 0x1C, 0x00, 
    // 86 : V
    0x22, 0x22, 0x22, 0x22, 0x22, 0x14, 0x08, 0x00, 
    // 87 : W
    0x22, 0x22, 0x22, 0x2A, 0x2A, 0x36, 0x22, 0x00, 
    // 88 : X
    0x22, 0x22, 0x14, 0x08, 0x14, 0x22, 0x22, 0x00, 
    // 89 : Y
    0x22, 0x22, 0x14, 0x08, 0x08, 0x08, 0x08, 0x00, 
    // 90 : Z
    0x3E, 0x20, 0x10, 0x08, 0x04, 0x02, 0x3E, 0x00, 
    // 91 : [
    0x3E, 0x06, 0x06, 0x06, 0x06, 0x06, 0x3E, 0x00, 
    // 92 : backslash
    0x00, 0x02, 0x04, 0x08, 0x10, 0x20, 0x00, 0x00, 
    // 93 : ]
    0x3E, 0x30, 0x30, 0x30, 0x30, 0x30, 0x3E, 0x00, 
    // 94 : ^
    0x00, 0x00, 0x08, 0x14, 0x22, 0x00, 0x00, 0x00, 
    // 95 : _
    0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x7F, 
    // 96 : `
    0x04, 0x08, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 
    // 97 : a
    0x00, 0x00, 0x1C, 0x20, 0x3C, 0x22, 0x3C, 0x00, 
    // 98 : b
    0x02, 0x02, 0x1E, 0x22, 0x22, 0x22, 0x1E, 0x00, 
    // 99 : c
    0x00, 0x00, 0x3C, 0x02, 0x02, 0x02, 0x3C, 0x00, 
    // 100 : d
    0x20, 0x20, 0x3C, 0x22, 0x22, 0x22, 0x3C, 0x00, 
    // 101 : e
    0x00, 0x00, 0x1C, 0x22, 0x3E, 0x02, 0x3C, 0x00, 
    // 102 : f
    0x18, 0x24, 0x04, 0x1E, 0x04, 0x04, 0x04, 0x00, 
    // 103 : g
    0x00, 0x00, 0x1C, 0x22, 0x22, 0x3C, 0x20, 0x1C, 
    // 104 : h
    0x02, 0x02, 0x1E, 0x22, 0x22, 0x22, 0x22, 0x00, 
    // 105 : i
    0x08, 0x00, 0x0C, 0x08, 0x08, 0x08, 0x1C, 0x00, 
    // 106 : j
    0x10, 0x00, 0x18, 0x10, 0x10, 0x10, 0x12, 0x0C, 
    // 107 : k
    0x02, 0x02, 0x22, 0x12, 0x0E, 0x12, 0x22, 0x00, 
    // 108 : l
    0x0C, 0x08, 0x08, 0x08, 0x08, 0x08, 0x1C, 0x00, 
    // 109 : m
    0x00, 0x00, 0x36, 0x2A, 0x2A, 0x2A, 0x22, 0x00, 
    // 110 : n
    0x00, 0x00, 0x1E, 0x22, 0x22, 0x22, 0x22, 0x00, 
    // 111 : o
    0x00, 0x00, 0x1C, 0x22, 0x22, 0x22, 0x1C, 0x00, 
    // 112 : p
    0x00, 0x00, 0x1E, 0x22, 0x22, 0x1E, 0x02, 0x02, 
    // 113 : q
    0x00, 0x00, 0x3C, 0x22, 0x22, 0x3C, 0x20, 0x20, 
    // 114 : r
    0x00, 0x00, 0x3A, 0x06, 0x02, 0x02, 0x02, 0x00, 
    // 115 : s
    0x00, 0x00, 0x3C, 0x02, 0x1C, 0x20, 0x1E, 0x00, 
    // 116 : t
    0x04, 0x04, 0x1E, 0x04, 0x04, 0x24, 0x18, 0x00, 
    // 117 : u
    0x00, 0x00, 0x22, 0x22, 0x22, 0x32, 0x2C, 0x00, 
    // 118 : v
    0x00, 0x00, 0x22, 0x22, 0x22, 0x14, 0x08, 0x00, 
    // 119 : w
    0x00, 0x00, 0x22, 0x22, 0x2A, 0x2A, 0x36, 0x00, 
    // 120 : x
    0x00, 0x00, 0x22, 0x14, 0x08, 0x14, 0x22, 0x00, 
    // 121 : y
    0x00, 0x00, 0x22, 0x22, 0x22, 0x3C, 0x20, 0x1C, 
    // 122 : z
    0x00, 0x00, 0x3E, 0x10, 0x08, 0x04, 0x3E, 0x00, 
    // 123 : {
    0x38, 0x0C, 0x0C, 0x06, 0x0C, 0x0C, 0x38, 0x00, 
    // 124 : |
    0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 0x08, 
    // 125 : }
    0x0E, 0x18, 0x18, 0x30, 0x18, 0x18, 0x0E, 0x00, 
    // 126 : ~
    0x2C, 0x1A, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 
    // 127 : 
    0x00, 0x2A, 0x14, 0x2A, 0x14, 0x2A, 0x00, 0x00, 
};

static void WozMemoryByteToFontIndex(int byte, int *fontIndex, int *inverse)
{
    *inverse = 0;

    if(byte >= 0 && byte <= 31) {
        *fontIndex = byte - 0 + 32;
        *inverse = 1;
    } else if(byte >= 32 && byte <= 63) {
        *fontIndex = byte - 32 + 0;
        *inverse = 1;
    } else if(byte >= 64 && byte <= 95) {
        *fontIndex = byte - 64 + 32; // XXX BLINK 
        *inverse = 1;
    } else if(byte >= 96 && byte <= 127){
        *fontIndex = byte - 96 + 0; // XXX BLINK 
        *inverse = 1;
    } else if(byte >= 128 && byte <= 159)
        *fontIndex = byte - 128 + 32;
    else if(byte >= 160 && byte <= 191)
        *fontIndex = byte - 160 + 0;
    else if(byte >= 192 && byte <= 223)
        *fontIndex = byte - 192 + 32;
    else if(byte >= 224 && byte <= 255)
        *fontIndex = byte - 224 + 64;
    else 
        *fontIndex = 33;
}


uint8_t TextBuffer[0x400];

static void FillRowBuffer40Text([[maybe_unused]] int frameIndex, int rowNumber, [[maybe_unused]] size_t maxSamples, uint8_t* rowBuffer)
{
    int rowIndex = rowNumber - WOZ_MODE_TOP;
    if((rowIndex >= 0) && (rowIndex < 192)) {
        memset(rowBuffer + WOZ_MODE_LEFT, NTSCBlack, WOZ_MODE_WIDTH);
        int rowInText = rowIndex / 8;
        int rowInGlyph = rowIndex % 8;
        const uint8_t *rowSrc = TextBuffer + rowInText * 40;
        uint8_t *rowDst = rowBuffer + WOZ_MODE_LEFT;
        for(int textColumn = 0; textColumn < 40; textColumn++) {
            uint8_t byte = rowSrc[textColumn];
            int fontIndex, inverse;
            WozMemoryByteToFontIndex(byte, &fontIndex, &inverse);
            int fontRowByte = WozModeFontBytes[fontIndex * 8 + rowInGlyph];
            for(int column = 0; column < 7; column ++) {
                *rowDst++ = ((fontRowByte & 0x01) ^ inverse) ? NTSCWhite : NTSCBlack;
                *rowDst++ = ((fontRowByte & 0x01) ^ inverse) ? NTSCWhite : NTSCBlack;
                fontRowByte = fontRowByte >> 1;
            }
        }
    }
}

static void FillRowBuffer(int frameIndex, [[maybe_unused]] int lineWithinField, int rowNumber, size_t maxSamples, uint8_t* rowBuffer)
{
    FillRowBuffer40Text(frameIndex, rowNumber, maxSamples, rowBuffer);
}

static int ModeInit([[maybe_unused]] void *private_data, uint8_t black, uint8_t white)
{
    NTSCBlack = black;
    NTSCWhite = white;

    return 1;
}

static void ModeFini([[maybe_unused]] void *private_data)
{
}

static int ModeNeedsColorburst()
{
    return false;
}


const int textport_row_base_addresses[] = 
{
    0x400,
    0x480,
    0x500,
    0x580,
    0x600,
    0x680,
    0x700,
    0x780,
    0x428,
    0x4A8,
    0x528,
    0x5A8,
    0x628,
    0x6A8,
    0x728,
    0x7A8,
    0x450,
    0x4D0,
    0x550,
    0x5D0,
    0x650,
    0x6D0,
    0x750,
    0x7D0,
};

void draw_textport(const std::array<uint8_t, 64 * 1024>& RAM)
{
    printf("TEXTPORT:\n");
    printf("------------------------------------------\n");
    for(int row = 0; row < 24; row++) {
        printf("|");
        for(int col = 0; col < 40; col++) {
            int addr = textport_row_base_addresses[row] + col;
            int ch = RAM[addr] & 0x7F;
            printf("%c", isprint(ch) ? ch : '?');
        }
        printf("|\n");
    }
    printf("------------------------------------------\n");
}

std::tuple<uint16_t, uint16_t> wozAddressToTextBufferAddress(uint16_t wozAddress)
{
    uint16_t part1 = (wozAddress & 0x0380) >>   7;
    uint16_t part2 = (wozAddress & 0x007F) / 0x28;
    uint16_t byte = (wozAddress & 0x007F) % 0x28;

    uint16_t row = part2 * 8 + part1;
    uint16_t bufferAddress = row * 40 + byte;

    return std::make_tuple(row, bufferAddress);
}

std::deque<uint8_t> keyboard_buffer;

struct bus
{
    std::array<uint8_t, 32 * 1024> ROM;
    std::array<uint8_t, 64 * 1024> RAM;
    bool textport_changed;

    bus(const uint8_t* ROM_) :
        textport_changed(false)
    {
        std::copy(ROM_, ROM_ + 32 * 1024, ROM.begin());
        std::fill(RAM.begin(), RAM.end(), 0xA5);
    }

    uint8_t read(uint16_t addr) const
    {
        if(addr < 0xC000)
        {
            return RAM[addr];
        }
        else if(addr == 0xC000)
        {
            if(!keyboard_buffer.empty()) {
                return 0x80 | keyboard_buffer[0];
            } else {
                return 0x00;
            }
        }
        else if(addr == 0xC010)
        {
            // reset keyboard latch
            if(!keyboard_buffer.empty())
            {
                keyboard_buffer.pop_front();
            }
            return 0x0;
        }
        return ROM[addr - 0x8000];
    }

    void write(uint16_t addr, uint8_t data)
    {
        if(addr < 0xC000)
        {
            RAM[addr] = data;
            if(addr >= 0x400 && addr < 0x800)
            {
                textport_changed = true;
                size_t wozAddress = addr - 0x400;
                uint16_t within_page;
                std::tie(std::ignore, within_page) = wozAddressToTextBufferAddress(wozAddress);
                TextBuffer[within_page] = data;
            }
        }
        else if(addr == 0xC010)
        {
            // reset keyboard latch
            if(!keyboard_buffer.empty())
            {
                keyboard_buffer.pop_front();
            }
        }
    }
};

extern "C" {
int simple_apple2_main(int argc, const char **argv);
};

static int register_app_initializer = []() -> int {
    LauncherRegisterApp("Apple ][ Emulator (no disk)", "", "", "", "", {}, {"apple2.rom"}, simple_apple2_main);
    return 1;
}();

enum EventType
{
    NONE, KEYDOWN, KEYUP, RESET, REBOOT, PASTE, SPEED, QUIT, PAUSE, EJECT_FLOPPY, INSERT_FLOPPY,
    REQUEST_ITERATION_PERIOD_IN_MILLIS,         /* request fixed simulation time period between calls to iterate() */
    WITHDRAW_ITERATION_PERIOD_REQUEST,          /* withdraw request for fixed simulation time */
};

const int LEFT_SHIFT = 340;
const int LEFT_CONTROL = 341;
const int LEFT_ALT = 342;
const int LEFT_SUPER = 343;
const int RIGHT_SHIFT = 344;
const int RIGHT_CONTROL = 345;
const int RIGHT_ALT = 346;
const int RIGHT_SUPER = 347;
const int ESCAPE = 256;
const int ENTER = 257;
const int TAB = 258;
const int BACKSPACE = 259;
const int RIGHT = 262;
const int LEFT = 263;
const int DOWN = 264;
const int UP = 265;
const int CAPS_LOCK = 280;

struct event {
    EventType type;
    int value;
    char *str; // ownership transfered - caller of DequeueEvent must free
    event(EventType type_, int value_, char *str_ = NULL) :
        type(type_),
        value(value_),
        str(str_)
    {}
};

RoKeyRepeatManager keyRepeat;

std::deque<event> event_queue;

bool force_caps_on = true;

static bool event_waiting()
{
    return event_queue.size() > 0;
}

static event dequeue_event()
{
    if(event_waiting()) {
        event e = event_queue.front();
        event_queue.pop_front();
        return e;
    } else
        return {NONE, 0};
}

static const std::map<int, int> HIDkeyToInterfaceKey = 
{
    {KEYCAP_A, 'A'},
    {KEYCAP_B, 'B'},
    {KEYCAP_C, 'C'},
    {KEYCAP_D, 'D'},
    {KEYCAP_E, 'E'},
    {KEYCAP_F, 'F'},
    {KEYCAP_G, 'G'},
    {KEYCAP_H, 'H'},
    {KEYCAP_I, 'I'},
    {KEYCAP_J, 'J'},
    {KEYCAP_K, 'K'},
    {KEYCAP_L, 'L'},
    {KEYCAP_M, 'M'},
    {KEYCAP_N, 'N'},
    {KEYCAP_O, 'O'},
    {KEYCAP_P, 'P'},
    {KEYCAP_Q, 'Q'},
    {KEYCAP_R, 'R'},
    {KEYCAP_S, 'S'},
    {KEYCAP_T, 'T'},
    {KEYCAP_U, 'U'},
    {KEYCAP_V, 'V'},
    {KEYCAP_W, 'W'},
    {KEYCAP_X, 'X'},
    {KEYCAP_Y, 'Y'},
    {KEYCAP_Z, 'Z'},
    {KEYCAP_1_EXCLAMATION, '1'},
    {KEYCAP_2_AT, '2'},
    {KEYCAP_3_NUMBER, '3'},
    {KEYCAP_4_DOLLAR, '4'},
    {KEYCAP_5_PERCENT, '5'},
    {KEYCAP_6_CARET, '6'},
    {KEYCAP_7_AMPERSAND, '7'},
    {KEYCAP_8_ASTERISK, '8'},
    {KEYCAP_9_OPAREN, '9'},
    {KEYCAP_0_CPAREN, '0'},
    {KEYCAP_HYPHEN_UNDER, '-'},
    {KEYCAP_EQUAL_PLUS, '='},
    {KEYCAP_OBRACKET_OBRACE, '['},
    {KEYCAP_CBRACKET_CBRACE, ']'},
    {KEYCAP_BACKSLASH_PIPE, '\\'},
    {KEYCAP_SEMICOLON_COLON, ';'},
    {KEYCAP_SINGLEQUOTE_DOUBLEQUOTE, '\''},
    {KEYCAP_COMMA_LESS, ','},
    {KEYCAP_PERIOD_GREATER, '.'},
    {KEYCAP_SLASH_QUESTION, '/'},
    {KEYCAP_GRAVE_TILDE, '`'},
    {KEYCAP_SPACE, ' '},
    {KEYCAP_ENTER, ENTER},
    {KEYCAP_ESCAPE, ESCAPE},
    {KEYCAP_BACKSPACE, BACKSPACE},
    {KEYCAP_TAB, TAB},
    {KEYCAP_LEFTSHIFT, LEFT_SHIFT},
    {KEYCAP_LEFTCONTROL, LEFT_CONTROL},
    {KEYCAP_LEFTALT, LEFT_ALT},
    {KEYCAP_LEFTGUI, LEFT_SUPER},
    {KEYCAP_RIGHTSHIFT, RIGHT_SHIFT},
    {KEYCAP_RIGHTCONTROL, RIGHT_CONTROL},
    {KEYCAP_RIGHTALT, RIGHT_ALT},
    {KEYCAP_RIGHTGUI, RIGHT_SUPER},
    {KEYCAP_LEFT, LEFT},
    {KEYCAP_RIGHT, RIGHT},
    {KEYCAP_UP, UP},
    {KEYCAP_DOWN, DOWN},
};

static void ProcessKey(int press, int key)
{
    static bool super_down = false;
    static bool caps_lock_down = false;

    // XXX not ideal, can be enqueued out of turn
    if(caps_lock_down && !force_caps_on) {

        caps_lock_down = false;
        event_queue.push_back({KEYUP, CAPS_LOCK});

    } else if(!caps_lock_down && force_caps_on) {

        caps_lock_down = true;
        event_queue.push_back({KEYDOWN, CAPS_LOCK});
    }

    if(press) {

        if(key == KEYCAP_LEFTGUI || key == KEYCAP_RIGHTGUI) {

            super_down = true;

        } else {

            if(key == KEYCAP_CAPSLOCK) {
                force_caps_on = true;
            }

            if(key == KEYCAP_F12) {

                if(super_down) {
                    event_queue.push_back({REBOOT, 0});
                } else {
                    event_queue.push_back({RESET, 0});
                }

            } else {

                if(HIDkeyToInterfaceKey.count(key) > 0) {
                    event_queue.push_back({KEYDOWN, HIDkeyToInterfaceKey.at(key)});
                }
            }
        }

    } else {

        if(key == KEYCAP_LEFTGUI || key == KEYCAP_RIGHTGUI) {

            super_down = false;

        } else {

            if(key == KEYCAP_CAPSLOCK) {
                force_caps_on = false;
            }

            if(HIDkeyToInterfaceKey.count(key) > 0) {
                event_queue.push_back({KEYUP, HIDkeyToInterfaceKey.at(key)});
            }
        }
    }
}

static void poll_events()
{
    struct RoEvent ev;
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

struct key_to_ascii
{
    uint8_t no_shift_no_control;
    uint8_t yes_shift_no_control;
    uint8_t no_shift_yes_control;
    uint8_t yes_shift_yes_control;
};

static std::map<int, key_to_ascii> interface_key_to_apple2e = 
{
    {'A', {97, 65, 1, 1}},
    {'B', {98, 66, 2, 2}},
    {'C', {99, 67, 3, 3}},
    {'D', {100, 68, 4, 4}},
    {'E', {101, 69, 5, 5}},
    {'F', {102, 70, 6, 6}},
    {'G', {103, 71, 7, 7}},
    {'H', {104, 72, 8, 8}},
    {'I', {105, 73, 9, 9}},
    {'J', {106, 74, 10, 10}},
    {'K', {107, 75, 11, 11}},
    {'L', {108, 76, 12, 12}},
    {'M', {109, 77, 13, 13}},
    {'N', {110, 78, 14, 14}},
    {'O', {111, 79, 15, 15}},
    {'P', {112, 80, 16, 16}},
    {'Q', {113, 81, 17, 17}},
    {'R', {114, 82, 18, 18}},
    {'S', {115, 83, 19, 19}},
    {'T', {116, 84, 20, 20}},
    {'U', {117, 85, 21, 21}},
    {'V', {118, 86, 22, 22}},
    {'W', {119, 87, 23, 23}},
    {'X', {120, 88, 24, 24}},
    {'Y', {121, 89, 25, 25}},
    {'Z', {122, 90, 26, 26}},
    {'1', {'1', '!', 0, 0}},
    {'2', {'2', '@', 0, 0}},
    {'3', {'3', '#', 0, 0}},
    {'4', {'4', '$', 0, 0}},
    {'5', {'5', '%', 0, 0}},
    {'6', {'6', '^', 0, 0}},
    {'7', {'7', '&', 0, 0}},
    {'8', {'8', '*', 0, 0}},
    {'9', {'9', '(', 0, 0}},
    {'0', {'0', ')', 0, 0}},
    {'-', {'-', '_', 0, 0}},
    {'=', {'=', '+', 0, 0}},
    {'[', {'[', '{', 0, 0}},
    {']', {']', '}', 0, 0}},
    {'\\', {'\\', '|', 0, 0}},
    {';', {';', ':', 0, 0}},
    {'\'', {'\'', '"', 0, 0}},
    {',', {',', '<', 0, 0}},
    {'.', {'.', '>', 0, 0}},
    {'/', {'/', '?', 0, 0}},
    {'`', {'`', '~', 0, 0}},
    {' ', {' ', ' ', 0, 0}},
};

static void enqueue_key(uint8_t k)
{
    keyboard_buffer.push_back(k);
}

static bool delete_is_left_arrow = true;

static void process_events()
{
    static bool shift_down = false;
    static bool control_down = false;
    static bool caps_down = false;

    while(event_waiting()) {
        event e = dequeue_event();
        if(e.type == KEYDOWN) {
            if((e.value == LEFT_SHIFT) || (e.value == RIGHT_SHIFT))
                shift_down = true;
            else if((e.value == LEFT_CONTROL) || (e.value == RIGHT_CONTROL))
                control_down = true;
            else if(e.value == CAPS_LOCK) {
                caps_down = true;
            } else if(e.value == ENTER) {
                enqueue_key(141 - 128);
            } else if(e.value == TAB) {
                enqueue_key('	');
            } else if(e.value == ESCAPE) {
                enqueue_key('');
            } else if(e.value == BACKSPACE) {
                if(delete_is_left_arrow) {
                    enqueue_key(136 - 128);
                } else {
                    enqueue_key(255 - 128);
                }
            } else if(e.value == RIGHT) {
                enqueue_key(149 - 128);
            } else if(e.value == LEFT) {
                enqueue_key(136 - 128);
            } else if(e.value == DOWN) {
                enqueue_key(138 - 128);
            } else if(e.value == UP) {
                enqueue_key(139 - 128);
            } else {
                auto it = interface_key_to_apple2e.find(e.value);
                if(it != interface_key_to_apple2e.end()) {
                    const key_to_ascii& k = (*it).second;
                    if(!shift_down) {
                        if(!control_down) {
                            if(caps_down && (e.value >= 'A') && (e.value <= 'Z'))
                                enqueue_key(k.yes_shift_no_control);
                            else
                                enqueue_key(k.no_shift_no_control);
                        } else  
                            enqueue_key(k.no_shift_yes_control);
                    } else {
                        if(!control_down)
                            enqueue_key(k.yes_shift_no_control);
                        else
                            enqueue_key(k.yes_shift_yes_control);
                    }
                }
            }
        } else if(e.type == KEYUP) {
            if((e.value == LEFT_SHIFT) || (e.value == RIGHT_SHIFT))
                shift_down = false;
            else if((e.value == LEFT_CONTROL) || (e.value == RIGHT_CONTROL))
                control_down = false;
            else if(e.value == CAPS_LOCK) {
                caps_down = false;
            }
        }
    }
}

int simple_apple2_main(int argc, const char **argv)
{
    if(argc < 2) {
        fprintf(stderr, "usage: %s apple2.rom\n", argv[0]);
        return -1;
    }

    FILE *romfile = fopen(argv[1], "rb");
    if(!romfile) {
        printf("couldn't open \"%s\" for reading\n", argv[1]);
        return -1;
    }

    fseek(romfile, 0, SEEK_END);
    long length = ftell(romfile);
    if(length != 32768)
    {
        printf("expected a 32768 byte ROM file\n");
        return -1;
    }
    fseek(romfile, 0, SEEK_SET);
    std::vector<uint8_t> rom(length);
    fread(rom.data(), 1, length, romfile);
    fclose(romfile);

    dummyclock clock;

    bus machine(rom.data());
    CPU6502<dummyclock, bus> cpu(clock, machine);

    RoVideoSetMode(0, RO_VIDEO_ROW_SAMPLES_912, nullptr, ModeInit, ModeFini, FillRowBuffer, ModeNeedsColorburst);

    bool quit = false;
    uint32_t prev_housekeeping = RoGetMillis();
    uint32_t prev_speed = RoGetMillis();
    uint64_t prev_cycles = 0;
    do {
        for(int i = 0; i < 1000; i++)
        {
            cpu.cycle();
        }

        uint32_t now_housekeeping = RoGetMillis();
        if(now_housekeeping > prev_housekeeping + 50)
        {
            poll_events();
            process_events();
            prev_housekeeping = now_housekeeping;
            RoDoHousekeeping();
        }

        uint32_t now_speed = RoGetMillis();
        if(now_speed > prev_speed + 1000)
        {
            prev_speed = now_speed;

            uint64_t now_cycles = clock.cycles;
            printf("%llu cycles per second (approx %.2fx an Apple ][)\n", now_cycles - prev_cycles, (now_cycles - prev_cycles) / 1023000.0);
            prev_cycles = now_cycles;
        }
    } while(!quit);

    exit(EXIT_SUCCESS);
}
