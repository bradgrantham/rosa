#ifndef _TEXT_MODE_H_
#define _TEXT_MODE_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

void RoTextMode(void);

void RoTextModeClearDisplay(void);

void RoTextModeGetSize(int *w, int *h);

void RoTextModeClearArea(int column, int w, int row, int h, uint8_t attributes);

void RoTextModeSetLine(int row, int column, uint8_t attributes, const char *string);

enum {
    TEXT_NO_ATTRIBUTES = 0x00,
    TEXT_INVERSE       = 0x01,
};

#ifdef __cplusplus
};
#endif /* __cplusplus */

#endif /* _TEXT_MODE_H_ */
