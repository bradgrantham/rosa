# Rocinante ("Rosa" for short) Programming Guide



[TOC]

## The Rosa Emulator

The Rosa Emulator, `rosa-emu`, allows rapid development in desktop environments of applications for the Rocinante hardware platform.

### Build Requirements

Building `rosa-emu` requires SDL2 development libraries with CMake configuration to find the SDL libraries and headers.  For MacOS, MacPorts and Homebrew provide an `sdl2` package.  For Debian-derived Linux distributions, try `libsdl2-dev`.  On Windows one may have to download a CMake SDL2 development package, create a  directory using `-DSDL2_DIR=...` .  See https://trenki2.github.io/blog/2017/06/02/using-sdl2-with-cmake/ for more information.

### Example commands for cloning, configuring, and building `rosa-emu`:

```bash
git clone --recurse-submodules  https://github.com/bradgrantham/rosa-emu
cd rosa-emu
cmake -Bbuild . # optionally build type etc, e.g. -DCMAKE_BUILD_TYPE=Debug
(cd build ; make)
./build/rosa-emu
```

Rosa apps run in `rosa-emu` in the current working directory by default.  That's where apps will look for files, ROM and floppy images, etc.  You can specify another directory as the current directory using `--root-dir`.

Run the emulator from the build directory provided to `cmake`, e.g.`./build/rosa-emu` By default no files are provided for the existing apps.  To demonstrate the MP3 player, for example, create a new root directory, e.g. `emu-root`, then put some MP3 files in it, then run the emulator using that directory.

```C++
./build/rosa-emu --root-dir emu-root
```



### Current Emulator Limitations

* `--root-dir` is emulated with `chdir`.  If applications open the directory `.` or files beginning with `./` or with no directory name, they will correctly open files in the specified `--root-dir`.  If applications open absolute paths, they will incorrectly open files in the emulator's host system.
* Color video modes are emulated as if the display is a black-and-white TV.
* `RoNTSCWaitFrame()` is not yet implemented
* Joystick and keypad is not yet implemented.
* The emulator doesn't produce a HiDPI-aware window; it could be prettier.
* The three "console" buttons are not yet implemented.
* `RoDebugOverlayPrintf()` sends the formatted string to `stdout`.  `RoDebugOverlaySetLine()` sends the requested line to `stdout`.
* Output to serial console is implemented using `stdout`.  Input from serial console is not implemented.
* It is possible to allocate far more memory in the emulator than is available on the Rocinante hardware.



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

Call `RoDoHousekeeping()` frequently, like 100 times a second.  Housekeeping includes USB polling, updating the system LEDs, and anything else that should happen frequently but can't be performed from a separate interrupt handler on the hardware or a thread in the emulator.



## Timing

Rosa apps can use C++ `std::chrono` as well as `gettimeofday`.  Rosa's API includes a pair of limited convenience functions for timing: `RoDelayMillis()` blocks for a specified number of milliseconds, and `RoGetMillis()` returns the number of milliseconds since system startup.  (The number of millis since startup will wrap around and need special handling if the system runs for more than 49 days.)



## Video

Video modes in Rosa are implemented as functions that fill NTSC sample buffers.  Rosa's kernel wraps the sample buffers with sync and blank and optional colorburst signals to provide an NTSC video signal.  Currently 64KB are reserved for video memory.  See the use of `RoNTSCSetMode()`by `RoTextMode()` in`text-mode.cpp` for a low-res text mode and the existing `apple2e` and `coleco` apps for custom video modes.

Currently the row buffer functions are hardcoded to request 704 samples per line; 4 samples per wavelength of the color burst.  This allows full expression of NTSC color by outputting a waveform that analog equipment recognizes as a color signal.  (Most 8-bit consoles and computers in the late 1970's and early 1980's generated color for TV using a similar digital form.).

Some equpiment, notably any based on the TMS9918 series, used a clock frequency 6x the colorburst frequency.  A later revision of Rosa may extend the video mode API to include a selectable frequency multiplier for the colorburst frequency but this will be provided with a new entrypoint.

For grayscale video, provide a colorburst function that returns 0.

