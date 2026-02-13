#pragma once

#include <string>

struct LibPathVersion {
    std::string path, name, repo, version;
    LibPathVersion(std::string, std::string, std::string, std::string);
};

bool contains_str(const std::string& str_input, const std::string& str_sub);


bool wget_to_folder(const std::string& url, const std::string& folder);

bool git_clone_to_folder(const std::string& repo_url, const std::string& target_folder); 
bool git_clone_branch_to_folder(const std::string& repo_url, const std::string &version,
                         const std::string& target_folder);

bool remove_folder(const std::string&);



std::string get_lib_name(std::string, std::string);
std::string get_parent(std::string repo, std::string pattern);

LibPathVersion get_lib_name_version(std::string lib_path, std::string repo, std::string pattern);




