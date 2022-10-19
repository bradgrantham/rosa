# Rocinante ("Rosa" for short) Programming Guide

## The Rosa Emulator

Building `rosa-emu` requires SDL2 development libraries.  On Windows one may have to add a CMake SDL2 development package directory using `-DSDL2_DIR=...` .  See https://trenki2.github.io/blog/2017/06/02/using-sdl2-with-cmake/ for more information.

```bash
git clone --recurse-submodules  https://github.com/bradgrantham/rosa-emu
cd rosa-emu
cmake -Bbuild . # optionally build type etc, e.g. -DCMAKE_BUILD_TYPE=Debug
(cd build ; make)
./rosa-emu
```

Rosa apps run in `rosa-emu` in the current working directory by default.  That's where apps will look for files, ROM and floppy images, etc.  You can specify another directory as the current directory using `--root-dir`.

## Adding an application - Quick Start

Add a directory to `rosa-apps`, e.g. `my-app`.  Make a main file, e.g.  `my-app/my-app.cpp`.  Call your `main` function something else, something unique, like `my_app_main`.

Here's a very simple application that prints "Hello World" in the text mode, waits for a key press, and exits.

```C++
#include "rocinante.h"
#include "events.h"
#include "text-mode.h"

extern "C" {
int my_app_main(int argc, char **argv);
};

int my_app_main(int argc, char **argv)
{
    bool quit = false;

    RoTextMode();
    RoTextModeClearDisplay();
    RoTextModeSetLine(0, 0, TEXT_NO_ATTRIBUTES, "Hello World");
    RoTextModeSetLine(1, 0, TEXT_INVERSE, "WOOOO!");

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
    }

    return 0;
}
```

Add code to the `launcher.cpp` declaring your main, like `int my_app_main(int argc, const char **argv);`, then add a string to `application_names`:

```        C++
        std::vector<const char*> application_names = {
            "MP3 Player",
            "Colecovision Emulator",
            "Apple //e Emulator",
            "My App"
        };
```

Finally, add launching code for your app to a new case statement in `launcher.cpp`:

```
            case 3: {
                const char *args[] = {
                    "my-app",
                };
                my_app_main(sizeof(args) / sizeof(args[0]), args);
            }	
```

## Write programs more or less like POSIX C++ apps

Can use C++, including: static initializers, chrono

Can use C, including: posix; some stdlib like open, close, read, write, gettimeofday, sbrk, getpid (always 1), fstat, lseek

Cannot use: fprintf("%lld"); most stdlib like exit, fork, exec, link, unlink, wait

Not sure about: C++ locking

## System functionality

Call `RoDoHousekeeping()` frequently, more than 10 times a second.  It's okay to run it after every `RoPollEvent()` call.

## Video

Video modes in Rosa are implemented as functions that fill NTSC sample buffers.  Rosa's kernel wraps the sample buffers with sync and blank and optional colorburst signals to provide an NTSC video signal.  `RoTextMode()` initializes a low-res text mode The existing `apple2e` and `coleco` apps demonstrate custom video modes. 

## Audio

Call `RoAudioGetSamplingInfo() `to get the audio sampling rate and the number of bytes of stereo unsigned 8-bit samples that are preferred in a single audio buffer update.  That is to say, each audio sample is left then right audio samples in one byte each, from 0 to 255.

## Events (keyboard, mouse, joystick)

Query keyboard events with `RoEventPoll`().

Keyboard events are key press and release values including modifier keys.  The existing `apple2e` and `coleco` apps demonstrate how to combine modifier keys and pressed to provide application-specific keyboard data.

Rosa provides convenience functions for a single key repeat.  Create a `RoKeyRepeatManager`.  After calling `RoEventPoll()`, chain its results through `RoKeyRepeatUpdate()`, which will created a repeating key press if the repeat delay has been exceeded.  The `apple2e` and `coleco` apps demonstrate the use of `RoKeyRepeatManager`.

Query joystick values using `RoGetJoystickState` using either `CONTROLLER_1` or `CONTROLLER_2`.  The result is zero or more of the active direction and fire button values or'd together:

- CONTROLLER_FIRE_BIT
- CONTROLLER_NORTH_BIT
- CONTROLLER_EAST_BIT
- CONTROLLER_SOUTH_BIT
- CONTROLLER_WEST_BIT

Query keypad values if desired (and when a Colecovision keypad is attached) using `RoGetKeypadState` using either `CONTROLLER_1` or `CONTROLLER_2`.  The result is one of the `CONTROLLER_KEYPAD` values: `0`, `1`, `2`, `3`, `4`, `5`, `6`, `7`, `8`, `9`, `asterisk`, or `pound`.

