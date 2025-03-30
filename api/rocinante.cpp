#include "rocinante.h"
#include "ntsc-kit.h"

void RoVideoSetMode(bool interlaced, RoRowConfig line_config, void* private_data, RoVideoModeInitFunc initFunc, RoVideoModeFiniFunc finiFunc, RoVideoModeFillLineBufferFunc fillBufferFunc, RoVideoModeNeedsColorburstFunc needsColorBurstFunc)
{
    NTSCLineConfig config = [](RoRowConfig line_config) {
        switch(line_config)
        {
            case RO_VIDEO_ROW_SAMPLES_910:
                return NTSC_LINE_SAMPLES_910;
            case RO_VIDEO_ROW_SAMPLES_912:
                return NTSC_LINE_SAMPLES_912;
            case RO_VIDEO_ROW_SAMPLES_1368:
                return NTSC_LINE_SAMPLES_1368;
            default:
                printf("Received unexpected value %d for line_config in %s.\n", line_config, __FUNCTION__);
                printf("Returning NTSC_LINE_SAMPLES_912 and hoping for the best!\n");
                return NTSC_LINE_SAMPLES_912;
        }
    }(line_config);

    NTSCSetMode(interlaced, config, private_data, initFunc, finiFunc, fillBufferFunc, needsColorBurstFunc);
}

void RoVideoWaitNextField(void)
{
    NTSCWaitNextField();
}

int RoVideoWaitNextLine(void)
{
    return NTSCWaitNextLine();
}
