#include <cstdio>
#include <cstdlib>
#include <cassert>
#include <chrono>
#include <array>
#include "SDL.h"

#include "rocinante.h"
#include "events.h"

SDL_AudioDeviceID audio_device;
bool audio_needs_start = true;
SDL_AudioFormat actual_audio_format;

void EnqueueStereoU8AudioSamples(uint8_t *buf, size_t sz)
{
    if(audio_needs_start) {
        audio_needs_start = false;
        SDL_PauseAudioDevice(audio_device, 0);
        /* give a little data to avoid gaps and to avoid a pop */
        std::array<uint8_t, 2048> lead_in;
        size_t sampleCount = lead_in.size() / 2;
        for(size_t i = 0; i < sampleCount; i++) {
            lead_in[i * 2 + 0] = static_cast<uint8_t>(128 + (buf[0] - 128) * i / sampleCount);
            lead_in[i * 2 + 1] = static_cast<uint8_t>(128 + (buf[0] - 128) * i / sampleCount);
        }
        SDL_QueueAudio(audio_device, lead_in.data(), static_cast<uint32_t>(lead_in.size()));
    }

    if(actual_audio_format == AUDIO_U8) {
        SDL_QueueAudio(audio_device, buf, static_cast<uint32_t>(sz));
    }
}


SDL_Window *window;
SDL_Renderer *renderer;
SDL_Surface *surface;

const static int SCREEN_X = 704;
const static int SCREEN_Y = 480;
const static int SCREEN_SCALE = 1;

void Start(uint32_t& stereoU8SampleRate, size_t& preferredAudioBufferSizeBytes)
{
#if defined(EMSCRIPTEN)

    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_EVENTS) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        exit(1);
    }

#else /* ! EMSCRIPTEN */

    if(SDL_Init(SDL_INIT_VIDEO | SDL_INIT_AUDIO | SDL_INIT_JOYSTICK | SDL_INIT_EVENTS) != 0) {
        SDL_Log("Unable to initialize SDL: %s", SDL_GetError());
        exit(1);
    }

#endif /* EMSCRIPTEN */

    window = SDL_CreateWindow("Rocinante Emulator", SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, SCREEN_X * SCREEN_SCALE, SCREEN_Y * SCREEN_SCALE, SDL_WINDOW_RESIZABLE);
    if(!window) {
        printf("could not open window\n");
        exit(1);
    }
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if(!renderer) {
        printf("could not create renderer\n");
        exit(1);
    }
    surface = SDL_CreateRGBSurface(0, SCREEN_X, SCREEN_Y, 24, 0, 0, 0, 0);
    if(!surface) {
        printf("could not create surface\n");
        exit(1);
    }

    SDL_AudioSpec audiospec;
    audiospec.freq = 44100;
    audiospec.format = AUDIO_U8;
    audiospec.channels = 2;
    audiospec.samples = 1024;
    audiospec.callback = nullptr;
    SDL_AudioSpec obtained;

    audio_device = SDL_OpenAudioDevice(nullptr, 0, &audiospec, &obtained, SDL_AUDIO_ALLOW_FREQUENCY_CHANGE);
    assert(audio_device > 0);
    assert(obtained.channels == audiospec.channels);

    switch(obtained.format) {
        case AUDIO_U8: {
            /* okay, native format */
            break;
        }
        default:
            printf("unknown audio format chosen: %X\n", obtained.format);
            exit(1);
    }

    stereoU8SampleRate = obtained.freq;
    preferredAudioBufferSizeBytes = obtained.samples * 2;
    actual_audio_format = obtained.format;

    SDL_PumpEvents();
}

bool shift_pressed = false;
bool quit_requested = false;

