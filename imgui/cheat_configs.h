#pragma once
#include "Windows.h"
#include "iostream"
#include <fstream>
#include <filesystem>
#include <string>
#include "../settings.hpp"

namespace config {
    std::string get_exe_directory() {
        return std::filesystem::current_path().string();
    }

    void save_config() {
        std::string filepath = get_exe_directory() + "/morva999_base.cfg";
        std::ofstream file(filepath);
        if (!file.is_open()) return;

        file << "[Aimbot]\n";
        file << "enable=" << (settings::aimbot::enable ? "true" : "false") << "\n";
        file << "smoothness=" << settings::aimbot::smoothness << "\n";
        file << "show_fov=" << (settings::aimbot::show_fov ? "true" : "false") << "\n";
        file << "fov=" << settings::aimbot::fov << "\n";
        file << "[Visuals]\n";
        file << "Box=" << (settings::visuals::Box ? "true" : "false") << "\n";
        file << "skeleton=" << (settings::visuals::skeleton ? "true" : "false") << "\n";
        file << "CorneredBox=" << (settings::visuals::CorneredBox ? "true" : "false") << "\n";
        file << "fill_box=" << (settings::visuals::fill_box ? "true" : "false") << "\n";
        file << "username=" << (settings::visuals::username ? "true" : "false") << "\n";

        file.close();
    }

    void load_config() {
        std::string filepath = get_exe_directory() + "/config.cfg";
        std::ifstream file(filepath);
        if (!file.is_open()) return;

        std::string line;
        std::string section;
        while (std::getline(file, line)) {
            line.erase(0, line.find_first_not_of(" \t"));
            line.erase(line.find_last_not_of(" \t") + 1);

            if (line.empty()) continue;

            if (line[0] == '[' && line[line.size() - 1] == ']') {
                section = line.substr(1, line.size() - 2);
                continue;
            }

            size_t equals_pos = line.find('=');
            if (equals_pos == std::string::npos) continue;
            std::string key = line.substr(0, equals_pos);
            std::string value = line.substr(equals_pos + 1);

            key.erase(0, key.find_first_not_of(" \t"));
            key.erase(key.find_last_not_of(" \t") + 1);
            value.erase(0, value.find_first_not_of(" \t"));
            value.erase(value.find_last_not_of(" \t") + 1);

            if (section == "Aimbot") {
                if (key == "enable") settings::aimbot::enable = (value == "true");
                else if (key == "smoothness") settings::aimbot::smoothness = std::stof(value);
                else if (key == "show_fov") settings::aimbot::show_fov = (value == "true");
                else if (key == "fov") settings::aimbot::fov = std::stof(value);
            }
            else if (section == "Visuals") {
                if (key == "Box") settings::visuals::Box = (value == "true");
                else if (key == "skeleton") settings::visuals::skeleton = (value == "true");
                else if (key == "CorneredBox") settings::visuals::CorneredBox = (value == "true");
                else if (key == "fill_box") settings::visuals::fill_box = (value == "true");
                else if (key == "username") settings::visuals::username = (value == "true");
            }
        }

        file.close();
    }
}
