#include <cstdlib>
#include <cstring>
#include <iostream>
#include <string>

#include "get_files.h"
#include "logging.h"

LibPathVersion::LibPathVersion(std::string path, std::string name, std::string repo, std::string version)
    : path(path), name(name), repo(repo), version(version) {}

bool wget_to_folder(const std::string& url, const std::string& folder) {
    std::string cmd = "wget -P \"" + folder + "\" \"" + url + "\"";
    return std::system(cmd.c_str()) == 0;
}

bool contains_str(const std::string& str_input, const std::string& str_sub) {
    return str_input.find(str_sub) != std::string::npos;
}

bool git_clone_to_folder(const std::string& repo_url,
                         const std::string& target_folder) {
    std::string cmd = "git clone \"" + repo_url + "\" \"" + target_folder + "\"";
    return std::system(cmd.c_str()) == 0;
}

bool git_clone_branch_to_folder(const std::string& repo_url, const std::string &version,
                         const std::string& target_folder) {
    std::string cmd = "git clone --branch \"" + version + "\" --single-branch \"" + repo_url + "\" \"" + target_folder + "\"";
    std::cout << cmd << "\n";
    return std::system(cmd.c_str()) == 0;
}

bool remove_folder(const std::string& target_folder) {
    std::string cmd = "rm -r " + target_folder;
    return std::system(cmd.c_str()) == 0;
}

// bool in_str(std::string str, std::vector<std::string> list) {
//     return std::find(list.begin(), list.end(), str) != list.end();
// }



std::string get_parent(std::string repo, std::string pattern) {
    if (!contains_str(repo, pattern)) {
        LogError(repo + " is not a valid repository.");
        std::exit(0);
    }

    char *pattern_c = pattern.data();
    char *str = repo.data();

    char *save_ptr, *last_word, *semi_last;
    char *token = strtok_r(str, pattern_c, &save_ptr);
    while(token!=nullptr) {
        semi_last = last_word;
        last_word = token;
        token = strtok_r(nullptr, pattern_c, &save_ptr);
    }

    std::string semi_last_str = semi_last; 
    return semi_last_str;
}

std::string get_lib_name(std::string repo, std::string pattern) {
    if (!contains_str(repo, pattern)) {
        LogError(repo + " is not a valid repository.");
        std::exit(0);
    }

    char *pattern_c = pattern.data();
    char *str = repo.data();

    char *save_ptr, *last_word;
    char *token = strtok_r(str, pattern_c, &save_ptr);
    while(token!=nullptr) {
        last_word = token;
        token = strtok_r(nullptr, pattern_c, &save_ptr);
    }

    std::string lib_name = last_word; 
    return lib_name;
}


LibPathVersion get_lib_name_version(std::string lib_path, std::string repo, std::string pattern) {
    std::string version, repo_name, lib_name;


    if (contains_str(repo, "=")) {
        std::string version_pattern = "=";
        char *pattern_c = version_pattern.data();
        char *str = repo.data();

        char *save_ptr;
        char *token = strtok_r(str, pattern_c, &save_ptr);
        repo_name = token;
        token = strtok_r(nullptr, pattern_c, &save_ptr);

        version = token;
        lib_name = get_lib_name(repo_name, pattern);
    } else {
        repo_name = repo;
        lib_name = get_lib_name(repo, pattern);
        version = "latest";
    }

    lib_path = lib_path + "/" + lib_name;

    return LibPathVersion(lib_path, lib_name, repo_name, version);
}
