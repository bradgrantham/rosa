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
        }
    }(line_config);

    NTSCSetMode(interlaced, config, private_data, initFunc, finiFunc, fillBufferFunc, needsColorBurstFunc);
}

void RoVideoWaitNextField()
{
    NTSCWaitNextField();
}

int RoVideoWaitNextLine()
{
    return NTSCWaitNextLine();
}
