#pragma once

#include "config/AppConfig.hpp"
#include "SPIFFS.h"

class ConfigManager {
public:
    static ConfigManager& instance();

    bool loadFromFile(const char* filepath);
    const AppConfig& getConfig() const;

private:
    ConfigManager(); // Constructor privado
    ~ConfigManager() = default;
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;

    AppConfig config_; // Almacena la configuración cargada
    bool loaded_ = false;
};