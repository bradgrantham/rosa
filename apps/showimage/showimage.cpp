#include <cstdio>
#include <string>
#include <vector>
#include <array>
#include <cstring>
#include "rocinante.h"
#include "events.h"
#include "ui.h"
#include "../launcher/launcher.h"

#if defined(ROSA)

extern "C" {
int showimage_main(int argc, const char **argv);
};

#define main showimage_main

static int initializer = []() -> int {
    LauncherRegisterApp("Image Viewer", "showimage", "Choose An Image File", "images", ".bmp", {}, {}, showimage_main);
    return 1;
}();

#endif

//----------------------------------------------------------------------------
// Video mode support

void HSVToRGB3f(float h, float s, float v, float *r, float *g, float *b)
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
            default: case 5: *r = v; *g = p; *b = q; break;
        }
    }
}


//----------------------------------------------------------------------------
// 8-bit 352x384 pixmap mode

#define VIDEO_8_BIT_MODE_WIDTH_SAMPLES 704
#define VIDEO_8_BIT_MODE_WIDTH_PIXELS (VIDEO_8_BIT_MODE_WIDTH_SAMPLES / 2)
#define VIDEO_8_BIT_MODE_ROWBYTES (VIDEO_8_BIT_MODE_WIDTH_PIXELS)
#define VIDEO_8_BIT_MODE_LEFT ((704 - VIDEO_8_BIT_MODE_WIDTH_SAMPLES) / 2) 
#define VIDEO_8_BIT_MODE_HEIGHT 480 
#define VIDEO_8_BIT_MODE_TOP (480 / 2 - VIDEO_8_BIT_MODE_HEIGHT / 2)

uint8_t *Video8BitFramebuffer;

uint8_t Video8BitColorsToNTSC[256][4];

static uint8_t Video8BitModeBlack;
static uint8_t Video8BitModeWhite;

void Video8BitSetPaletteEntry(int color, uint8_t r, uint8_t g, uint8_t b)
{
    float y, i, q;
    RoRGBToYIQ(r / 255.0f, g / 255.0f, b / 255.0f, &y, &i, &q);

    for(int phase = 0; phase < 4; phase++) {
        Video8BitColorsToNTSC[color][phase] = RoNTSCYIQToDAC(y, i, q, phase / 4.0f, Video8BitModeBlack, Video8BitModeWhite);
    }
}

void Video8BitSetPaletteEntry(int color, float r, float g, float b)
{
    float y, i, q;
    RoRGBToYIQ(r, g, b, &y, &i, &q);

    for(int phase = 0; phase < 4; phase++) {
        Video8BitColorsToNTSC[color][phase] = RoNTSCYIQToDAC(y, i, q, phase / 4.0f, Video8BitModeBlack, Video8BitModeWhite);
    }
}

uint8_t Video8BitGetColorIndex(int x, uint8_t *rowColors)
{
    return rowColors[x];
}

int Video8BitModeNeedsColorburst()
{
    return 1;
}

void Video8BitConvertPaletteToNTSCColors(const std::array<std::array<uint8_t, 3>, 256>& palette)
{
    for(int i = 0; i < 256; i++)
    {
        Video8BitSetPaletteEntry(i, palette[i][0], palette[i][1], palette[i][2]);
    }
}

void Video8BitMakePaletteBlack()
{
    for(int i = 0; i < 256; i++)
    {
        Video8BitColorsToNTSC[i][0] = Video8BitModeBlack;
        Video8BitColorsToNTSC[i][1] = Video8BitModeBlack;
        Video8BitColorsToNTSC[i][2] = Video8BitModeBlack;
        Video8BitColorsToNTSC[i][3] = Video8BitModeBlack;
    }
}

static int Video8BitModeInit([[maybe_unused]] void *private_data, uint8_t black_, uint8_t white_)
{
    Video8BitModeBlack = black_;
    Video8BitModeWhite = white_;
    Video8BitFramebuffer = new(std::nothrow) uint8_t[VIDEO_8_BIT_MODE_ROWBYTES * VIDEO_8_BIT_MODE_HEIGHT];
    if(Video8BitFramebuffer == nullptr)
    {
        // out of memory
        return 0;
    }
    Video8BitMakePaletteBlack();
    return 1;
}

static void Video8BitModeFini([[maybe_unused]] void *private_data)
{
    delete[] Video8BitFramebuffer;
}

