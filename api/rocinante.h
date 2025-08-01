#ifndef _ROCINANTE_H_
#define _ROCINANTE_H_

#include <math.h>
#include <inttypes.h>
#include <stdbool.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define ROCINANTE 1

typedef enum Status {
    RO_SUCCESS = 0,

    RO_USER_DECLINED = 1,               // The user declined a UI prompt (not an error)
    RO_ITEMS_DISCARDED = 2,             // The user buffer was filled but more items were available

    RO_NO_VIDEO_SUBSYSTEM_SET = -1,        // The platform did not set the video subsystem
    RO_INVALID_VIDEO_MODE_NUMBER = -2,     // The index passed was not in the range of valid modes
    RO_INVALID_STRUCTURE_SIZE = -3,        // The "size" parameter did not match the size of the requested structure
    RO_INVALID_STRUCTURE_TYPE = -4,        // The "type" parameter did not match the size of the requested structure
    RO_VIDEO_MODE_DOES_NOT_MATCH = -5,     // The "type" parameter to VideoModeGetInfo did not match the requested mode
    RO_VIDEO_MODE_INFO_UNSUPPORTED = -6,   // The video subsystem does not support returning info on the requested mode
    RO_INVALID_WINDOW = -7,                // A window was not open or valid with the provided index
    RO_INVALID_PARAMETER_VALUE = -8,       // A passed parameter was outside the valid range
    RO_WINDOW_CREATION_FAILED = -9,        // Window could not be created because of memory allocation or possibly other reason
    RO_RESOURCE_EXHAUSTED = -10,           // There were no more objects of the requested type
    RO_SIZE_EXCEEDED = -11,                // Window could not be created because of memory allocation or possibly other reason
    RO_RESOURCE_NOT_FOUND = -12,           // Root resource e.g. "/" could not be found
} Status;

#define RO_FAILURE(status) ((status) < 0)

//----------------------------------------------------------------------------
// Debug Overlay

/*! Add a line to the debug overlay.  The actual formatting function is vsnprintf.
    \param fmt The printf-style format.
    \param ... Arguments used in the format.
*/
void RoDebugOverlayPrintf(const char *fmt, ...);

/*! Set a line in the debug overlay.
    \param line The line to set.
    \param str The buffer to copy.
    \param size The number of bytes to copy.
*/
void RoDebugOverlaySetLine(int line, const char *str, size_t size);

//----------------------------------------------------------------------------
// Audio

/*! Get stereo audio stream info.
    \param rate Populated with the sampling rate (samples per second).
    \param bufferLength Populated with the number of 2-byte (one byte each left and right) unsigned (0-255) samples in the buffer.
*/
void RoAudioGetSamplingInfo(float *rate, size_t *chunkSize);

/*! Write audio samples.  Block until the write won't overlap the current audio read cursor
    \param writeSize Size of buffer in bytes.
    \param buffer The buffer of stereo u8 samples to write into the audio stream buffer.
    \return The number of bytes that had to be played before writing
*/
size_t RoAudioEnqueueSamplesBlocking(size_t writeSize /* in bytes */, uint8_t* buffer);

/*! Clear the audio stream to silence
*/
void RoAudioClear(void);


//----------------------------------------------------------------------------
// File operations

typedef enum RoFileChooserFlags {
    CHOOSE_FILE_NO_FLAGS = 0,
    CHOOSE_FILE_IGNORE_DOTFILES = 0x01,
} RoFileChooserFlags;

Status RoFillFilenameList(const char* dirName, uint32_t flags, const char* optionalFilterSuffix, size_t maxNames, char **filenames, size_t* filenamesSize);

//----------------------------------------------------------------------------
// Controllers; joysticks, keypads

enum {
    CONTROLLER_FIRE_BIT = 0x40,
    CONTROLLER_NORTH_BIT = 0x01,
    CONTROLLER_EAST_BIT = 0x02,
    CONTROLLER_SOUTH_BIT = 0x04,
    CONTROLLER_WEST_BIT = 0x08,
    CONTROLLER_KEYPAD_MASK = 0x0F,
    CONTROLLER_KEYPAD_0 = 0x05,
    CONTROLLER_KEYPAD_1 = 0x02,
    CONTROLLER_KEYPAD_2 = 0x08,
    CONTROLLER_KEYPAD_3 = 0x03,
    CONTROLLER_KEYPAD_4 = 0x0D,
    CONTROLLER_KEYPAD_5 = 0x0C,
    CONTROLLER_KEYPAD_6 = 0x01,
    CONTROLLER_KEYPAD_7 = 0x0A,
    CONTROLLER_KEYPAD_8 = 0x0E,
    CONTROLLER_KEYPAD_9 = 0x04,
    CONTROLLER_KEYPAD_asterisk = 0x09,
    CONTROLLER_KEYPAD_pound = 0x06,
};

