#ifndef _UI_H_
#define _UI_H_

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

//----------------------------------------------------------------------------
// File chooser and UI routines

Status RoPromptUserToChooseFromList(const char *title, const char* const* items, size_t itemCount, int *itemChosen);

Status RoPromptUserToChooseFile(const char *title, const char *dirName, uint32_t flags, const char *optionalFilterSuffix, char** fileChosen);

//----------------------------------------------------------------------------
// Convenient text display functions

void RoShowListOfItems(const char *title, const char* const* items, size_t itemsSize, int whichAtTop, int whichSelected);

void RoDisplayStringCentered(const char *message);

void RoDisplayStringAndWaitForEnter(const char *message);

#ifdef __cplusplus
};
#endif /* __cplusplus */


#endif /* _UI_H_ */