For color, provide a colorburst function that returns 1.  Convenience functions are provided to convert YIQ or RGB colors to an `ntsc_wave_t`, a 32-bit value that has 8-bit NTSC samples packed in LSB memory layout.  Byte N of an `ntsc_wave_t` corresponds to `sample % 4` in the output buffer.  That is to say, given an `ntsc-wave_t wave`, `wave & 0xFF` would be used for samples 0, 4, 8, etc; `(wave >> 8) & 0xFF` would be used for samples 1, 5, 9, etc. A video mode implementation is expected to generate `ntsc_wave_t` values infrequently; for example when palette entries are changed.



## Audio

Call `RoAudioGetSamplingInfo() `to get the audio sampling rate and the number of bytes of stereo unsigned 8-bit samples that are preferred in a single audio buffer update.  That is to say, each audio sample is left then right values, one byte each, from 0 to 255.



## Events (keyboard, mouse, joystick)

Query keyboard, mouse, and console button events with `RoEventPoll()`.  The `RoEvent` structure contains a `union` of all implemented event types and an `eventType` field to indicate which is valid.

Keyboard events are key press and release values including modifier keys.  The existing `apple2e` and `coleco` apps demonstrate how to combine modifier keys and pressed to provide application-specific keyboard data.

Rosa provides convenience functions for a single key repeat.  Create a `RoKeyRepeatManager`.  After calling `RoEventPoll()`, chain its results through `RoKeyRepeatUpdate()`, which will created a repeating key press if the repeat delay has been exceeded.  The `apple2e` and `coleco` apps demonstrate the use of `RoKeyRepeatManager`.

Mouse motion is represented as X and Y deltas.  Mouse button presses and releases are represented by press or release events with a button number.

Query joystick values using `RoGetJoystickState` using either `CONTROLLER_1` or `CONTROLLER_2`.  The result is zero or more of the active direction and fire button values or'd together:

- `CONTROLLER_FIRE_BIT`
- `CONTROLLER_NORTH_BIT`
- `CONTROLLER_EAST_BIT`
- `CONTROLLER_SOUTH_BIT`
- `CONTROLLER_WEST_BIT`

Query keypad values if desired (and when a Colecovision keypad is attached) using `RoGetKeypadState` using either `CONTROLLER_1` or `CONTROLLER_2`.  The result is one of the `CONTROLLER_KEYPAD` values: `0`, `1`, `2`, `3`, `4`, `5`, `6`, `7`, `8`, `9`, `asterisk`, or `pound`.

------



# Rocinante API Reference



## System API: `rocinante.h`

`typedef enum Status`
* `RO_FAILURE(status)` - `true` if the status was a failure

Not failure status codes:

* `RO_USER_DECLINED` - User canceled a UI prompt.  Not a failure status code.

Failure status codes:

* `RO_SIZE_EXCEEDED` - Size of passed buffer was insufficient.
* `RO_RESOURCE_NOT_FOUND` - Could not find requested resource by index or by name.
* `RO_RESOURCE_EXHAUSTED` - Could not allocate another object of the requested type.
* `RO_INVALID_PARAMETER_VALUE` - One parameter was not a valid value.

### Debug Overlay
`void RoDebugOverlayPrintf(const char *fmt, ...);`

* Print a debugging statement to the Debug Video Overlay a la `printf`

`void RoDebugOverlaySetLine(int line, const char *str, size_t size);`

* Set a line in the debug overlay directly.

### Audio
`void RoAudioGetSamplingInfo(float *rate, size_t *chunkSize);`

* Get the sampling rate and preferred byte count update

`size_t RoAudioEnqueueSamplesBlocking(size_t writeSize /* in bytes */, uint8_t* buffer);`

* Enqueue a buffer of U8 stereo samples
* Starts audio if paused

`void RoAudioClear();`

* Clear and pause audio

### UI and File menu operations

`enum RoFileChooserFlags`
* `CHOOSE_FILE_NO_FLAGS = 0,`

`Status RoFillFilenameList(const char* dirName, uint32_t flags, const char* optionalFilterSuffix, size_t maxNames, char **filenames, size_t* filenamesSize);`

