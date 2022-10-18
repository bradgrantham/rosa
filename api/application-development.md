# Rocinante API



## Write programs more or less like POSIX C++ apps



Can use C++, including:
* static initializers
* chrono

Can use C, including:

* posix
* some stdlib
  * open, close, read, write
  * gettimeofday
  * sbrk
  * getpid (always 1)
  * fstat
  * lseek

Cannot use:
* fprintf("%lld")
* most stdlib
  * exit
  * fork
  * exec
  * link
  * unlink
  * wait

Not sure about:
* C++ locking

## System API: rocinante.h

`typedef enum Status`
* `RO_FAILURE(status)
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
