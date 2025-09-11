#pragma once    

#include <string>
#include <map>
#include <iostream>


class Configure {

public:

    Configure(const std::string& configFile );
    ~Configure() = default;

    std::string get(const std::string& key, const std::string& defaultValue = "") ;



private:
    bool load (const std::string& configFile);
    std::map<std::string, std::string> configMap_;
    bool isLoaded_;
};

