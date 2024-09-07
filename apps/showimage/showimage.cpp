#include <cstdio>
#include <string>
#include <vector>
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

#define VIDEO_4_BIT_MODE_WIDTH_SAMPLES 704
#define VIDEO_4_BIT_MODE_WIDTH_PIXELS (VIDEO_4_BIT_MODE_WIDTH_SAMPLES / 2)
#define VIDEO_4_BIT_MODE_ROWBYTES (VIDEO_4_BIT_MODE_WIDTH_PIXELS / 2)
#define VIDEO_4_BIT_MODE_LEFT ((704 - VIDEO_4_BIT_MODE_WIDTH_SAMPLES) / 2) 
#define VIDEO_4_BIT_MODE_HEIGHT 480 
#define VIDEO_4_BIT_MODE_TOP (480 / 2 - VIDEO_4_BIT_MODE_HEIGHT / 2)

uint8_t *Video4BitFramebuffer;

uint8_t Video4BitColorsToNTSC[16][4];

void Video4BitSetPaletteEntry(int color, uint8_t r, uint8_t g, uint8_t b)
{
    float y, i, q;
    RoRGBToYIQ(r / 255.0f, g / 255.0f, b / 255.0f, &y, &i, &q);

    for(int phase = 0; phase < 4; phase++) {
        Video4BitColorsToNTSC[color][phase] = RoNTSCYIQToDAC(y, i, q, phase / 4.0);
    }
}

uint8_t Video4BitGetColorIndex(int x, uint8_t *rowColors)
{
    if((x & 0b1) == 0) {
        return rowColors[x / 2] & 0xF;
    } else {
        return (rowColors[x / 2] & 0xF0) >> 4;
    }
}

int Video4BitModeNeedsColorburst()
{
    return 1;
}

static int Video4BitModeInit([[maybe_unused]] void *private_data, uint8_t, uint8_t)
{
    Video4BitFramebuffer = new(std::nothrow) uint8_t[VIDEO_4_BIT_MODE_ROWBYTES * VIDEO_4_BIT_MODE_HEIGHT];
    if(Video4BitFramebuffer == nullptr)
    {
        // out of memory
        return 0;
    }

    return 1;
}

static void Video4BitModeFini([[maybe_unused]] void *private_data)
{
    delete[] Video4BitFramebuffer;
}

__attribute__((hot,flatten)) void Video4BitModeFillRowBuffer([[maybe_unused]] int frameIndex, int rowNumber, [[maybe_unused]] size_t maxSamples, uint8_t* rowBuffer)
{
    if((rowNumber >= VIDEO_4_BIT_MODE_TOP) && (rowNumber < VIDEO_4_BIT_MODE_HEIGHT))
    {
        int rowIndex = rowNumber - VIDEO_4_BIT_MODE_TOP;
        uint8_t* rowColors = Video4BitFramebuffer + rowIndex * VIDEO_4_BIT_MODE_ROWBYTES;

        // convert rowColors to NTSC waveform into rowDst 2 samples at a time.
        rowBuffer += VIDEO_4_BIT_MODE_LEFT;

        // two at a time
        for(int i = 0; i < VIDEO_4_BIT_MODE_WIDTH_PIXELS; i += 2)
        {
            uint8_t fb_byte = *rowColors++;

            uint8_t nybble = fb_byte & 0xF;
            uint8_t *color = Video4BitColorsToNTSC[nybble];
            *rowBuffer++ = color[0];
            *rowBuffer++ = color[1];

            nybble = fb_byte >> 4;
            color = Video4BitColorsToNTSC[nybble];
            *rowBuffer++ = color[2];
            *rowBuffer++ = color[3];
        }
    }
}