------



# Rocinante API Reference



## System API: rocinante.h

`typedef enum Status`
* `RO_FAILURE(status)`
* `RO_USER_DECLINED`
* `RO_SIZE_EXCEEDED`
* `RO_RESOURCE_NOT_FOUND`
* `RO_RESOURCE_EXHAUSTED`
* `RO_INVALID_PARAMETER_VALUE`

### Debug Overlay
`void RoDebugOverlayPrintf(const char *fmt, ...);`
`void RoDebugOverlaySetLine(int line, const char *str, size_t size);`

### Audio
`void RoAudioGetSamplingInfo(float *rate, size_t *chunkSize);`
`size_t RoAudioEnqueueSamplesBlocking(size_t writeSize /* in bytes */, uint8_t* buffer);`
`void RoAudioClear();`

### UI and File menu operations

`enum RoFileChooserFlags`
* `CHOOSE_FILE_NO_FLAGS = 0,`

Status RoFillFilenameList(const char* dirName, uint32_t flags, const char* optionalFilterSuffix, size_t maxNames, char **filenames, size_t* filenamesSize);

### Joystick, Paddle, Keypad

`CONTROLLER_*`
`RoControllerIndex` : `CONTROLLER_1`, `CONTROLLER_2`

`uint8_t RoGetJoystickState(RoControllerIndex which);`
`uint8_t RoGetKeypadState(RoControllerIndex which);`

### Video

#### Meant for use at initialization or rarely
`INLINE unsigned char RoNTSCYIQToDAC(float y, float i, float q, float tcycles);`
`INLINE unsigned char RoNTSCYIQDegreesToDAC(float y, float i, float q, int degrees);`
`INLINE ntsc_wave_t RoNTSCYIQToWave(float y, float i, float q);`
`INLINE void RoRGBToYIQ(float r, float g, float b, float *y, float *i, float *q);`
`INLINE void RoYIQToRGB(float y, float i, float q, float *r, float *g, float *b);`
`INLINE ntsc_wave_t RoNTSCRGBToWave(float r, float g, float b);`

#### Video mode initialization
`typedef void (*RoNTSCModeFillRowBufferFunc)(int frameIndex, int rowNumber, size_t maxSamples, uint8_t* rowBuffer);`
`typedef int (*RoNTSCModeNeedsColorburstFunc)();`
`void RoNTSCSetMode(int interlaced, RoNTSCModeFillRowBufferFunc fillBufferFunc, RoNTSCModeNeedsColorburstFunc needsColorBurstFunc, unsigned char *blackvalue, unsigned char *whitevalue);`

#### Synchronization to Vertical Retrace
`extern void RoNTSCWaitFrame(void);`

### Housekeeping, need to call > 10 times per second

`int RoDoHousekeeping(void);`

### In emergency and nothing else to be done

`void RoPanic(void);`

### Timing Convenience

`void RoDelayMillis(uint32_t millis);`
`uint32_t RoGetMillis();`

## Events API: `events.h`

### Polling for events

`int RoEventPoll(RoEvent *event); /* 0 if none, 1 if filled */`

### Keyboard and mouse events

`enum RoKeyCap`
`MouseMoveEvent`
`ButtonPressEvent`
`ButtonReleaseEvent`
`KeyboardRawEvent`
`RoEvent`

### Key repeat manager

`RoKeyRepeatManager`

`void RoKeyRepeatRelease(RoKeyRepeatManager *mgr, int released);`
`void RoKeyRepeatPress(RoKeyRepeatManager *mgr, int pressed);`
`int RoKeyRepeatUpdate(RoKeyRepeatManager *mgr, int haveEvent, RoEvent* ev);`

## Low-res Text mode: `text-mode.h`

`void RoTextMode();`
`void RoTextModeClearDisplay();`
`void RoTextModeGetSize(int *w, int *h);`
`TEXT_NO_ATTRIBUTES, TEXT_INVERSE`
`void RoTextModeClearArea(int column, int w, int row, int h, uint8_t attributes);`
`void RoTextModeSetLine(int row, int column, uint8_t attributes, const char *string);`

## UI elements: `ui.h`

`Status RoPromptUserToChooseFromList(const char *title, const char* const* items, size_t itemCount, int *itemChosen);`
`Status RoPromptUserToChooseFile(const char *title, const char *dirName, uint32_t flags, const char *optionalFilterSuffix, char** fileChosen);`
`void RoShowListOfItems(const char *title, const char* const* items, size_t itemsSize, int whichAtTop, int whichSelected);`
`void RoDisplayStringCentered(const char *message);`
`void RoDisplayStringAndWaitForEnter(const char *message);`

