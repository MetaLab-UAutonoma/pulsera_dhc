#include "config/ConfigManager.hpp"
#include <ArduinoJson.h>
#include <FS.h>              // Necesario para interactuar con el sistema de archivos (SPIFFS)
#include "utils/logger.hpp"  // Para loguear los resultados


// Método para acceder a la única instancia (patrón Singleton)
ConfigManager& ConfigManager::instance() {
    static ConfigManager instance;
    return instance;
}

// El constructor privado inicializa la configuración con valores por defecto.
// Estos valores se usarán si el archivo config.json no existe o falla al cargar.
ConfigManager::ConfigManager() {
    config_ = {
        .general = {
            .telefono_destino = "+56900000000"
        },
        .ntp = {
            .server = "pool.ntp.org",
            .gmt_offset_sec = -14400, // GMT-4
            .daylight_offset_sec = 0
        },
        .input = {
            .temp_sensor = {
                .ciclo_monitoreo_seg = 30.0f,
                .duracion_lectura_seg = 20.0f,
                .timer_estabilizacion_seg = 0.5f
            },
            .pox_sensor = {
                .ciclo_monitoreo_seg = 30.0f,
                .duracion_lectura_seg = 20.0f,
                .timer_estabilizacion_seg = 10.0f
            }
        },
        .output = {
            .modem = {
                .timer_check_seg = 5.0f
            },
            .sd = { .enabled = true, .cs_pin = 5 }
        },
        .business = {
            .watchdog = {
                .check_interval_ms = 5000,
                .temp_rule = {36.0f, 38.5f, 300, 60, 3600},
                .spo2_rule = {92.0f, 100.0f, 120, 60, 3600},
                .bpm_rule = {50.0f, 110.0f, 180, 60, 3600}
            }
        }
    };
}

