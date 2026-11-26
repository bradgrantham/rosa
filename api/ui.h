#ifndef _UI_H_
#define _UI_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

//----------------------------------------------------------------------------
// File chooser and UI routines

// Prompts are driven by controller 1 (north and south move and repeat while
// held, west is "back", fire selects) or by a keyboard (arrows, ENTER, ESCAPE).

// Returns RO_USER_DECLINED and sets *itemChosen to -1 if can_cancel and the user went back.
Status RoPromptUserToChooseFromList(const char *title, const char* const* items, size_t itemCount, int *itemChosen, int can_cancel);

// Show a message above a row of buttons.  East and west move the highlight,
// which starts on defaultButton, and fire selects.  defaultButton should be
// the safe choice: ESCAPE returns RO_USER_DECLINED with it in *buttonChosen.
Status RoPromptUserWithButtons(const char *title, const char *message, const char* const* buttons, size_t buttonCount, int defaultButton, int *buttonChosen);

Status RoPromptUserToChooseFile(const char *title, const char *dirName, uint32_t flags, const char *optionalFilterSuffix, char** fileChosen);

//----------------------------------------------------------------------------
// Convenient text display functions

int RoCompareCaseInsensitive(const char* a, const char* b);

void RoShowListOfItems(const char *title, const char* const* items, size_t itemsSize, int whichAtTop, int whichSelected, int can_cancel);

void RoDisplayStringCentered(const char *message);

void RoDisplayStringAndWaitForEnter(const char *message);

#ifdef __cplusplus
};
#endif /* __cplusplus */


#endif /* _UI_H_ */
