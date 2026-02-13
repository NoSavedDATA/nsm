#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include "../utils/get_libs_version.h"

void add_dependencies(std::vector<std::string> &);

void handle_file(std::unordered_map<std::string, std::string>,
                 std::unordered_map<std::string, Dependency>&,
                 std::string lib_path, int argc, char *argv []);

void handle_install(std::unordered_map<std::string, std::string>,
                    std::unordered_map<std::string, Dependency>&,
                    std::string lib_path, int argc, char *argv []);