typedef enum RoControllerIndex { CONTROLLER_1, CONTROLLER_2 } RoControllerIndex;

uint8_t RoGetJoystickState(RoControllerIndex which);
uint8_t RoGetKeypadState(RoControllerIndex which);

//----------------------------------------------------------------------------
// Video configuration

typedef enum {
    RO_VIDEO_ROW_SAMPLES_910 = 1,          // 912 samples, 4 per colorburst cycle
    RO_VIDEO_ROW_SAMPLES_912 = 2,          // 912 samples, 4 per colorburst cycle
    RO_VIDEO_ROW_SAMPLES_1368 = 3,         // 1368 samples, 6 per colorburst cycle
} RoRowConfig;

typedef int (*RoVideoModeInitFunc)(void* private_data, uint8_t blackvalue, uint8_t whitevalue);
typedef void (*RoVideoModeFiniFunc)(void* private_data);
typedef void (*RoVideoModeFillLineBufferFunc)(int frameIndex, int lineWithinField, int lineNumber, size_t maxSamples, uint8_t* lineBuffer);
typedef int (*RoVideoModeNeedsColorburstFunc)(void);

typedef uint32_t ntsc_wave_t;

inline unsigned char RoNTSCYIQToDAC(float y, float i, float q, float tcycles, uint8_t black, uint8_t white)
{
// This is transcribed from the NTSC spec, double-checked.
    float w_t = tcycles * M_PI * 2;
    float sine = sinf(w_t + 33.0f / 180.0f * M_PI);
    float cosine = cosf(w_t + 33.0f / 180.0f * M_PI);
    float signal = y + q * sine + i * cosine;
// end of transcription

    return black + signal * (white - black);
}

inline unsigned char RoNTSCYIQDegreesToDAC(float y, float i, float q, int degrees, uint8_t black, uint8_t white)
{
    float sine, cosine;
    if(degrees == 0) {
        sine = 0.544638f;
        cosine = 0.838670f;
    } else if(degrees == 90) {
        sine = 0.838670f;
        cosine = -0.544638f;
    } else if(degrees == 180) {
        sine = -0.544638f;
        cosine = -0.838670f;
    } else if(degrees == 270) {
        sine = -0.838670f;
        cosine = 0.544638f;
    } else {
        sine = 0;
        cosine = 0;
    }
    float signal = y + q * sine + i * cosine;

    return black + signal * (white - black);
}

// This is transcribed from the NTSC spec, double-checked.
inline void RoRGBToYIQ(float r, float g, float b, float *y, float *i, float *q)
{
    *y = .30f * r + .59f * g + .11f * b;
    *i = -.27f * (b - *y) + .74f * (r - *y);
    *q = .41f * (b - *y) + .48f * (r - *y);
}

// Alternatively, a 3x3 matrix transforming [r g b] to [y i q] is:
// (untested - computed from equation above)
// 0.300000 0.590000 0.110000
// 0.599000 -0.277300 -0.321700
// 0.213000 -0.525100 0.312100

// A 3x3 matrix transforming [y i q] back to [r g b] is:
// (untested - inverse of 3x3 matrix above)
// 1.000000 0.946882 0.623557
// 1.000000 -0.274788 -0.635691
// 1.000000 -1.108545 1.709007

// Using inverse 3x3 matrix above.  Tested numerically to be the inverse of RGBToYIQ
inline void RoYIQToRGB(float y, float i, float q, float *r, float *g, float *b)
{
    *r = 1.0f * y + .946882f * i + 0.623557f * q;
    *g = 1.000000f * y + -0.274788f * i + -0.635691f * q;
    *b = 1.000000f * y + -1.108545f * i + 1.709007f * q;
}

void RoVideoSetMode(bool interlaced, RoRowConfig line_config, void* private_data, RoVideoModeInitFunc initFunc, RoVideoModeFiniFunc finiFunc, RoVideoModeFillLineBufferFunc fillBufferFunc, RoVideoModeNeedsColorburstFunc needsColorBurstFunc);
void RoVideoWaitNextField(void);
int RoVideoWaitNextLine(void);

//----------------------------------------------------------------------------
// Do periodic work that has to happen > 10 times a second

int RoDoHousekeeping(void);

//----------------------------------------------------------------------------
// Flash system LED and infinite loop

void RoPanic(void);

//----------------------------------------------------------------------------
// Timing convenience functions

void RoDelayMillis(uint32_t millis);
uint32_t RoGetMillis(void);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif /* _ROCINANTE_H_ */
