#ifndef _NTSC_KIT_PLATFORM_H_
#define _NTSC_KIT_PLATFORM_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

// Defined by platform, called by NTSC-Kit
void PlatformEnableNTSCScanout(NTSCLineConfig line_config, bool interlaced);
void PlatformDisableNTSCScanout(void);
uint8_t PlatformVoltageToDACValue(float voltage);
int PlatformGetNTSCLineNumber(void);

// Defined by NTSC-KIT, called by platform, preferably immediately after
// beginning scanout of the *other* scanline buffer
void NTSCFillLineBuffer(int frameNumber, int lineNumber, unsigned char *lineBuffer);

// Defined by NTSC-KIT, called by platform to initialize state
void NTSCInitialize(void);

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif /* _NTSC_KIT_PLATFORM_H_ */
