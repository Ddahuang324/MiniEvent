#include "../include/Configure/Configure.hpp"
#include <fstream>
#include <iostream>


static void trim(std::string& str) {
    if(str.empty()) return;

    // 去除字符串 str 开头的空白字符（包括空格、制表符、换行符和回车符）
    str.erase(0, str.find_first_not_of(" \t\n\r"));
    // 去除字符串 str 结尾的空白字符（包括空格、制表符、换行符和回车符）
    str.erase(str.find_last_not_of(" \t\n\r") + 1);
}


Configure::Configure(const std::string& configFile)
    : isLoaded_(false) {
    // 尝试加载配置文件
    if (!load(configFile)) {
        std::cout << "Failed to open configure file: " << configFile << std::endl;
        isLoaded_ = false;
    } else {
        isLoaded_ = true;
    }
}


bool Configure::load(const std::string& configFile) {
    std::ifstream ifs(configFile.c_str());
    if (!ifs.is_open()) return false;

    std::string line;

    while (std::getline(ifs, line)) {
        trim(line);
        if (line.empty() || line[0] == '#') continue; // 跳过空行和注释行

        std::string::size_type pos = line.find('=');
        if (pos == std::string::npos) continue; // 跳过不包含

        std::string key = line.substr(0, pos);
        std::string value = line.substr(pos + 1);

        trim(key);
        trim(value);

        if (!key.empty()) {
            configMap_[key] = value;
        }
    }
    return true;
}

std::string Configure::get(const std::string& key, const std::string& defaultValue) {
    std::map<std::string, std::string>::const_iterator it = configMap_.find(key);
    if (it != configMap_.end()) {
        return it->second;
    }
    return defaultValue;
}