* Fill a list of files from a directory of the filesystem. _Primarily meant for internal use by `ui.h` functions (see below)_.

### Joystick, Paddle, Keypad

`CONTROLLER_*`
`RoControllerIndex` : `CONTROLLER_1`, `CONTROLLER_2`

`uint8_t RoGetJoystickState(RoControllerIndex which);`

* Read a bitmask of joystick state

`uint8_t RoGetKeypadState(RoControllerIndex which);`

* Read the current keypad button press

### Video

#### Meant for use at initialization or rarely
`INLINE unsigned char RoNTSCYIQToDAC(float y, float i, float q, float tcycles);`

* Return the DAC sample value at a given location along the color waveform for a YIQ value.  Typically tcycles is 0.0, 0.25, 0.5, or 0.75 for 704 video samples.  (_Number of samples not guaranteed_)

`INLINE unsigned char RoNTSCYIQDegreesToDAC(float y, float i, float q, int degrees);`

* Return the DAC sample value at a given location along the color waveform for a YIQ value.  Degrees can vary 0 to 359.

`INLINE ntsc_wave_t RoNTSCYIQToWave(float y, float i, float q);`

* Return 4 NTSC samples packed into a 32-bit value for a YIQ color.

`INLINE void RoRGBToYIQ(float r, float g, float b, float *y, float *i, float *q);`

* Convert RGB to YIQ

`INLINE void RoYIQToRGB(float y, float i, float q, float *r, float *g, float *b);`

* Convert YIQ to RGB

`INLINE ntsc_wave_t RoNTSCRGBToWave(float r, float g, float b);`

* Return 4 NTSC samples packed into a 32-bit value for an RGB color.

#### Video mode initialization

A video mode switch may be requested by an application kit module (like the low-res text mode in `text-mode.h` requested by `RoTextMode()`) or by an application.

`typedef int (*RoNTSCModeInitVideoMemoryFunc)(void* buffer, uint32_t bufferSize, uint8_t blackvalue, uint8_t whitevalue);`

* Called by Rocinante kernel during a video mode switch requested by RoNTSCSetMode.  The requesting application or module should attempt to map its video resources into the provided `buffer`, not to exceed `bufferSize` bytes.  The minimum and maximum sample values are provided in `blackvalue` and `whitevalue`.

`typedef void (*RoNTSCModeFillRowBufferFunc)(int frameIndex, int rowNumber, size_t maxSamples, uint8_t* rowBuffer);`

* Called by Rocinante kernel for every scan line (so more than 15,000 times per second) to fill NTSC samples.  At the time of writing `maxSamples` is 704 but is not guaranteed not to change.
* In interlaced mode, `rowNumber` ranges from 0 to 239, and in non-interlaced mode, `rowNumber` ranges from 0 to 479.

`typedef int (*RoNTSCModeNeedsColorburstFunc)();`

* Called by Rocinante kernel every frame to determine if scanlines require the colorburst signal.  If colorburst is not provided, a TV or video monitor will typically provide higher resolution and will display only gray pixels.

`void RoNTSCSetMode(int interlaced, RoNTSCModeInitVideoMemoryFunc initFunc, RoNTSCModeFillRowBufferFunc fillBufferFunc, RoNTSCModeNeedsColorburstFunc needsColorBurstFunc);`

* Request a video mode change

#### Synchronization to Vertical Retrace
`extern void RoNTSCWaitFrame(void);`

* Block until the beginning of the next frame

### Housekeeping, need to call > 10 times per second

`int RoDoHousekeeping(void);`

* Perform housekeeping, e.g. polling USB.  This is safe to call up to 100 times per second.  Calling not frequently enough may lose USB events and calling too frequently will introduce high overhead.

### In emergency and nothing else to be done

`void RoPanic(void);`

* Flash the system LED and loop forever.  Maybe print something first.

### Timing Convenience

`void RoDelayMillis(uint32_t millis);`

* Block execution for `millis` milliseconds.

`uint32_t RoGetMillis();`

* Get milliseconds since system startup.

## Events API: `events.h`

### Polling for events

`int RoEventPoll(RoEvent *event); /* 0 if none, 1 if filled */`

* Poll for events

