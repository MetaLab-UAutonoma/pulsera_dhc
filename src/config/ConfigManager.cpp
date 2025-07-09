#include "ConfigManager.hpp"
#include <Arduino.h>

ConfigManager& ConfigManager::instance() {
    static ConfigManager instance;
    return instance;
}

ConfigManager::ConfigManager() {
    cargarConfiguracion();
}

bool ConfigManager::cargarConfiguracion() {
    // Aquí cargas la configuración directamente en config_
    config_.general.telefono_destino = "+56921686892";

    config_.ntp.server = "cl.pool.ntp.org";
    config_.ntp.gmt_offset_sec = -14400;
    config_.ntp.daylight_offset_sec = 0;

    config_.input.temp_sensor = {30.0f, 20.0f, 0.5f};
    config_.input.pox_sensor  = {30.0f, 20.0f, 10.0f};

    config_.output.modem.timer_check_seg = 5.0f;

    config_.business.watchdog.check_interval_ms = 5000;

    config_.business.watchdog.temp_rule = {36.0f, 38.5f, 300, 60, 3600};
    config_.business.watchdog.spo2_rule = {92.0f, 100.0f, 10, 60, 3600};
    config_.business.watchdog.bpm_rule  = {50.0f, 110.0f, 180, 60, 3600};

    Serial.println("✅ Configuración cargada directamente en código:");
    Serial.print("📞 Teléfono destino: ");
    Serial.println(config_.general.telefono_destino.c_str());
    Serial.print("⏱️ Timer check seg: ");
    Serial.println(config_.output.modem.timer_check_seg);

    return true;
}

const AppConfig& ConfigManager::getConfig() const {
    return config_;
}
