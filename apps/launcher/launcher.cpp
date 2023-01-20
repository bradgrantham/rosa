#include <cstdio>
#include <string>
#include <vector>
#include "rocinante.h"
#include "text-mode.h"
#include "ui.h"
#include "launcher.h"

struct LauncherRecord
{
    // Should std::move
    std::string name;
    std::string exe_name;
    std::string what_to_choose;
    std::string where_to_choose;
    std::string suffix;
    std::vector<std::string> first_args;
    std::vector<std::string> last_args;
    int (*main)(int argc, const char **argv);
};

std::vector<LauncherRecord> *Apps;

void LauncherRegisterApp(const std::string& name, const std::string& exe_name, const std::string& what_to_choose, const std::string& where_to_choose, const std::string& suffix, const std::vector<std::string>& first_args, const std::vector<std::string>& last_args, int (*main)(int argc, const char **argv))
{
    LauncherRecord rec = {
        .name = name,
        .exe_name = exe_name,
        .what_to_choose = what_to_choose,
        .where_to_choose = where_to_choose,
        .suffix = suffix,
        .first_args = first_args,
        .last_args = last_args,
        .main = main
    };
    if(Apps == nullptr) {
        Apps = new std::vector<LauncherRecord>;
    }
    Apps->push_back(rec); // Should std::move
    printf("registered %s, apps = %zd\n", name.c_str(), Apps->size());
}

extern "C" {

int launcher_main([[maybe_unused]] int argc, [[maybe_unused]] char **argv)
{
    std::vector<const char*> applications;
    for(const auto& app: (*Apps)) {
        // XXX bare pointer to char* to interface with C
        applications.push_back(app.name.c_str());
    }
    
    while(1) {
        RoTextMode();
        int whichApplication;
        Status result = RoPromptUserToChooseFromList("Choose an application", applications.data(), applications.size(), &whichApplication, 0);

        if(result != RO_SUCCESS) {
            continue;
        }

        const auto& app = (*Apps)[whichApplication];
        std::vector<const char*> args;
        args.push_back(app.exe_name.c_str());
        for(const auto& initarg: app.first_args) {
            args.push_back(initarg.c_str());
        }

        if(app.what_to_choose.empty()) {

            for(const auto& lastarg: app.last_args) {
                args.push_back(lastarg.c_str());
            }
            app.main(args.size(), args.data());
            break;

        } else {

            Status status;
            char *fileChosenInDir;
            char fileChosen[512];

            const char *suffix = app.suffix.empty() ? NULL : app.suffix.c_str();

            status = RoPromptUserToChooseFile(app.what_to_choose.c_str(), app.where_to_choose.c_str(), CHOOSE_FILE_IGNORE_DOTFILES, suffix, &fileChosenInDir);
            sprintf(fileChosen, "%s/%s", app.where_to_choose.c_str(), fileChosenInDir);
            if(status == RO_SUCCESS) {
                args.push_back(fileChosen);
                for(const auto& lastarg: app.last_args) {
                    args.push_back(lastarg.c_str());
                }
                app.main(args.size(), args.data());
            }
        }
    }
    return EXIT_SUCCESS;
}

};