static void HandleEvents(void)
{
    SDL_Event event;
    while (SDL_PollEvent(&event)) {
        switch (event.type) {
            case SDL_WINDOWEVENT:
                // printf("window event %d\n", event.window.event);
                // switch(event.window.event) {
                // }
                break;
            case SDL_QUIT:
                quit_requested = true;
                break;

            case SDL_KEYDOWN:
                switch (event.key.keysym.scancode) {
                    case SDL_SCANCODE_Q:
                        quit_requested = true;
                        break;
                    case SDL_SCANCODE_RSHIFT:
                    case SDL_SCANCODE_LSHIFT:
                        shift_pressed = true;
                        break;
                    case SDL_SCANCODE_W:
                        break;
                    case SDL_SCANCODE_A:
                        break;
                    case SDL_SCANCODE_S:
                        break;
                    case SDL_SCANCODE_D:
                        break;
                    case SDL_SCANCODE_SPACE:
                        break;
                    case SDL_SCANCODE_RETURN:
                        break;
                    case SDL_SCANCODE_0:
                        break;
                    case SDL_SCANCODE_1:
                        break;
                    case SDL_SCANCODE_2:
                        break;
                    case SDL_SCANCODE_3:
                        if(shift_pressed) {
                        } else {
                        }
                        break;
                    case SDL_SCANCODE_4:
                        break;
                    case SDL_SCANCODE_5:
                        break;
                    case SDL_SCANCODE_6:
                        break;
                    case SDL_SCANCODE_7:
                        break;
                    case SDL_SCANCODE_8:
                        if(shift_pressed) {
                        } else {
                        }
                        break;
                    case SDL_SCANCODE_9:
                        break;
                    default:
                        break;
                }
                break;
            case SDL_KEYUP:
                switch (event.key.keysym.scancode) {
                    case SDL_SCANCODE_V:
                        break;
                    case SDL_SCANCODE_N:
                        break;
                    case SDL_SCANCODE_R:
                        break;
                    case SDL_SCANCODE_RSHIFT:
                    case SDL_SCANCODE_LSHIFT:
                        shift_pressed = false;
                        break;
                    case SDL_SCANCODE_W:
                        break;
                    case SDL_SCANCODE_A:
                        break;
                    case SDL_SCANCODE_S:
                        break;
                    case SDL_SCANCODE_D:
                        break;
                    case SDL_SCANCODE_SPACE:
                        break;
                    case SDL_SCANCODE_RETURN:
                        break;
                    case SDL_SCANCODE_0:
                        break;
                    case SDL_SCANCODE_1:
                        break;
                    case SDL_SCANCODE_2:
                        break;
                    case SDL_SCANCODE_3:
                        break;
                    case SDL_SCANCODE_4:
                        break;
                    case SDL_SCANCODE_5:
                        break;
                    case SDL_SCANCODE_6:
                        break;
                    case SDL_SCANCODE_7:
                        break;
                    case SDL_SCANCODE_8:
                        break;
                    case SDL_SCANCODE_9:
                        break;
                    default:
                        break;
                }
                break;
            default:
                break;
        }
    }
}

void Frame(unsigned char *samples)
{
    using namespace std::chrono_literals;

    if (SDL_MUSTLOCK(surface)) SDL_LockSurface(surface);

    uint8_t* framebuffer = reinterpret_cast<uint8_t*>(surface->pixels);
    for(int y = 0; y < 480; y++) {
        for(int x = 0; x < 704; x++) {
            uint8_t *pixel = framebuffer + 3 * (x + y * SCREEN_X);
            uint8_t *sample = samples + x + y * SCREEN_X;
            pixel[0] = sample[0];
            pixel[1] = sample[0];
            pixel[2] = sample[0];
        }
    }

    if (SDL_MUSTLOCK(surface)) SDL_UnlockSurface(surface);

#if 0
    std::chrono::time_point<std::chrono::system_clock> now = std::chrono::system_clock::now();
    std::chrono::duration<float> elapsed;
    
    elapsed = now - previous_event_time;
    if(elapsed.count() > .05) {
        HandleEvents();
        previous_event_time = now;
    }

    elapsed = now - previous_draw_time;
    if(elapsed.count() > .05) {
#endif
        SDL_Texture *texture = SDL_CreateTextureFromSurface(renderer, surface);
        if(!texture) {
            printf("could not create texture\n");
            exit(1);
        }
        SDL_RenderClear(renderer);
        SDL_RenderCopy(renderer, texture, NULL, NULL);
        SDL_RenderPresent(renderer);
        SDL_DestroyTexture(texture);
#if 0
        previous_draw_time = now;
    }
#endif
}

#if defined(EMSCRIPTEN)
void caller(void *f_)
{
    std::function<bool()> *f = (std::function<bool()>*)f_;

    bool quit = (*f)();
    if(quit) {
        emscripten_cancel_main_loop();
    }
}
#endif /* EMSCRIPTEN */ 

uint8_t samples[SCREEN_X * SCREEN_Y];

int RoEventPoll(RoEvent *event)
{
    HandleEvents();
    if(quit_requested) {
        SDL_Quit();
        exit(0);
    }
    printf("called unimplemented %s\n", __func__);
}

