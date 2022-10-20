#include <cstdio>
#include <cstdlib>
#include <cctype>
#include <cstring>
#include <cassert>
#include <deque>
#include <string>
#include <vector>
#include <tuple>
#include <chrono>
#include <iostream>
#include <map>
#include <unordered_map>
#include <unistd.h>

#include "interface.h"

#include "events.h"
#include "rocinante.h"
// #include "hid.h"

using namespace std;

namespace APPLE2Einterface
{

DisplayMode display_mode = TEXT;
int display_page = 0; // Apple //e page minus 1 (so 0,1 not 1,2)
bool mixed_mode = false;
bool vid80 = false;
bool altchar = false;

static constexpr int text_page1_base = 0x400;
static constexpr int text_page2_base = 0x800;
static constexpr int text_page_size = 0x400;
static constexpr int hires_page1_base = 0x2000;
static constexpr int hires_page2_base = 0x4000;
static constexpr int hires_page_size = 8192;

//----------------------------------------------------------------------------
// Woz (Apple ][ graphics) mode

// NTSC single-field (non-interlaced) is 704 visible samples, 240 visible lines (or so)

#define WOZ_MODE_LEFT 72 
#define WOZ_MODE_WIDTH 560 
#define WOZ_MODE_HEIGHT 192 
#define WOZ_MODE_TOP (240 / 2 - WOZ_MODE_HEIGHT / 2) 
#define WOZ_MODE_MIXED_TEXT_ROWS 4
#define WOZ_MODE_FONT_HEIGHT 8
#define WOZ_MODE_FONT_WIDTH 7

DisplayMode WozModeDisplayMode = TEXT;
int WozModeAux = 0;
int WozModeMixed = 0;
int WozModePage = 0;
int WozModeVid80 = 0;
int WozModeDHGR = 0;

void WozModeClearFlags()
{
    WozModeDisplayMode = TEXT;
    WozModeAux = 0;
    WozModeMixed = 0;
    WozModePage = 0;
    WozModeVid80 = 0;
    WozModeDHGR = 0;
}
// indexed by aux, then by page, then by buffer address in scan order, not in Apple ][ memory order
typedef uint8_t WozModeHGRBuffers_t[2][7680];
typedef uint8_t WozModeTextBuffers_t[2][960];
WozModeHGRBuffers_t* WozModeHGRBuffers;
WozModeTextBuffers_t* WozModeTextBuffers;

static uint8_t NTSCBlack;
static uint8_t NTSCWhite;

__attribute__((hot,flatten)) void WozModeFillRowBufferHGR([[maybe_unused]] int frameIndex, int rowNumber, [[maybe_unused]] size_t maxSamples, uint8_t* rowBuffer)
{
    int rowIndex = rowNumber - WOZ_MODE_TOP;
    uint8_t darker = NTSCBlack + (NTSCWhite - NTSCBlack) / 4; // XXX debug
    if((rowIndex >= 0) && (rowIndex < 192)) {
        const uint8_t *rowSrc = WozModeHGRBuffers[WozModeAux][WozModePage] + rowIndex * 40; // row - ...?

        for(int byteIndex = 0; byteIndex < 40; byteIndex++) {

            uint8_t byte = *rowSrc++;

            uint8_t colorShift = byte >> 7;
            uint8_t *rowDst = rowBuffer + WOZ_MODE_LEFT + byteIndex * 14 + colorShift;

            for(int bitIndex = 0; bitIndex < 7; bitIndex++) {
                if(byte & 0x1) {
                    if(0) {
                        // XXX debug
                        *rowDst++ = darker;
                        *rowDst++ = darker;
                    } else {
                        *rowDst++ = NTSCWhite;
                        *rowDst++ = NTSCWhite;
                    }
                } else {
                    if(0) {
                        *rowDst++ = NTSCBlack;
                        *rowDst++ = NTSCBlack;
                    } else {
                        rowDst += 2;
                    }
                }
                byte = byte >> 1;
            }
        }
    }
}

__attribute__((hot,flatten)) void WozModeFillRowBufferDHGR([[maybe_unused]] int frameIndex, int rowNumber, [[maybe_unused]] size_t maxSamples, uint8_t* rowBuffer)
{
    int rowIndex = rowNumber - WOZ_MODE_TOP;
    if((rowIndex >= 0) && (rowIndex < 192)) {
        const uint8_t *rowSrc;
        
        /* Even is bytes from AUX DHGR */
        rowSrc = WozModeHGRBuffers[1][WozModePage] + rowIndex * 40; // row - ...?
        for(int byteIndex = 0; byteIndex < 40; byteIndex++) {

            uint8_t byte = *rowSrc++;

            /* For whatever reason, DHGR starts one clock late. */
            uint8_t *rowDst = rowBuffer + WOZ_MODE_LEFT + byteIndex * 14 + 1;

            for(int bitIndex = 0; bitIndex < 7; bitIndex++) {
                *rowDst++ = (byte & 0x1) ? NTSCWhite : NTSCBlack;
                byte = byte >> 1;
            }
        }

        /* Odd is bytes from MAIN DHGR */
        rowSrc = WozModeHGRBuffers[0][WozModePage] + rowIndex * 40; // row - ...?
        for(int byteIndex = 0; byteIndex < 40; byteIndex++) {

            uint8_t byte = *rowSrc++;

            /* For whatever reason, DHGR starts one clock late. */
            uint8_t *rowDst = rowBuffer + WOZ_MODE_LEFT + byteIndex * 14 + 1 + 7;

            for(int bitIndex = 0; bitIndex < 7; bitIndex++) {
                *rowDst++ = (byte & 0x1) ? NTSCWhite : NTSCBlack;
                byte = byte >> 1;
            }
        }
    }
}

int WozModeFontOffset = 32;
const unsigned char WozModeFontBytes[96 * WOZ_MODE_FONT_HEIGHT] = {
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

void WozMemoryByteToFontIndex(int byte, int *fontIndex, int *inverse)
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

void WozModeFillRowBuffer80Text([[maybe_unused]] int frameIndex, int rowNumber, [[maybe_unused]] size_t maxSamples, uint8_t* rowBuffer)
{
    int rowIndex = rowNumber - WOZ_MODE_TOP;
    if((rowIndex >= 0) && (rowIndex < 192)) {
        memset(rowBuffer + WOZ_MODE_LEFT, NTSCBlack, WOZ_MODE_WIDTH);
        int rowInText = rowIndex / 8;
        int rowInGlyph = rowIndex % 8;

        const uint8_t *rowSrc;
        uint8_t *rowDst;

        rowSrc = WozModeTextBuffers[0][WozModePage] + rowInText * 40;
        rowDst = rowBuffer + WOZ_MODE_LEFT + 7;
        for(int textColumn = 0; textColumn < 40; textColumn++) {
            uint8_t byte = rowSrc[textColumn];
            int fontIndex, inverse;
            WozMemoryByteToFontIndex(byte, &fontIndex, &inverse);
            int fontRowByte = WozModeFontBytes[fontIndex * 8 + rowInGlyph];
            for(int column = 0; column < 7; column ++) {
                *rowDst++ = ((fontRowByte & 0x01) ^ inverse) ? NTSCWhite : NTSCBlack;
                fontRowByte = fontRowByte >> 1;
            }
            rowDst += 7;
        }

        rowSrc = WozModeTextBuffers[1][WozModePage] + rowInText * 40;
        rowDst = rowBuffer + WOZ_MODE_LEFT + 0;
        for(int textColumn = 0; textColumn < 40; textColumn++) {
            uint8_t byte = rowSrc[textColumn];
            int fontIndex, inverse;
            WozMemoryByteToFontIndex(byte, &fontIndex, &inverse);
            int fontRowByte = WozModeFontBytes[fontIndex * 8 + rowInGlyph];
            for(int column = 0; column < 7; column ++) {
                *rowDst++ = ((fontRowByte & 0x01) ^ inverse) ? NTSCWhite : NTSCBlack;
                fontRowByte = fontRowByte >> 1;
            }
            rowDst += 7;
        }
    }
}

void WozModeFillRowBuffer40Text([[maybe_unused]] int frameIndex, int rowNumber, [[maybe_unused]] size_t maxSamples, uint8_t* rowBuffer)
{
    int rowIndex = rowNumber - WOZ_MODE_TOP;
    if((rowIndex >= 0) && (rowIndex < 192)) {
        memset(rowBuffer + WOZ_MODE_LEFT, NTSCBlack, WOZ_MODE_WIDTH);
        int rowInText = rowIndex / 8;
        int rowInGlyph = rowIndex % 8;
        const uint8_t *rowSrc = WozModeTextBuffers[WozModeAux][WozModePage] + rowInText * 40;
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

void WozModeFillRowBufferLGR([[maybe_unused]] int frameIndex, int rowNumber, [[maybe_unused]] size_t maxSamples, uint8_t* rowBuffer)
{
    int rowIndex = rowNumber - WOZ_MODE_TOP;
    if((rowIndex >= 0) && (rowIndex < 192)) {

        memset(rowBuffer + WOZ_MODE_LEFT, NTSCBlack, WOZ_MODE_WIDTH);

        int rowInText = rowIndex / 8;
        int rowInGlyph = rowIndex % 8;

        const uint8_t *rowSrc = WozModeTextBuffers[WozModeAux][WozModePage] + rowInText * 40;

        uint8_t *rowDst = rowBuffer + WOZ_MODE_LEFT;

        for(int column = 0; column < 560;) {
            uint8_t byte = rowSrc[column / 14];
            uint8_t nybble = ((rowInGlyph < 4) ? (byte) : (byte >> 4)) & 0xF;

            for(int i = 0; i < 14; i++, column++) {
                int bitInNybble = column % 4;
                int bit = nybble & (1 << bitInNybble);
                *rowDst++ = bit ? NTSCWhite : NTSCBlack;
            }
        }
    }
}

int WozModeNeedsColorburst()
{
    return (WozModeDisplayMode != TEXT);
}

void WozModeFillRowBuffer(int frameIndex, int rowNumber, size_t maxSamples, uint8_t* rowBuffer)
{
    int rowIndex = rowNumber - WOZ_MODE_TOP;
    DisplayMode mode = WozModeDisplayMode;
    if(WozModeMixed && (rowIndex >= WOZ_MODE_HEIGHT - WOZ_MODE_MIXED_TEXT_ROWS * WOZ_MODE_FONT_HEIGHT)) {
        mode = TEXT;
    }
    switch(mode) {
        case TEXT: 
            if(WozModeVid80) {
                WozModeFillRowBuffer80Text(frameIndex, rowNumber, maxSamples, rowBuffer);
            } else {
                WozModeFillRowBuffer40Text(frameIndex, rowNumber, maxSamples, rowBuffer);
            }
            break;
        case HIRES: 
            if(WozModeDHGR) {
                WozModeFillRowBufferDHGR(frameIndex, rowNumber, maxSamples, rowBuffer);
            } else {
                WozModeFillRowBufferHGR(frameIndex, rowNumber, maxSamples, rowBuffer);
            }
            break;
        case LORES: 
            WozModeFillRowBufferLGR(frameIndex, rowNumber, maxSamples, rowBuffer);
            break;
    }
}

//----------------------------------------------------------------------------

RoKeyRepeatManager keyRepeat;

deque<event> event_queue;

bool force_caps_on = true;

float audioSampleRate;
size_t audioChunkLengthBytes;

bool event_waiting()
{
    return event_queue.size() > 0;
}

event dequeue_event()
{
    if(event_waiting()) {
        event e = event_queue.front();
        event_queue.pop_front();
        return e;
    } else
        return {NONE, 0};
}

tuple<float,bool> get_paddle(int num)
{
    if(num < 0 || num > 3) {
        return make_tuple(-1, false);
    }
    return make_tuple(0, false);
}

static int WozModeInitVideoMemory(void *videoMemory, uint32_t size, uint8_t black, uint8_t white)
{
    auto reserve = [](void*& ptr, uint32_t& remaining, size_t rsv) {
        void *old = ptr;
        assert(remaining >= rsv);
        ptr = static_cast<uint8_t*>(ptr) + rsv;
        remaining -= rsv;
        return old;
    };

    WozModeHGRBuffers = static_cast<WozModeHGRBuffers_t*>(reserve(videoMemory, size, 2 * sizeof(WozModeHGRBuffers_t)));
    WozModeTextBuffers = static_cast<WozModeTextBuffers_t*>(reserve(videoMemory, size, 2 * sizeof(WozModeTextBuffers_t)));
    NTSCBlack = black;
    NTSCWhite = white;

    return 1; // XXX should return 0 here if memory insufficient
}

void start(bool, bool, bool, bool)
{
    RoNTSCSetMode(0, WozModeInitVideoMemory, WozModeFillRowBuffer, WozModeNeedsColorburst);
    RoAudioGetSamplingInfo(&audioSampleRate, &audioChunkLengthBytes);
    event_queue.push_back({KEYDOWN, CAPS_LOCK});
}

void apply_writes(void);

const std::map<int, int> HIDkeyToInterfaceKey = 
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

void ProcessKey(int press, int key)
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

void poll_events()
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

extern "C" { 
void enqueue_ascii(int key);
}

const std::unordered_map<int, int> must_shift = {
    {'!', '1'},
    {'@', '2'},
    {'#', '3'},
    {'$', '4'},
    {'%', '5'},
    {'^', '6'},
    {'&', '7'},
    {'*', '8'},
    {'(', '9'},
    {')', '0'},
    {'_', '-'},
    {'+', '='},
    {'{', '['},
    {'}', ']'},
    {'|', '\\'},
    {':', ';'},
    {'"', '\''},
    {'<', ','},
    {'>', '.'},
    {'?', '/'},
    {'~', '`'},
};

void enqueue_ascii(int key)
{
    if((key == '\n') || (key == '\r')) {
        event_queue.push_back({KEYDOWN, ENTER});
        event_queue.push_back({KEYUP, ENTER});
    } else if((key >= 'a') && (key <= 'z')) {
        event_queue.push_back({KEYDOWN, LEFT_SHIFT});
        event_queue.push_back({KEYDOWN, key - 'a' + 'A'});
        event_queue.push_back({KEYUP, key - 'a' + 'A'});
        event_queue.push_back({KEYUP, LEFT_SHIFT});
    } else if((key >= 'A') && (key <= 'Z')) {
        event_queue.push_back({KEYDOWN, LEFT_SHIFT});
        event_queue.push_back({KEYDOWN, key});
        event_queue.push_back({KEYUP, key});
        event_queue.push_back({KEYUP, LEFT_SHIFT});
    } else if(must_shift.count(key) > 0) {
        event_queue.push_back({KEYDOWN, LEFT_SHIFT});
        event_queue.push_back({KEYDOWN, must_shift.at(key)});
        event_queue.push_back({KEYUP, must_shift.at(key)});
        event_queue.push_back({KEYUP, LEFT_SHIFT});
    } else if((key >= ' ') && (key <= '`')) {
        event_queue.push_back({KEYDOWN, key});
        event_queue.push_back({KEYUP, key});
    }
}

void map_history_to_lines(const ModeHistory& history, [[maybe_unused]] unsigned long long current_byte)
{
#if 0
    for(const auto& modePoint: history) {
#else
    // We don't have per-line modes yet, so just update to the
    // last mode setting for speed.
    if(history.size() > 0) {
        const auto& modePoint = history.back();
#endif
        const auto& [when, lastMode] = modePoint;
        (void)when;
        WozModeDisplayMode = lastMode.mode;
        WozModeMixed = lastMode.mixed;
        WozModePage = lastMode.page;
        WozModeVid80 = lastMode.vid80;
        WozModeDHGR = lastMode.dhgr;
    }
}

void iterate(const ModeHistory& history, unsigned long long current_byte, [[maybe_unused]] float megahertz)
{
    apply_writes();

    map_history_to_lines(history, current_byte);

    poll_events();
}

void shutdown()
{
}

typedef pair<int, bool> address_auxpage;
std::map<address_auxpage, uint8_t> writes;
int collisions = 0;

std::tuple<uint16_t, uint16_t> wozAddressToHGRBufferAddress(uint16_t wozAddress)
{
    uint16_t part1 = (wozAddress & 0x1C00) >>  10;
    uint16_t part2 = (wozAddress & 0x0380) >>   7;
    uint16_t part3 = (wozAddress & 0x007F) / 0x28;
    uint16_t byte = (wozAddress & 0x007F) % 0x28;

    uint16_t row = part3 * 64 + part2 * 8 + part1;
    uint16_t bufferAddress = row * 40 + byte;

    return std::make_tuple(row, bufferAddress);
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


void write2(int addr, bool aux, uint8_t data)
{
    // We know text page 1 and 2 are contiguous
    if((addr >= text_page1_base) && (addr < text_page2_base + text_page_size)) {

        int page = (addr >= text_page2_base) ? 1 : 0;
        size_t wozAddress = addr - text_page1_base - page * text_page_size;
        uint16_t within_page;
        std::tie(std::ignore, within_page) = wozAddressToTextBufferAddress(wozAddress);
        WozModeTextBuffers[aux ? 1 : 0][page][within_page] = data;

    } else if(((addr >= hires_page1_base) && (addr < hires_page1_base + hires_page_size)) || ((addr >= hires_page2_base) && (addr < hires_page2_base + hires_page_size))) {

        int page = (addr < hires_page2_base) ? 0 : 1;
        uint16_t wozAddress = addr - hires_page1_base - page * hires_page_size;
        uint16_t within_page;
        std::tie(std::ignore, within_page) = wozAddressToHGRBufferAddress(wozAddress);
        WozModeHGRBuffers[aux ? 1 : 0][page][within_page] = data;
    }
}

void apply_writes(void)
{
    for(auto it : writes) {
        int addr;
        bool aux;
        tie(addr, aux) = it.first;
        write2(addr, aux, it.second); 
    }
    writes.clear();
    collisions = 0;
}

bool write(uint16_t addr, bool aux, uint8_t data)
{
    // We know text page 1 and 2 are contiguous
    if((addr >= text_page1_base) && (addr < text_page2_base + text_page_size)) {

        if(writes.find({addr, aux}) != writes.end())
            collisions++;
        writes[{addr, aux}] = data;
        if(writes.size() > 1000) {
            apply_writes();
        }
        return true;

    } else if(((addr >= hires_page1_base) && (addr < hires_page1_base + hires_page_size)) || ((addr >= hires_page2_base) && (addr < hires_page2_base + hires_page_size))) {

        if(writes.find({addr, aux}) != writes.end())
            collisions++;
        writes[{addr, aux}] = data;
        if(writes.size() > 1000) {
            apply_writes();
        }
        return true;
    }
    return false;
}

void show_floppy_activity([[maybe_unused]] int number, bool activity)
{
    if(activity) {
        // RoLEDSet(1 + number, 255, 0, 0);
    } else {
        // RoLEDSet(1 + number, 0, 0, 0);
    }
}

int get_audio_sample_rate()
{
    return (int)audioSampleRate;
}

size_t get_preferred_audio_buffer_size_samples()
{
    // Divide by 2 because Rocinante alternates between blocking at beginning and middle
    return audioChunkLengthBytes / 2;
}

bool audio_needs_start = true;

void enqueue_audio_samples(uint8_t *buf, size_t count)
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
        RoAudioEnqueueSamplesBlocking(count * 2, lead_in);
        audio_needs_start = false;
    }
    (void)RoAudioEnqueueSamplesBlocking(count * 2, buf);
}

};
