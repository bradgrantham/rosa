#include <cstring>
#include "rocinante.h"
#include "events.h"
#include "fonts.h"

extern "C" {
int trs80_main(int argc, char **argv);
};

//----------------------------------------------------------------------------
// Text Mode

#define Trs80ColumnCount 64
#define Trs80RowCount 16
#define Trs80CharWidth 8
#define Trs80CharHeight 12

// TODO check with Brad.
#define NTSCWidth 704
#define NTSCHeight 240

#define Trs80PixelWidth (Trs80ColumnCount*Trs80CharWidth)
#define Trs80PixelHeight (Trs80RowCount*Trs80CharHeight)

#define Trs80HorizontalMargin ((NTSCWidth - Trs80PixelWidth)/2)
#define Trs80VerticalMargin ((NTSCHeight - Trs80PixelHeight)/2)

#define Trs80BlinkPeriodMs 233

static char *TextModeBuffer;

static uint8_t NTSCBlack, NTSCWhite;

void Trs80TextModeClearDisplay()
{
    memset(TextModeBuffer, ' ', Trs80ColumnCount * Trs80RowCount);
}

int Trs80TextModeInitVideoMemory(void *videoMemory, uint32_t size, uint8_t black, uint8_t white)
{
    auto reserve = [](void*& ptr, uint32_t& remaining, size_t rsv) {
        void *old = ptr;
        if (remaining < rsv) {
            // Out of memory.
            return (void *) 0;
        }
        ptr = static_cast<uint8_t*>(ptr) + rsv;
        remaining -= rsv;
        return old;
    };

    TextModeBuffer = static_cast<char*>(reserve(videoMemory, size, Trs80RowCount * Trs80ColumnCount));
    if (TextModeBuffer == 0) {
        // Out of memory.
        return 0;
    }
    NTSCBlack = black;
    NTSCWhite = white;
    Trs80TextModeClearDisplay();

    return 1;
}

__attribute__((hot,flatten)) void Trs80TextModeFillRowBuffer([[maybe_unused]] int frameIndex, int rowNumber, [[maybe_unused]] size_t maxSamples, uint8_t* rowBuffer)
{
    int rowWithinTextArea = rowNumber - Trs80VerticalMargin;
    int charRow = rowWithinTextArea / Trs80FontHeight;

    if (rowWithinTextArea >= 0 && charRow < Trs80RowCount) {
        int charPixelY = (rowWithinTextArea % Trs80FontHeight);
        uint8_t *rowDst = rowBuffer + Trs80HorizontalMargin;

        for (int charCol = 0; charCol < Trs80ColumnCount; charCol++) {
            uint8_t character = TextModeBuffer[charRow * Trs80ColumnCount + charCol];
            uint8_t charRowBits = Trs80FontBits[character * Trs80FontHeight + charPixelY];

            for(int charPixelX = 0; charPixelX < Trs80FontWidth; charPixelX++) {
                int pixel = (charRowBits & (0x01 << charPixelX));
                *rowDst++ = pixel ? NTSCWhite : NTSCBlack;
            }
        }
    }
}

void Trs80TextModeGetSize(int *w, int *h)
{
    *w = Trs80ColumnCount;
    *h = Trs80RowCount;
}

int Trs80TextModeNeedsColorburst()
{
    return 0;
}

void Trs80TextModeClearArea(int column, int w, int row, int h)
{
    for(int y = row; y < row + h; y++) {
        memset(TextModeBuffer + y * Trs80ColumnCount + column, ' ', w);
    }
}

void Trs80TextModeSetLine(int row, int column, const char *string)
{
    int stringExceedsWidth = strlen(string) > Trs80ColumnCount - static_cast<size_t>(column);
    size_t toCopy = stringExceedsWidth ? Trs80ColumnCount - column : strlen(string);
    memcpy(TextModeBuffer + row * Trs80ColumnCount + column, string, toCopy);
}

void Trs80TextMode()
{
    RoNTSCSetMode(0, Trs80TextModeInitVideoMemory, Trs80TextModeFillRowBuffer, Trs80TextModeNeedsColorburst);
}

int trs80_main([[maybe_unused]] int argc, [[maybe_unused]] char **argv)
{
    bool quit = false;

    Trs80TextMode();
    Trs80TextModeClearDisplay();
    Trs80TextModeSetLine(0, 0, "Cass?");
    Trs80TextModeSetLine(1, 0, "Memory Size?");
    Trs80TextModeSetLine(2, 0, "Radio Shack Model III Basic");
    Trs80TextModeSetLine(3, 0, "(c) '80 Tandy");
    Trs80TextModeSetLine(4, 0, "READY");
    Trs80TextModeSetLine(5, 0, ">");
    for (int i = 0; i < 256; i++) {
        TextModeBuffer[1024 - 256 + i] = i;
    }

    while(!quit) {
        RoEvent ev;
        int haveEvent = RoEventPoll(&ev);

        if(haveEvent) {
            switch(ev.eventType) {
                case RoEvent::KEYBOARD_RAW: {
                    const struct KeyboardRawEvent raw = ev.u.keyboardRaw;
                    if(raw.isPress) {
                        quit = true;
                    }
                    break;
                }
                
                default:
                    // pass;
                    break;
            }
        }
        RoDoHousekeeping();

        // Blink cursor.
        uint32_t now = RoGetMillis();
        uint32_t blink = (now / Trs80BlinkPeriodMs) % 2 == 0;
        Trs80TextModeSetLine(5, 1, blink ? "\xB0" : "\x80");
    }

    return 0;
}
