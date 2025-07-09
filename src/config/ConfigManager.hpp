#pragma once

#include "config/AppConfig.hpp"



class ConfigManager {

    public:
    static ConfigManager& instance();

    bool cargarConfiguracion();  // carga config en memoria (sin JSON)
    const AppConfig& getConfig() const;
   

private:
    ConfigManager();
    ~ConfigManager() = default;
    ConfigManager(const ConfigManager&) = delete;
    ConfigManager& operator=(const ConfigManager&) = delete;

    AppConfig config_; // Aquí se guarda la configuración real
};
