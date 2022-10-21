#include <cstdio>
#include "rocinante.h"
#include "text-mode.h"
#include "ui.h"

extern "C" {

int apple2_main(int argc, const char **argv);
int coleco_main(int argc, const char **argv);
int mp3player_main(int argc, const char **argv);
int trs80_main(int argc, const char **argv);

int launcher_main([[maybe_unused]] int argc, [[maybe_unused]] char **argv)
{
    while(1) {
        RoTextMode();
        const char* applications[] = {"MP3 Player", "Colecovision Emulator", "Apple //e Emulator", "TRS-80 Emulator"};
        int whichApplication;
        Status result = RoPromptUserToChooseFromList("Choose an application", applications, 4, &whichApplication, 0);

        if(result != RO_SUCCESS) {
            continue;
        }

        switch(whichApplication) {

            case 0: {
                Status status;
                char *fileChosenInDir;
                char fileChosen[512];

                status = RoPromptUserToChooseFile("Choose an MP3 File", ".", CHOOSE_FILE_IGNORE_DOTFILES, ".mp3", &fileChosenInDir);
                sprintf(fileChosen, "./%s", fileChosenInDir);
                if(status == RO_SUCCESS) {
                    const char *args[] = {
                        "mp3player",
                        fileChosen,
                    };
                    mp3player_main(sizeof(args) / sizeof(args[0]), args);
                }
                break;
            }

            case 1: {
                Status status;
                char *fileChosenInDir;
                char fileChosen[512];

                status = RoPromptUserToChooseFile("Choose a Coleco Cartridge", "coleco", CHOOSE_FILE_IGNORE_DOTFILES, NULL /* ".dsk" */, &fileChosenInDir);
                if(status == RO_SUCCESS) {
                    sprintf(fileChosen, "coleco/%s", fileChosenInDir);
                    const char *args[] = {
                        "emulator",
                        "coleco/COLECO.ROM",
                        fileChosen,
                    };
                    coleco_main(sizeof(args) / sizeof(args[0]), args); /* doesn't return */
                }
                break;
            }

            case 2: {
                printf("Starting Apple ][ emulation menu\n"); // console_flush();

                Status status;
                char *fileChosenInDir;
                char fileChosen[512];

                status = RoPromptUserToChooseFile("Choose an Apple ][ boot disk", "floppies", CHOOSE_FILE_IGNORE_DOTFILES, NULL /* ".dsk" */, &fileChosenInDir);
                if(status == RO_SUCCESS) {
                    sprintf(fileChosen, "floppies/%s", fileChosenInDir);
                    const char *args[] = {
                        "apple2e",
                        // "-fast",
                        "-diskII", "diskII.c600.c6ff.bin", fileChosen, "none",
                        "apple2e.rom",
                    };
                    apple2_main(sizeof(args) / sizeof(args[0]), args); /* doesn't return */

                } else {

                    // declined or error
                }
                break;
            }

            case 3: {
                const char *args[] = {
                    "trs80",
                };
                trs80_main(sizeof(args) / sizeof(args[0]), args);
                break;
            }
        }
    }
}

};

