#pragma once

#include <unordered_map>
#include <string>

struct Dependency {
    std::string dependent_lib, name, op, version;
    int opc;

    Dependency(std::string, std::string, std::string, std::string);
    void Print();
    std::string Error(std::string);
};


std::unordered_map<std::string, std::string> get_libs_version(std::string);
std::unordered_map<std::string, Dependency> get_libs_dependencies(std::string);