### Keyboard and mouse events

`MouseMoveEvent`

* Contains a mouse event with X and Y delta in `x` and `y`

`ButtonPressEvent`

* Contains a mouse button press or console button press event with button number in `button`.  If a console button was pressed (`CONSOLE_BUTTONPRESS`) then there will be no release event.

`ButtonReleaseEvent`

* Contains a mouse button releast event with button number in `button`

`KeyboardRawEvent`

* Cotnains a keyboard raw press or release event in `key`. If the key was pressed, `isPress` will be `1`, otherwise will be `0`

`RoEvent`

* An event, storing the type in `eventType` ; `MOUSE_MOVE`, `MOUSE_BUTTONPRESS`, `MOUSE_BUTTONRELEASE`, `KEYBOARD_RAW`, `CONSOLE_BUTTONPRESS`, or `EVENTS_LOST` in the event that the system queue overflowed since the last `RoEventPoll` call and events were lost.

`enum RoKeyCap`

* Enumerates the keyboard keys available on Rosa - see `events.h` for the list.

### Key repeat manager

`struct RoKeyRepeatManager`

* Class encapsulating key repeat state

`void RoKeyRepeatInit(RoKeyRepeatManager *mgr);`

* Initialize an instance of `RoKeyRepeatManager`

`void RoKeyRepeatRelease(RoKeyRepeatManager *mgr, int released);`

* internal use

`void RoKeyRepeatPress(RoKeyRepeatManager *mgr, int pressed);`

* internal use

`int RoKeyRepeatUpdate(RoKeyRepeatManager *mgr, int haveEvent, RoEvent* ev);`

* Call with an initialized `RoKeyRepeatManager` and the result of `RoEventPoll` in `haveEvent` and the `RoEvent` pass into `RoEventPoll` in `ev`.  If `RoEventPoll` didn't return an event, this function tracks a timer for the last press of a key without a release, and will fill `ev` with a key press according to key repeat timing.

## Low-res Text mode: `text-mode.h`

`void RoTextMode();`

* Request the low-res text mode

`void RoTextModeClearDisplay();`

* Clear the low-res text characters and attributes.

`void RoTextModeGetSize(int *w, int *h);`

* Get the low-rest text area size width and height

`TEXT_NO_ATTRIBUTES`

* Character is displayed normally

`TEXT_INVERSE`

* Character is displayed inverted (black character in white rectangle)

`void RoTextModeClearArea(int column, int w, int row, int h, uint8_t attributes);`

* Clear an area of the text mode screen

`void RoTextModeSetLine(int row, int column, uint8_t attributes, const char *string);`

* Copy the provided string into the text mode screen with the provided attributes.

## UI elements: `ui.h`

`Status RoPromptUserToChooseFromList(const char *title, const char* const* items, size_t itemCount, int *itemChosen, int can_cancel);`

* Display the provided list of items in the low-res text mode, allow the user to choose one (or cancel) and then return the chosen item index in `itemChosen`.
* Allow the user to exit by hitting "ESC" in which case the function returns `RO_USER_DECLINED`.

`Status RoPromptUserToChooseFile(const char *title, const char *dirName, uint32_t flags, const char *optionalFilterSuffix, char** fileChosen);`

* Probe the list of files in the directory named `dirName`,  show the list of files in in the low-res text mode, allow the user to choose one (or cancel) and then return a pointer to the chosen filename in `fileChosen`.
* Application must `free(*fileChosen)` when done with the filename.
* Optional flags include `CHOOSE_FILE_IGNORE_DOTFILES` to not allow chosing a filename starting with `.`
* Provide an optional filter suffix (e.g. `mp3`) to show only files with that suffix.

``void RoShowListOfItems(const char *title, const char* const* items, size_t itemsSize, int whichAtTop, int whichSelected, int can_cancel);`

* Internal use - show a list of items in the text mode, offer "ESC" as an option if `can_cancel` is true.

`void RoDisplayStringCentered(const char *message);`

* Display a string centered in the low-res text mode.

`void RoDisplayStringAndWaitForEnter(const char *message);`

* Display a string centered in the low-res text mode and wait for the user to press enter