void RoDebugOverlayPrintf(const char *fmt, ...)
{
    printf("called unimplemented %s\n", __func__);
}

void RoDebugOverlaySetLine(int line, const char *str, size_t size)
{
    printf("called unimplemented %s\n", __func__);
}

void RoAudioGetSamplingInfo(float *rate, size_t *chunkSize)
{
    *rate = 44100;
    *chunkSize = 1024;
}

size_t RoAudioEnqueueSamplesBlocking(size_t writeSize /* in bytes */, uint8_t* buffer)
{
    EnqueueStereoU8AudioSamples(buffer, writeSize);
}

void RoAudioClear()
{
    // printf("called unimplemented %s\n", __func__);
}

Status RoFillFilenameList(const char* dirName, uint32_t flags, const char* optionalFilterSuffix, size_t maxNames, char **filenames, size_t* filenamesSize)
{
    printf("called unimplemented %s\n", __func__);
}

uint8_t RoGetJoystickState(RoControllerIndex which)
{
    printf("called unimplemented %s\n", __func__);
}

uint8_t RoGetKeypadState(RoControllerIndex which)
{
    printf("called unimplemented %s\n", __func__);
}

bool NTSCModeFuncsValid = false;
RoNTSCModeFillRowBufferFunc NTSCModeFillRowBuffer;
RoNTSCModeNeedsColorburstFunc NTSCModeNeedsColorburst;
bool NTSCModeInterlaced = false;

void RoNTSCSetMode(int interlaced_, RoNTSCModeFillRowBufferFunc fillBufferFunc_, RoNTSCModeNeedsColorburstFunc needsColorBurstFunc_, unsigned char *blackvalue, unsigned char *whitevalue)
{
    // XXX Need to lock here versus any threaded access to these variables
    NTSCModeFuncsValid = false;

    NTSCModeFillRowBuffer = fillBufferFunc_;
    NTSCModeNeedsColorburst = needsColorBurstFunc_;
    NTSCModeInterlaced = interlaced_;

    /* Should these be similar to HW values to exercise reduced precision? */
    *blackvalue = 0;
    *whitevalue = 255;

    NTSCModeFuncsValid = true;
}

extern void RoNTSCWaitFrame(void)
{
    printf("called unimplemented %s\n", __func__);
}

void RoDelayMillis(uint32_t millis)
{
    printf("called unimplemented %s\n", __func__);
}

uint32_t RoGetMillis()
{
    printf("called unimplemented %s\n", __func__);
}

int RoDoHousekeeping(void)
{
    // XXX if time has sufficiently elapsed
    if(NTSCModeInterlaced) {
        for(int lineNumber = 0; lineNumber < 480; lineNumber++) {
            if(NTSCModeFuncsValid) {
                NTSCModeFillRowBuffer(0, lineNumber, 704, samples + lineNumber * 704);
            }
        }
    } else {
        for(int lineNumber = 0; lineNumber < 240; lineNumber++) {
            if(NTSCModeFuncsValid) {
                NTSCModeFillRowBuffer(0, lineNumber, 704, samples + (lineNumber * 2 + 0) * 704);
                memcpy(samples + (lineNumber * 2 + 1) * 704, samples + (lineNumber * 2 + 0) * 704, 704);
            }
        }
    }

    Frame(samples);
}

extern "C" {
    int launcher_main(int argc, const char **argv);
}

/*
    make thread filling the samples buffer
        like STM32 RowHandler, incrementing linenumber and frame number etc
        track time - as close as possible to 15... KHz
        call row funcs
    implement RoGetMillis and RoDelay
    wrap elapsed time around Frame(samples)
    implement EventPoll
    implement DebugOverlay
*/

int main([[maybe_unused]] int argc, [[maybe_unused]] char **argv)
{
    uint32_t stereoU8SampleRate;
    size_t preferredAudioBufferSizeBytes;

    memset(samples, 0xaa, sizeof(samples));

    Start(stereoU8SampleRate, preferredAudioBufferSizeBytes);

#if defined(EMSCRIPTEN)

    std::function<bool()> body_for_emscripten = [&]()->bool{ return Iterate(); };
    emscripten_set_main_loop_arg(caller, &body_for_emscripten, 0, 1);

#else /* !EMSCRIPTEN */

    const char *args[] = {
        "launcher",
    };
    launcher_main(sizeof(args) / sizeof(args[0]), args);

    SDL_Quit();

#endif /* EMSCRIPTEN */

    return 0;
}
