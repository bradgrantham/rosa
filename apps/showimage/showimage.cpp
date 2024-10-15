#include <cstdio>
#include <string>
#include <vector>
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
    LauncherRegisterApp("Image Viewer", "showimage", "an image file", "images", ".ppm", {}, {}, showimage_main);
    return 1;
}();

#endif

//----------------------------------------------------------------------------
// 4-bit 352x384 pixmap mode

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

[[maybe_unused]] const static unsigned char RGBFor4BitPalette[][3] = {
    // From Arne's 16-color general purpose palette
     {0, 0, 0},
     {157, 157, 157},
     {255, 255, 255},
     {190, 38, 51},
     {224, 111, 139},
     {73, 60, 43},
     {164, 100, 34},
     {235, 137, 49},
     {247, 226, 107},
     {47, 72, 78},
     {68, 137, 26},
     {163, 206, 39},
     {27, 38, 50},
     {0, 87, 132},
     {49, 162, 242},
     {178, 220, 239},
     {255, 0, 255},
};

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
            case 5: *r = v; *g = p; *b = q; break;
        }
    }
}

void Set8BitVideoMode()
{
    for(int i = 0; i < 256; i++)
    {
        // H3S2V3
        float h = ((i >> 5) & 7) / 7.0f * M_PI * 2;
        float s = ((i >> 3) & 3) / 3.0f;
        float v = ((i >> 0) & 7) / 7.0f;
        float r, g, b;
        HSVToRGB3f(h, s, v, &r, &g, &b);
        Video8BitSetPaletteEntry(i, r, g, b);
    }
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

enum {
    MAX_SCREEN_ROW_PIXELS = 1024,
    MAX_FILE_ROW_PIXELS = 4096,
};

int FindClosestColor(unsigned char palette[][3], int paletteSize, int r, int g, int b)
{
#if 0
    float bestDiff = 200000;  // skosh above the maximum difference, 3 * 65536
    int c = -1;

    for(int i = 0; i < paletteSize; i++) {

        float pr = (unsigned int)palette[i][0];
        float pg = (unsigned int)palette[i][1];
        float pb = (unsigned int)palette[i][2];
        float diff = (pr - r) * (pr - r) + (pg - g) * (pg - g) + (pb - b) * (pb - b);
        if(diff == 0) {
            return i;
        }
        if(diff < bestDiff) {
            bestDiff = diff;
            c = i;
        }
    }
#else
    int bestDiff = 200000;  // skosh above the maximum difference, 3 * 65536
    int c = -1;

    for(int i = 0; i < paletteSize; i++) {

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
#endif
    return c;
}


extern "C" {

int main([[maybe_unused]] int argc, const char **argv)
{
    [[maybe_unused]] const char *filename;

    filename = argv[1];

    Set8BitVideoMode();
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

    int paletteSize = 256;
    unsigned char (*palette)[3] = (unsigned char (*)[3])malloc(sizeof(palette[0]) * paletteSize);
    if(palette == NULL) {
        printf("failed to allocate palette\n");
        exit(1);
    }
    for(int i = 0; i < paletteSize; i++)
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

    FILE *fp;
    fp = fopen (filename, "rb");
    if(fp == NULL) {
        printf("ERROR: couldn't open \"%s\" for reading, errno %d\n", filename, errno);
        exit(1);
    }

    int ppmtype, max, width, height;

    if(fscanf(fp, "P%d %d %d %d ", &ppmtype, &width, &height, &max) != 4) {
        printf("couldn't read PPM header from \"%s\"\n", filename);
        fclose(fp);
        exit(1);
    }

    if((ppmtype != 5) && (ppmtype != 6)) {
        printf("unsupported image type %d for \"%s\"\n", ppmtype, filename);
        free(palette);
        fclose(fp);
        exit(1);
    }

    if(width > MAX_FILE_ROW_PIXELS) {
	printf("ERROR: width %d of image in \"%s\" is too large for static row of %u pixels\n",
            width, filename, MAX_SCREEN_ROW_PIXELS);
        free(palette);
        fclose(fp);
        exit(1);
    }

    static unsigned char (*rowRGB)[3];
    rowRGB = (unsigned char (*)[3]) malloc(sizeof(rowRGB[0]) * MAX_FILE_ROW_PIXELS);
    if(rowRGB == NULL) {
        printf("failed to allocate row for pixel data\n");
        free(palette);
        fclose(fp);
        exit(1);
    }

    signed short (*rowError)[MAX_SCREEN_ROW_PIXELS][3]; // + 1 in either direction
    int currentErrorRow = 0;
    rowError = (signed short (*)[MAX_SCREEN_ROW_PIXELS][3])malloc(sizeof(rowError[0]) * 2);
    if(rowError == NULL) {
        printf("failed to allocate row for error data\n");
        free(rowRGB);
        free(palette);
        fclose(fp);
        exit(1);
    }
    memset(rowError, 0, sizeof(rowError[0]) * 2);

    int prevImageRow = -1;
    for(int y = 0; y < VIDEO_8_BIT_MODE_HEIGHT; y++)
    {
        int srcRow = y * height / VIDEO_8_BIT_MODE_HEIGHT;

        while (prevImageRow < srcRow) {
            if(ppmtype == 6)
            {
                if(fread(rowRGB, 3, width, fp) != (size_t)width)
                {
                    printf("ERROR: couldn't read row %d from \"%s\"\n", srcRow, filename);
                    free(palette);
                    free(rowError);
                    free(rowRGB);
                    exit(1);
                }
            }
            else if(ppmtype == 5)
            {
                if(fread(rowRGB, 1, width, fp) != (size_t)width)
                {
                    printf("ERROR: couldn't read row %d from \"%s\"\n", srcRow, filename);
                    free(palette);
                    free(rowError);
                    free(rowRGB);
                    exit(1);
                }
                // expand P5 row to P6 RGB
                for(int i = 0; i < width; i++)
                {
                    int x = width - 1 - i;
                    unsigned char gray = ((unsigned char *)rowRGB)[x];
                    rowRGB[x][0] = gray;
                    rowRGB[x][1] = gray;
                    rowRGB[x][2] = gray;
                }
            }
            prevImageRow++;
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
            int c = FindClosestColor(palette, paletteSize, correctedRGB[0], correctedRGB[1], correctedRGB[2]);

            SetPixel(x, y, c);

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
    free(palette);
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
