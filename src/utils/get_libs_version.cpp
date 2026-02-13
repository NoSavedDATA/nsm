

#include <filesystem>
#include <fstream>
#include <iostream>
#include <map>
#include <mutex>
#include <stdio.h>
#include <string>
#include <unordered_map>
#include <vector>

#include "get_files.h"
#include "get_libs_version.h"

namespace fs = std::filesystem;

std::vector<std::string> listdir(const fs::path& dir) {
    std::vector<std::string> entries;
    std::error_code ec;

    if (!fs::exists(dir, ec) || !fs::is_directory(dir, ec))
        return entries;

    for (const auto& entry : fs::directory_iterator(dir, ec)) {
        if (ec) break;
        entries.push_back(entry.path().filename().string());
    }

    return entries;
}

std::vector<fs::path> glob_extension(const fs::path& rootDir, std::string extension) {
    std::vector<fs::path> cppFiles;
    std::mutex mtx;

    for (const auto& entry : fs::recursive_directory_iterator(rootDir)) {
        const std::string filename = entry.path().string();
        if (filename.size() >= extension.size() &&
            filename.compare(filename.size() - extension.size(), extension.size(), extension) == 0) {
            std::lock_guard<std::mutex> lock(mtx);
            cppFiles.push_back(entry.path());
        }
    }

    return std::move(cppFiles);
}



std::vector<fs::path> find_nested_files(const fs::path& lib_dir, std::string file) {
    std::vector<fs::path> result;

    std::error_code ec;

    if (!fs::exists(lib_dir, ec) || !fs::is_directory(lib_dir, ec))
        return result;

    for (const auto& entry : fs::directory_iterator(lib_dir, ec)) {
        if (ec) break;

        if (!entry.is_directory(ec))
            continue;

        fs::path version_path = entry.path() / file;

        if (fs::exists(version_path, ec) && fs::is_regular_file(version_path, ec)) {
            result.push_back(fs::canonical(version_path, ec));
        }
    }

    return result;
}


std::unordered_map<std::string,std::string> get_libs_version(std::string lib_path) {
    std::unordered_map<std::string,std::string> libs_version;
    
    std::vector<fs::path> version_files = find_nested_files(lib_path, "version.txt");

    for (int i=0; i<version_files.size(); ++i) {
        std::ifstream file;
        file.open(version_files[i]);
        std::string line;
        std::getline(file, line);

        std::string lib = get_parent(version_files[i], "/");

        
        libs_version[lib] = line;
    }
 
    std::vector<std::string> libs = listdir(lib_path);

    for (const auto lib : libs) {
        if (libs_version.count(lib)==0)
            libs_version[lib] = "latest";
    }

    return libs_version;
}



std::unordered_map<std::string, int> requirement_op_map = {{"=", 0}, {"<", 1}, {">", 2}, {"<=", 3}, {">=", 4}};

Dependency::Dependency(std::string dependent_lib, std::string name, std::string op, std::string version)
    : dependent_lib(dependent_lib), name(name), op(op), version(version) {
    opc = requirement_op_map[op];
}

void Dependency::Print() {
    std::cout << name << ":\n\t" << dependent_lib << ", " << op << ", " << version << "\n";
}

std::string Dependency::Error(std::string required_version) {
    std::string err = "Tried to install " + name + " version \"" + required_version + "\", but " + dependent_lib + " requires " + name + op + "\""+ version + "\"";
    return err;
}

void parse_dependency(std::string lib, std::string line,
                        std::unordered_map<std::string, Dependency> &libs_deps) {
    const std::string ops[] = {">=", "<=", "=", ">", "<"};

    for (const auto& op : ops) {
        size_t pos = line.find(op);
        if (pos != std::string::npos) {

            std::string lib_name = line.substr(0, pos);

            size_t slash_pos = lib_name.find("/");
            lib_name = lib_name.substr(slash_pos+1);
            


            Dependency r = Dependency(lib, lib_name, op, line.substr(pos + op.size()));

            auto trim = [](std::string& s) {
                const char* ws = " \t\n\r";
                s.erase(0, s.find_first_not_of(ws));
                s.erase(s.find_last_not_of(ws) + 1);
            };

            trim(r.name);
            trim(r.version);


            libs_deps.emplace(lib_name, r);
        }
    }
}


std::unordered_map<std::string,Dependency> get_libs_dependencies(std::string lib_path) {
    std::unordered_map<std::string, Dependency> libs_deps;
    
    std::vector<fs::path> version_files = find_nested_files(lib_path, "deps.txt");

    for (int i=0; i<version_files.size(); ++i) {
        std::ifstream file;
        file.open(version_files[i]);
        std::string line;
        std::getline(file, line);

        std::string lib = get_parent(version_files[i], "/");

        if (contains_str(line, "=")||contains_str(line,"<")||contains_str(line, ">")) {
            parse_dependency(lib, line, libs_deps);
        }
    }
    return libs_deps;
}