// Carga la configuración desde el archivo JSON en el sistema de archivos
bool ConfigManager::loadFromFile(const char* filepath) {
    if (!SPIFFS.begin(true)) {
        logger.log(LOG_ERROR, "ConfigManager: Falló al montar el sistema de archivos SPIFFS.");
        return false;
    }

    File configFile = SPIFFS.open(filepath, "r");
    if (!configFile) {
        logger.log(LOG_WARN, "ConfigManager: No se encontró '%s'. Se usarán los valores por defecto.", filepath);
        return false;
    }

    JsonDocument doc;
    DeserializationError error = deserializeJson(doc, configFile);
    configFile.close();

    if (error) {
        logger.log(LOG_ERROR, "ConfigManager: Error al parsear el archivo JSON: %s", error.c_str());
        return false;
    }

    // --- Mapeo completo del JSON a las structs anidadas ---

    // FIX 3: Manejo especial para std::string
    // Primero verificamos si la clave existe y es un string
    if (doc["general"]["telefono_destino"].is<const char*>()) {
        config_.general.telefono_destino = doc["general"]["telefono_destino"].as<std::string>();
    }

    if (doc["ntp"]["server"].is<const char*>()) {
        config_.ntp.server = doc["ntp"]["server"].as<std::string>();
    }

    // Para tipos numéricos, el operador | funciona sin problemas.
    config_.ntp.gmt_offset_sec = doc["ntp"]["gmt_offset_sec"] | config_.ntp.gmt_offset_sec;
    config_.ntp.daylight_offset_sec = doc["ntp"]["daylight_offset_sec"] | config_.ntp.daylight_offset_sec;

    // Sección [input]
    JsonObject input = doc["input"];
    config_.input.temp_sensor.ciclo_monitoreo_seg = input["TempSensor"]["ciclo_monitoreo_seg"] | config_.input.temp_sensor.ciclo_monitoreo_seg;
    config_.input.temp_sensor.duracion_lectura_seg = input["TempSensor"]["duracion_lectura_seg"] | config_.input.temp_sensor.duracion_lectura_seg;
    config_.input.temp_sensor.timer_estabilizacion_seg = input["TempSensor"]["timer_estabilizacion_seg"] | config_.input.temp_sensor.timer_estabilizacion_seg;

    config_.input.pox_sensor.ciclo_monitoreo_seg = input["PoxMax30100Sensor"]["ciclo_monitoreo_seg"] | config_.input.pox_sensor.ciclo_monitoreo_seg;
    config_.input.pox_sensor.duracion_lectura_seg = input["PoxMax30100Sensor"]["duracion_lectura_seg"] | config_.input.pox_sensor.duracion_lectura_seg;
    config_.input.pox_sensor.timer_estabilizacion_seg = input["PoxMax30100Sensor"]["timer_estabilizacion_seg"] | config_.input.pox_sensor.timer_estabilizacion_seg;

    // Sección [output]
    config_.output.modem.timer_check_seg = doc["output"]["Sim7600Modem"]["timer_check_seg"] | config_.output.modem.timer_check_seg;
    JsonObject sdJson = doc["output"]["SD"];
    config_.output.sd.enabled = sdJson["enabled"] | config_.output.sd.enabled;
    config_.output.sd.cs_pin  = sdJson["cs_pin"] | config_.output.sd.cs_pin;

    // Sección [business]
    JsonObject watchdog = doc["business"]["watchdog"];
    uint32_t interval_s = watchdog["check_interval_seg"] | (config_.business.watchdog.check_interval_ms / 1000);
    config_.business.watchdog.check_interval_ms = interval_s * 1000;

    JsonObject rules = watchdog["rules"];
    config_.business.watchdog.temp_rule.min_val = rules["temp"]["min_val"] | config_.business.watchdog.temp_rule.min_val;
    config_.business.watchdog.temp_rule.max_val = rules["temp"]["max_val"] | config_.business.watchdog.temp_rule.max_val;
    config_.business.watchdog.temp_rule.alert_sec = rules["temp"]["alert_sec"] | config_.business.watchdog.temp_rule.alert_sec;
    config_.business.watchdog.temp_rule.hist_items = rules["temp"]["hist_items"] | config_.business.watchdog.temp_rule.hist_items;
    config_.business.watchdog.temp_rule.hist_age_sec = rules["temp"]["hist_age_sec"] | config_.business.watchdog.temp_rule.hist_age_sec;

    config_.business.watchdog.spo2_rule.min_val = rules["spo2"]["min_val"] | config_.business.watchdog.spo2_rule.min_val;
    config_.business.watchdog.spo2_rule.max_val = rules["spo2"]["max_val"] | config_.business.watchdog.spo2_rule.max_val;
    config_.business.watchdog.spo2_rule.alert_sec = rules["spo2"]["alert_sec"] | config_.business.watchdog.spo2_rule.alert_sec;
    config_.business.watchdog.spo2_rule.hist_items = rules["spo2"]["hist_items"] | config_.business.watchdog.spo2_rule.hist_items;
    config_.business.watchdog.spo2_rule.hist_age_sec = rules["spo2"]["hist_age_sec"] | config_.business.watchdog.spo2_rule.hist_age_sec;
    
    config_.business.watchdog.bpm_rule.min_val = rules["bpm"]["min_val"] | config_.business.watchdog.bpm_rule.min_val;
    config_.business.watchdog.bpm_rule.max_val = rules["bpm"]["max_val"] | config_.business.watchdog.bpm_rule.max_val;
    config_.business.watchdog.bpm_rule.alert_sec = rules["bpm"]["alert_sec"] | config_.business.watchdog.bpm_rule.alert_sec;
    config_.business.watchdog.bpm_rule.hist_items = rules["bpm"]["hist_items"] | config_.business.watchdog.bpm_rule.hist_items;
    config_.business.watchdog.bpm_rule.hist_age_sec = rules["bpm"]["hist_age_sec"] | config_.business.watchdog.bpm_rule.hist_age_sec;

    loaded_ = true;
    logger.log(LOG_INFO, "Configuración estructurada cargada desde '%s'", filepath);
    return true;
}

// Devuelve una referencia constante a la configuración cargada
const AppConfig& ConfigManager::getConfig() const {
    return config_;
}