__attribute__((hot,flatten)) void Video8BitModeFillRowBuffer([[maybe_unused]] int frameIndex, [[maybe_unused]] int lineWithinField, int rowNumber, [[maybe_unused]] size_t maxSamples, uint8_t* rowBuffer)
{
    if((rowNumber >= VIDEO_8_BIT_MODE_TOP) && (rowNumber < VIDEO_8_BIT_MODE_HEIGHT))
    {
        int rowIndex = rowNumber - VIDEO_8_BIT_MODE_TOP;
        uint8_t* rowColors = Video8BitFramebuffer + rowIndex * VIDEO_8_BIT_MODE_ROWBYTES;

        // convert rowColors to NTSC waveform into rowDst 2 samples at a time.
        rowBuffer += VIDEO_8_BIT_MODE_LEFT;

        for(int i = 0; i < VIDEO_8_BIT_MODE_WIDTH_PIXELS; i += 2)
        {
            uint8_t fb_byte = *rowColors++;

            uint8_t *color = Video8BitColorsToNTSC[fb_byte];
            *rowBuffer++ = color[0];
            *rowBuffer++ = color[1];

            fb_byte = *rowColors++;
            color = Video8BitColorsToNTSC[fb_byte];
            *rowBuffer++ = color[2];
            *rowBuffer++ = color[3];
        }
    }
}

void Set8BitVideoMode()
{
    RoVideoSetMode(1, RO_VIDEO_ROW_SAMPLES_912, nullptr, Video8BitModeInit, Video8BitModeFini, Video8BitModeFillRowBuffer, Video8BitModeNeedsColorburst);
}

int SetPixel(int x, int y, int c)
{
    uint8_t *byte = Video8BitFramebuffer + y * VIDEO_8_BIT_MODE_ROWBYTES + x;
    *byte = c;
    return 1;
}

void ClearScreen(int c)
{
    for(int y = 0; y < VIDEO_8_BIT_MODE_HEIGHT; y++)
    {
        for(int x = 0; x < VIDEO_8_BIT_MODE_WIDTH_PIXELS; x++)
        {
            SetPixel(x, y, c);
        }
    }
}

void MakeHSVPalette(std::array<std::array<uint8_t, 3>, 256>& palette)
{
    for(int i = 0; i < 256; i++)
    {
        // H3S2V3
        float h = ((i >> 5) & 7) / 7.0f * M_PI * 2;
        float s = ((i >> 3) & 3) / 3.0f;
        float v = ((i >> 0) & 7) / 7.0f;
        float r, g, b;
        HSVToRGB3f(h, s, v, &r, &g, &b);
        palette[i][0] = r * 255;
        palette[i][1] = g * 255;
        palette[i][2] = b * 255;
    }
}


enum {
    MAX_SCREEN_ROW_PIXELS = 1024,
    MAX_FILE_ROW_PIXELS = 4096,
};

int FindClosestColor(const std::array<std::array<uint8_t, 3>, 256> & palette, int r, int g, int b)
{
    int bestDiff = 200000;  // skosh above the maximum difference, 3 * 65536
    int c = -1;

    for(size_t i = 0; i < palette.size(); i++) {

        int pr = (unsigned int)palette[i][0];
        int pg = (unsigned int)palette[i][1];
        int pb = (unsigned int)palette[i][2];
        int diff = ((pr - r) * (pr - r) + (pg - g) * (pg - g) + (pb - b) * (pb - b));
        if(diff == 0) {
            return i;
        }
        if(diff < bestDiff) {
            bestDiff = diff;
            c = i;
        }
    }
    return c;
}

#pragma pack(push, 1)
// BMP File Header structure
struct BMPFileHeader {
    uint16_t signature;      // 'BM' signature
    uint32_t fileSize;       // Size of the BMP file in bytes
    uint16_t reserved1;      // Reserved
    uint16_t reserved2;      // Reserved
    uint32_t dataOffset;     // Offset to the beginning of the pixel data
};

// DIB Header structure (Windows BITMAPINFOHEADER)
struct BMPInfoHeader {
    uint32_t headerSize;     // Size of this header in bytes (40 bytes)
    int32_t  width;          // Width of the image in pixels
    int32_t  height;         // Height of the image in pixels
    uint16_t planes;         // Number of color planes (must be 1)
    uint16_t bitsPerPixel;   // Bits per pixel (we're targeting 8-bit)
    uint32_t compression;    // Compression method (0 = none)
    uint32_t imageSize;      // Size of the raw bitmap data
    int32_t  xPixelsPerMeter; // Horizontal resolution in pixels per meter
    int32_t  yPixelsPerMeter; // Vertical resolution in pixels per meter
    uint32_t colorsUsed;     // Number of colors in the palette
    uint32_t colorsImportant; // Number of important colors (0 means all)
};
#pragma pack(pop)

