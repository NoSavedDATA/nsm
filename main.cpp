#include <iostream>
#include <map>
#include <stdio.h>
#include <string>
#include <unordered_map>
#include <vector>

#include "src/include.h"


std::map<std::string, int> modes = {{"install", 0}, {"i", 0},
                                    {"remove", 1},  {"r", 1},
                                    {"file", 2},    {"f", 2},
                                    {"help", 3},    {"h", 3}, {"-h", 3}, {"--help", 3},
                                    {"version", 4},    {"v", 4}};


void handle_remove(std::string lib_path, int argc, char *argv []) {
    if (argc<=2)
        LogError("Remove operation missing packages.");

    for (int i=2; i<argc; ++i) {
        std::string lib_dir;
        if (contains_str(argv[i], "/"))
            lib_dir = lib_path + "/" + get_lib_name(argv[i], "/"); 
        else
            lib_dir = lib_path + "/" + argv[i];
        remove_folder(lib_dir); 
    }
}

void handle_version(std::unordered_map<std::string, std::string> &libs_version, int argc, char *argv[]) {
    if (argc<=2) {
        std::cout << "nsm v expected arguments." << "\n";
        return;
    }

    for (int i=2; i<argc; ++i) {
        std::string lib_name;
        if (contains_str(argv[i], "/"))
            lib_name = get_lib_name(argv[i], "/");
        else
            lib_name = argv[i];

        if (libs_version.count(lib_name)==0)
            LogError("Lib " + lib_name + " not found."); 
        else 
            std::cout << lib_name << " - " << libs_version[lib_name] << "\n";
    }
}

void handle_help() {
    std::cout << "\nnsm install git_profile/git_repo" << "\n";
    std::cout << "nsm i git_profile/git_repo" << "\n";
    std::cout << "\tIntalls a package from a github repository." << "\n\n";

    std::cout << "nsm remove git_profile/packet_name" << "\n";
    std::cout << "nsm r git_profile/packet_name" << "\n";
    std::cout << "nsm r packet_name" << "\n";
    std::cout << "\tRemoves an installed package." << "\n\n$";

    std::cout << "nsm version git_profile/packet_name" << "\n";
    std::cout << "nsm v git_profile/packet_name" << "\n";
    std::cout << "nsm v packet_name" << "\n";
    std::cout << "\tRetrieves the version of an installed package." << "\n\n";
}




int main(int argc, char* argv[]) {
    if (argc==1) {
        std::cout << "nsm expected arguments." << "\n";
        return -1;
    }

    std::string mode_str = argv[1];
    if (modes.count(mode_str)==0) {
        LogError("Invalid argument: " + mode_str);
        return -1;
    }
    int mode = modes[mode_str];


    std::string lib_path = std::getenv("NSK_LIBS");

    std::unordered_map<std::string, std::string> libs_version = get_libs_version(lib_path);
    std::unordered_map<std::string, Dependency> libs_deps = get_libs_dependencies(lib_path);

    if (mode==0)
        handle_install(libs_version, libs_deps, lib_path, argc, argv);
    else if (mode==1)
        handle_remove(lib_path, argc, argv);
    else if (mode==2)
        handle_file(libs_version, libs_deps, lib_path, argc, argv);
    else if (mode==3)
        handle_help();
    else if (mode==4)
        handle_version(libs_version, argc, argv);
    else {}


    return 0;
}
