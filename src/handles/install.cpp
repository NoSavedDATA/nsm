#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <stdio.h>
#include <string>
#include <vector>

#include "../utils/include.h"

namespace fs = std::filesystem;


void add_dependencies(std::vector<std::string> &libs, std::string reqs_file) {    
    std::ifstream file;
    file.open(reqs_file);

    std::string line;
    while(std::getline(file, line)) {
        libs.push_back(line);
    }
}

inline bool compare_deps(std::string lib, std::string version,
                         std::unordered_map<std::string,Dependency> &libs_deps) {
    if (libs_deps.count(lib)==0)
        return true;

    Dependency dep = libs_deps.at(lib);

    

    float desired_version, dep_version;

    switch (dep.opc) {
        case 0: // =
            if (version=="latest") {
                std::cout << "\n";
                LogError(dep.Error(version));
                return false;
            }
            desired_version = std::stof(version);
            dep_version = std::stof(dep.version);
            if (desired_version!=dep_version) {
                std::cout << "\n";
                LogError(dep.Error(version));
                return false;
            }
            return true;
        case 1: // <
            if (version=="latest") {
                std::cout << "\n";
                LogError(dep.Error(version));
                return false;
            }
            desired_version = std::stof(version);
            dep_version = std::stof(dep.version);
            if (desired_version>=dep_version) {
                LogError(dep.Error(version));
                return false;
            }
            return true;
        case 2: // >
            if (version=="latest")
                return true; 
            desired_version = std::stof(version);
            dep_version = std::stof(dep.version);
            if (desired_version<=dep_version) {
                std::cout << "\n";
                LogError(dep.Error(version));
                return false;
            }
            return true;
        case 3: // <=
            if (version=="latest") {
                std::cout << "\n";
                LogError(dep.Error(version));
                return false;
            }
            desired_version = std::stof(version);
            dep_version = std::stof(dep.version);
            if (desired_version>dep_version) {
                std::cout << "\n";
                LogError(dep.Error(version));
                return false;
            }
            return true;
        case 4: // >=
            if (version=="latest")
                return true; 
            desired_version = std::stof(version);
            dep_version = std::stof(dep.version);
            if (desired_version<dep_version) {
                std::cout << "\n";
                LogError(dep.Error(version));
                return false;
            }
            return true;
        default:
            return false;
    }
}

inline bool compare_versions(std::string required_version, std::string cur_version, int version_mode) {

    if(required_version==cur_version)
        return true;

    switch (version_mode) {
        case 0: // =
            return required_version==cur_version;
        default:
            std::cout << "compare_version() mode not found" << "\n";
            std::exit(0);
    }
}



void install_libs(std::unordered_map<std::string,std::string> libs_version,
                  std::unordered_map<std::string,Dependency> &libs_deps,
                  std::string lib_path, std::vector<std::string> &libs) {
    for (int i=0; i<libs.size(); ++i) {
        std::string lib = libs[i];
        LibPathVersion lib_path_version = get_lib_name_version(lib_path, lib, "/");
        std::string lib_dir = lib_path_version.path;
        std::string repo = lib_path_version.repo;
        std::string lib_name = lib_path_version.name;
        std::string version = lib_path_version.version;


        // check deps 
        if(!compare_deps(lib_name, version, libs_deps))
            continue;

        // check if it needs skip or overwrite
        if (fs::exists(lib_dir)) {
            if (libs_version.count(lib_name)==0||compare_versions(version, libs_version[lib_name], 0))
                continue;
            else {
                std::string rm_command = "rm -r " + lib_dir;
                std::system(rm_command.c_str());
            }
        }

        // git clone
        std::string repository = "https://github.com/";
        repository += repo;
        repository += ".git";
        if (version=="latest")
            git_clone_to_folder(repository, lib_dir); 
        else
            git_clone_branch_to_folder(repository, version, lib_dir); 

        // wget
        std::string so_file = "https://github.com/";
        so_file += repo;
        so_file += "/releases/download/" + version + "/lib.so"; 
        wget_to_folder(so_file, lib_dir); 

        std::string deps_sh = lib_dir + "/deps.sh";
        if (fs::exists(deps_sh)) {
            std::string chmod = "chmod +x " + deps_sh;
            std::system(chmod.c_str());
            std::system(deps_sh.c_str());
        }

        // add deps
        std::string deps_file = lib_dir+"/deps.txt";
        if (fs::exists(deps_file))
            add_dependencies(libs, deps_file);
    }
}

void handle_file(std::unordered_map<std::string,std::string> libs_version, 
                 std::unordered_map<std::string,Dependency> &libs_deps,
                 std::string lib_path, int argc, char *argv []) {
    if (argc<=2)
        LogError("Install (f) operation missing files.");

    std::vector<std::string> libs;

    for (int i=2; i<argc; ++i) {
        if (fs::exists(argv[i]))
            add_dependencies(libs, argv[i]);
    }
    
    install_libs(libs_version, libs_deps, lib_path, libs); 

    libs_version = get_libs_version(lib_path);
    for (const auto pair : libs_version) {
        if(!compare_deps(pair.first, pair.second, libs_deps))
            continue;
    }
}


void handle_install(std::unordered_map<std::string,std::string> libs_version,
                    std::unordered_map<std::string,Dependency> &libs_deps,
                    std::string lib_path, int argc, char *argv []) {
    if (argc<=2)
        LogError("Install operation missing packages.");

    std::vector<std::string> libs;
    for (int i=2; i<argc; ++i)
        libs.push_back(argv[i]);
    
    install_libs(libs_version, libs_deps, lib_path, libs); 

    libs_version = get_libs_version(lib_path);
    for (const auto pair : libs_version) {
        if(!compare_deps(pair.first, pair.second, libs_deps))
            continue;
    }
}