extern "C" {

std::array<std::array<uint8_t, 3>, 256> palette;

int main([[maybe_unused]] int argc, const char **argv)
{
    [[maybe_unused]] const char *filename;

    filename = argv[1];

    Set8BitVideoMode();

    MakeHSVPalette(palette);
    Video8BitConvertPaletteToNTSCColors(palette);
    ClearScreen(1);

    if(true)
    {
        for(int y = 0; y < VIDEO_8_BIT_MODE_HEIGHT; y++)
        {
            for(int x = 0; x < VIDEO_8_BIT_MODE_WIDTH_PIXELS; x++)
            {
                int c = x * 256 / VIDEO_8_BIT_MODE_WIDTH_PIXELS;
                SetPixel(x, y, c);
            }
        }
    }

    FILE *fp;
    fp = fopen (filename, "rb");
    if(fp == NULL) {
        printf("ERROR: couldn't open \"%s\" for reading, errno %d\n", filename, errno);
        return 1;
    }

    // Read BMP file header
    static BMPFileHeader fileHeader;
    if(fread(&fileHeader, sizeof(BMPFileHeader), 1, fp) != 1) {
        printf("ERROR: couldn't read BMP file header from \"%s\"\n", filename);
        fclose(fp);
        return 1;
    }

    // Verify BMP signature ('BM')
    if(fileHeader.signature != 0x4D42) { // 'BM' in little endian
        printf("ERROR: invalid BMP signature in \"%s\"\n", filename);
        fclose(fp);
        return 1;
    }

    // Read BMP info header
    static BMPInfoHeader infoHeader;
    if(fread(&infoHeader, sizeof(BMPInfoHeader), 1, fp) != 1) {
        printf("ERROR: couldn't read BMP info header from \"%s\"\n", filename);
        fclose(fp);
        return 1;
    }

    // Validate BMP format
    if(infoHeader.bitsPerPixel != 8) {
        printf("ERROR: only 8-bit BMP files are supported, \"%s\" is %d-bit\n",
               filename, infoHeader.bitsPerPixel);
        fclose(fp);
        return 1;
    }

    if(infoHeader.compression != 0) {
        printf("ERROR: compressed BMP files are not supported in \"%s\"\n", filename);
        fclose(fp);
        return 1;
    }

    if(abs(infoHeader.width) > MAX_FILE_ROW_PIXELS) {
        printf("ERROR: width %d of image in \"%s\" is too large for static row of %u pixels\n",
               abs(infoHeader.width), filename, MAX_SCREEN_ROW_PIXELS);
        fclose(fp);
        return 1;
    }

    // Get the number of colors in the palette
    uint32_t numColors = (infoHeader.colorsUsed == 0) ? 256 : infoHeader.colorsUsed;
    if(numColors > 256) {
        printf("ERROR: BMP file \"%s\" has more than 256 colors\n", filename);
        fclose(fp);
        return 1;
    }

    // Read the color palette (BGRA format in BMP)
    static uint8_t bmpPalette[256][4]; // BMP palette uses BGRA format
    if(fread(bmpPalette, 4, numColors, fp) != numColors) {
        printf("ERROR: couldn't read color palette from \"%s\"\n", filename);
        fclose(fp);
        return 1;
    }

    // Convert BMP palette (BGRA) to RGB format
    for(uint32_t i = 0; i < numColors; i++) {
        palette[i][0] = bmpPalette[i][2]; // Red (BMP stores BGR)
        palette[i][1] = bmpPalette[i][1]; // Green
        palette[i][2] = bmpPalette[i][0]; // Blue
    }

    // Apply the NTSC conversion to the palette
    Video8BitConvertPaletteToNTSCColors(palette);

    // Calculate image dimensions
    int width = abs(infoHeader.width);
    int height = abs(infoHeader.height);
    bool isBottomUp = infoHeader.height > 0;

    // Calculate row padding - BMP rows are padded to 4-byte boundaries
    int rowPadding = (4 - (width % 4)) % 4;
    int rowSize = width + rowPadding;

    printf("%d by %d, isBottomUp %d, %d rowSize, %d offset\n", width, height, isBottomUp, rowSize, fileHeader.dataOffset);

    static unsigned char (*rowRGB)[3];
    rowRGB = (unsigned char (*)[3]) malloc(sizeof(rowRGB[0]) * MAX_FILE_ROW_PIXELS);
    if(rowRGB == NULL) {
        printf("failed to allocate row for pixel data\n");
        fclose(fp);
        return 1;
    }

    signed short (*rowError)[MAX_SCREEN_ROW_PIXELS][3]; // + 1 in either direction
    int currentErrorRow = 0;
    rowError = (signed short (*)[MAX_SCREEN_ROW_PIXELS][3])malloc(sizeof(rowError[0]) * 2);
    if(rowError == NULL)
    {
        printf("failed to allocate row for error data\n");
        free(rowRGB);
        fclose(fp);
        return 1;
    }
    memset(rowError, 0, sizeof(rowError[0]) * 2);

    // Allocate memory for row data and error diffusion
    uint8_t* rowData = (uint8_t*)malloc(rowSize);
    if(rowData == NULL) {
        printf("ERROR: failed to allocate row for pixel data\n");
        free(rowRGB);
        free(rowError);
        fclose(fp);
        return 1;
    }

    if(fseek(fp, fileHeader.dataOffset, SEEK_SET) != 0)
    {
        printf("ERROR: couldn't seek to image data in \"%s\"\n", filename);
        free(rowRGB);
        free(rowError);
        free(rowData);
        fclose(fp);
        return 1;
    }

    int prevImageRowDelta = isBottomUp ? -1 : 1;
    int prevImageRow = isBottomUp ? height : -1;

    for(int y = 0; y < VIDEO_8_BIT_MODE_HEIGHT; y++)
    {
        int srcRow = (isBottomUp ? (VIDEO_8_BIT_MODE_HEIGHT - y - 1) : y) * height / VIDEO_8_BIT_MODE_HEIGHT;

        while (prevImageRow != srcRow)
        {
            printf("read row %d\n", prevImageRow);
            size_t wasRead = fread(rowData, 1, rowSize, fp);
            if(wasRead != (size_t)rowSize)
            {
                printf("ERROR: couldn't read row %d from \"%s\", got %zd bytes\n", prevImageRow, filename, wasRead);
                free(rowError);
                free(rowRGB);
                return 1;
            }
            prevImageRow += prevImageRowDelta;
        }

        for(int i = 0; i < width; i++) {
            rowRGB[i][0] = palette[rowData[i]][0];
            rowRGB[i][1] = palette[rowData[i]][1];
            rowRGB[i][2] = palette[rowData[i]][2];
        }

        short (*errorThisRowFixed8)[3] = rowError[currentErrorRow] + 1; // So we can access -1 without bounds check

        int nextErrorRow = (currentErrorRow + 1) % 2;
        memset(rowError[nextErrorRow], 0, sizeof(rowError[0]));
        short (*errorNextRowFixed8)[3] = rowError[nextErrorRow] + 1;   // So we can access -1 without bounds check

        for(int x = 0; x < VIDEO_8_BIT_MODE_WIDTH_PIXELS; x++) {
            int srcCol = (x * width + width - 1) / VIDEO_8_BIT_MODE_WIDTH_PIXELS;

            // get the color with error diffused from previous pixels
            int correctedRGB[3];
            for(int i = 0; i < 3; i++) {
                 correctedRGB[i] = rowRGB[srcCol][i] + errorThisRowFixed8[x][i] / 256;
            }

            // Find the closest color in our palette
            int c = FindClosestColor(palette, correctedRGB[0], correctedRGB[1], correctedRGB[2]);

            SetPixel(x, isBottomUp ? (VIDEO_8_BIT_MODE_HEIGHT - y - 1) : y, c);

            // Calculate our error between what we wanted and what we got
            // and distribute it a la Floyd-Steinberg
            int errorFixed8[3];
            for(int i = 0; i < 3; i++) {
                errorFixed8[i] = 255 * (correctedRGB[i] - palette[c][i]);
            }
            for(int i = 0; i < 3; i++) {
                errorThisRowFixed8[x + 1][i] += errorFixed8[i] * 7 / 16;
            }
            for(int i = 0; i < 3; i++) {
                errorNextRowFixed8[x - 1][i] += errorFixed8[i] * 3 / 16;
            }
            for(int i = 0; i < 3; i++) {
                errorNextRowFixed8[x    ][i] += errorFixed8[i] * 5 / 16;
            }
            for(int i = 0; i < 3; i++) {
                errorNextRowFixed8[x + 1][i] += errorFixed8[i] * 1 / 16;
            }

            currentErrorRow = nextErrorRow;
        }
    }

    fclose(fp);
    free(rowError);
    free(rowRGB);

    uint32_t prevTick;
    prevTick = RoGetMillis();

    bool quit = false;
    do
    {
        uint32_t nowTick = RoGetMillis();
        // Setting this to + 16 made USB keyboard stop working.  
        if(nowTick >= prevTick + 10) {
            RoDoHousekeeping();
            prevTick = nowTick;
        }
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
                case RoEvent::CONSOLE_BUTTONPRESS: {
                    const ButtonPressEvent& press = ev.u.buttonPress;
                    if(press.button == 2) {
                        quit = true;
                    }
                }

                default:
                    // pass;
                    break;
            }
        }

    } while(!quit);

    return 0;
}

};