const static unsigned char RGBFor4BitPalette[][3] = {
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

void Set4BitVideoMode()
{
    for(int i = 0; i < 16; i++)
    {
        const uint8_t *c = RGBFor4BitPalette[i];
        Video4BitSetPaletteEntry(i, c[0], c[1], c[2]);
    }
    RoNTSCSetMode(1, RO_VIDEO_ROW_SAMPLES_912, nullptr, Video4BitModeInit, Video4BitModeFini, Video4BitModeFillRowBuffer, Video4BitModeNeedsColorburst);
}

int SetPixel(int x, int y, int c)
{
    int whichByte = x / 2;
    int whichNybble = x % 2;
    uint8_t value = c << (whichNybble * 4);
    uint8_t mask = ~(0xF << (whichNybble * 4));
    uint8_t *byte = Video4BitFramebuffer + y * VIDEO_4_BIT_MODE_ROWBYTES + whichByte;
    *byte = (*byte & mask) | value;
    return 1;
}

void ClearScreen(int c)
{
    for(int y = 0; y < VIDEO_4_BIT_MODE_HEIGHT; y++)
    {
        for(int x = 0; x < VIDEO_4_BIT_MODE_WIDTH_PIXELS; x++)
        {
            SetPixel(x, y, c);
        }
    }
}

enum {
    MAX_ROW_SIZE = 4096,
};

int FindClosestColor(unsigned char palette[][3], int paletteSize, int r, int g, int b)
{
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
    return c;
}


extern "C" {

int main([[maybe_unused]] int argc, const char **argv)
{
    [[maybe_unused]] const char *filename;

    filename = argv[1];

    Set4BitVideoMode();
    ClearScreen(1);

    if(false)
    {
        for(int y = 0; y < VIDEO_4_BIT_MODE_HEIGHT; y++)
        {
            for(int x = 0; x < VIDEO_4_BIT_MODE_WIDTH_PIXELS; x++)
            {
                int c = x * 16 / VIDEO_4_BIT_MODE_WIDTH_PIXELS;
                SetPixel(x, y, c);
            }
        }
    }

    unsigned char (*palette)[3] = (unsigned char (*)[3])malloc(sizeof(palette[0]) * 256);
    if(palette == NULL) {
        printf("failed to allocate palette\n");
        exit(1);
    }
    memcpy(palette, RGBFor4BitPalette, sizeof(RGBFor4BitPalette));
    int paletteSize = 16;

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

    if(width > MAX_ROW_SIZE) {
	printf("ERROR: width %d of image in \"%s\" is too large for static row of %u pixels\n",
            width, filename, MAX_ROW_SIZE);
        free(palette);
        fclose(fp);
        exit(1);
    }

    static unsigned char (*rowRGB)[3];
    rowRGB = (unsigned char (*)[3]) malloc(sizeof(rowRGB[0]) * MAX_ROW_SIZE);
    if(rowRGB == NULL) {
        printf("failed to allocate row for pixel data\n");
        free(palette);
        fclose(fp);
        exit(1);
    }

    signed short (*rowError)[MAX_ROW_SIZE][3]; // + 1 in either direction
    int currentErrorRow = 0;
    rowError = (signed short (*)[MAX_ROW_SIZE][3])malloc(sizeof(rowError[0]) * 2);
    memset(rowError, 0, sizeof(rowError[0]) * 2);
    if(rowError == NULL) {
        printf("failed to allocate row for error data\n");
        free(rowRGB);
        free(palette);
        fclose(fp);
        exit(1);
    }

    int prevY = -1;
    for(int srcRow = 0; srcRow < height; srcRow++) {

        if(ppmtype == 6) {
            if(fread(rowRGB, 3, width, fp) != (size_t)width) {
                printf("ERROR: couldn't read row %d from \"%s\"\n", srcRow, filename);
                free(palette);
                free(rowError);
                free(rowRGB);
                exit(1);
            }
        } else if(ppmtype == 5) {
            if(fread(rowRGB, 1, width, fp) != (size_t)width) {
                printf("ERROR: couldn't read row %d from \"%s\"\n", srcRow, filename);
                free(palette);
                free(rowError);
                free(rowRGB);
                exit(1);
            }
            // expand P5 row to P6 RGB
            for(int i = 0; i < width; i++) {
                int x = width - 1 - i;
                unsigned char gray = ((unsigned char *)rowRGB)[x];
                rowRGB[x][0] = gray;
                rowRGB[x][1] = gray;
                rowRGB[x][2] = gray;
            }
        }

        int y = (srcRow * VIDEO_4_BIT_MODE_HEIGHT + VIDEO_4_BIT_MODE_HEIGHT - 1) / height;

        if(y != prevY) {

            if(y >= VIDEO_4_BIT_MODE_HEIGHT) {
                printf("hm, y was >= height, skipped\n");
            }

            short (*errorThisRowFixed8)[3] = rowError[currentErrorRow] + 1; // So we can access -1 without bounds check

            int nextErrorRow = (currentErrorRow + 1) % 2;
            memset(rowError[nextErrorRow], 0, sizeof(rowError[0]));
            short (*errorNextRowFixed8)[3] = rowError[nextErrorRow] + 1;   // So we can access -1 without bounds check

            for(int x = 0; x < VIDEO_4_BIT_MODE_WIDTH_PIXELS; x++) {
                int srcCol = (x * width + width - 1) / VIDEO_4_BIT_MODE_WIDTH_PIXELS;

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
            }
            prevY = y;
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
