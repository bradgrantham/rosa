#include <new>
#include <cstring>
#include <cassert>
#include "rocinante.h"
#include "text-mode.h"
#include "8x16.h"

//----------------------------------------------------------------------------
// Text Mode

#define TextModeLeftTick 64
#define TextModeTopRow 10
#define TextModeFontWidthScale 2
#define TextModeCharGapPixels 1
#define TextModeFontHeightScale 1
#define TextModeWidth (604 / (font8x16Width * TextModeFontWidthScale + TextModeCharGapPixels))
#define TextModeHeight ((218 - 6) / font8x16Height)

static uint8_t *TextModeAttributes;
static char *TextModeBuffer;

static uint8_t NTSCBlack, NTSCWhite;

void RoTextModeClearDisplay(void)
{
    memset(TextModeBuffer, ' ', TextModeWidth * TextModeHeight);
    memset(TextModeAttributes, TEXT_NO_ATTRIBUTES, TextModeWidth * TextModeHeight);
}

static int RoTextModeInit([[maybe_unused]] void *private_data, uint8_t black, uint8_t white)
{
    TextModeAttributes = new(std::nothrow) uint8_t[TextModeHeight * TextModeWidth];
    if(TextModeAttributes == nullptr)
    {
        // out of memory
        return 0;
    }
    TextModeBuffer = new (std::nothrow) char[TextModeHeight * TextModeWidth];
    if(TextModeBuffer == nullptr)
    {
        delete[] TextModeAttributes;
        // out of memory
        return 0;
    }
    NTSCBlack = black;
    NTSCWhite = white;
    RoTextModeClearDisplay();

    return 1;
}

static void RoTextModeFini([[maybe_unused]] void *private_data)
{
    delete[] TextModeAttributes;
    delete[] TextModeBuffer;
}

__attribute__((hot,flatten)) void RoTextModeFillRowBuffer([[maybe_unused]] int frameIndex, [[maybe_unused]] int lineWithinField, int rowNumber, [[maybe_unused]] size_t maxSamples, uint8_t* rowBuffer)
{
    int fontScanlineHeight = font8x16Height * TextModeFontHeightScale;

    int rowWithinTextArea = rowNumber - TextModeTopRow;

    int charRow = rowWithinTextArea / fontScanlineHeight;
    int charPixelY = (rowWithinTextArea % fontScanlineHeight) / TextModeFontHeightScale;

// XXX this code assumes font width <= 8 and each row padded out to a byte
    if((rowWithinTextArea >= 0) && (charRow < TextModeHeight)) {

        uint8_t *rowDst = rowBuffer + TextModeLeftTick;

        for(int charCol = 0; charCol < TextModeWidth; charCol++) {

            uint8_t character = TextModeBuffer[charRow * TextModeWidth + charCol];
            int inverse = (TextModeAttributes[charRow * TextModeWidth + charCol] & TEXT_INVERSE) ? 0xFF : 0;
            uint8_t charRowBits = font8x16Bits[character * font8x16Height + charPixelY] ^ inverse;

            for(int charPixelX = 0; charPixelX < font8x16Width; charPixelX++) {

                int pixel = (charRowBits & (0x80 >> charPixelX));

                for(int col = 0; col < TextModeFontWidthScale; col++) {
                    *rowDst++ = pixel ? NTSCWhite : NTSCBlack;
                }
            }
        }
    }
}

void RoTextModeGetSize(int *w, int *h)
{
    *w = TextModeWidth;
    *h = TextModeHeight;
}

int RoTextModeNeedsColorburst(void)
{
    return 0;
}

void RoTextModeClearArea(int column, int w, int row, int h, uint8_t attributes)
{
    for(int y = row; y < row + h; y++) {
        memset(TextModeBuffer + y * TextModeWidth + column, ' ', w);
        memset(TextModeAttributes + y * TextModeWidth + column, attributes, w);
    }
}

void RoTextModeSetLine(int row, int column, uint8_t attributes, const char *string)
{
    int stringExceedsWidth = strlen(string) > TextModeWidth - static_cast<size_t>(column);
    size_t toCopy = stringExceedsWidth ? TextModeWidth - column : strlen(string);
    memcpy(TextModeBuffer + row * TextModeWidth + column, string, toCopy);
    memset(TextModeAttributes + row * TextModeWidth + column, attributes, toCopy);
}

void RoTextMode(void)
{
    RoVideoSetMode(0, RO_VIDEO_ROW_SAMPLES_912, nullptr, RoTextModeInit, RoTextModeFini, RoTextModeFillRowBuffer, RoTextModeNeedsColorburst);
}
