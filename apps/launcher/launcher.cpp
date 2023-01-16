#include <cstdio>
#include <string>
#include <vector>
#include "rocinante.h"
#include "text-mode.h"
#include "ui.h"

extern "C" {

int apple2_main(int argc, const char **argv);
int coleco_main(int argc, const char **argv);
int mp3player_main(int argc, const char **argv);
int trs80_main(int argc, const char **argv);

struct LauncherRecord
{
    // Should std::move
    std::string name;
    std::string what_to_choose;
    std::string where_to_choose;
    std::string suffix;
    std::vector<std::string> initial_args;
    int (*main)(int argc, const char **argv);
};

std::vector<LauncherRecord> Apps;

void LauncherRegisterApp(const std::string& name, const std::string& what_to_choose, const std::string& where_to_choose, const std::string& suffix, const std::vector<std::string>& initial_args, int (*main)(int argc, const char **argv))
{
    LauncherRecord rec = {
        .name = name,
        .what_to_choose = what_to_choose,
        .where_to_choose = where_to_choose,
        .suffix = suffix,
        .initial_args = initial_args,
        .main = main
    };
    Apps.push_back(rec); // Should std::move
}

int launcher_main([[maybe_unused]] int argc, [[maybe_unused]] char **argv)
{
    while(1) {
        RoTextMode();
        std::vector<const char*> applications = {
            "MP3 Player",
            "Colecovision Emulator",
            "Apple //e Emulator",
            "Apple //e Emulator (no disk II)",
            "TRS-80 Emulator"
        };
        int whichApplication;
        Status result = RoPromptUserToChooseFromList("Choose an application", applications.data(), applications.size(), &whichApplication, 0);

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
                    "apple2e",
                    "apple2e.rom",
                };
                apple2_main(sizeof(args) / sizeof(args[0]), args); /* doesn't return */

                break;
            }

            case 4: {
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

