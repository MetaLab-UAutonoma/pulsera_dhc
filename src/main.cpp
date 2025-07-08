#include "main.hpp"
#include <LittleFS.h>


PulseOximeter pox;
std::unique_ptr<PoxMax30100Sensor> poxSensor;
std::unique_ptr<TempSensor> tempSensor;
std::unique_ptr<Sim7600Modem> modem;

void setup() {
    
    Serial.begin(115200);
 
 
    // ⚠️ Montar el sistema de archivos si aún no lo hace ConfigManager
    Serial.println("Intentando abrir /config.json...");
    
    if (!ConfigManager::instance().loadFromFile("/config.json")) {
        Serial.println("❌ Error cargando configuración.");
        while (true);  // Detener ejecución si falla
    }

    const AppConfig& config = ConfigManager::instance().getConfig();

    logger.init(config.ntp.server.c_str(), config.ntp.gmt_offset_sec, config.ntp.daylight_offset_sec);

    pinMode(DEBUG_PIN, INPUT);
    Wire.begin();

    // ✅ Inicializar módulos usando config cargada
    poxSensor = std::make_unique<PoxMax30100Sensor>(pox, config.input.pox_sensor);
    tempSensor = std::make_unique<TempSensor>(PIN_SENSOR_TEMP, config.input.temp_sensor);
    modem = std::make_unique<Sim7600Modem>(
        simSerial,
        SIM_RX,
        SIM_TX,
        config.general.telefono_destino.c_str(),
        config.output.modem
    );

    modem->init();

    Watchdog& watchdog = Watchdog::instance();
    const auto& wd_rules_config = config.business.watchdog;

    auto temp_rule = std::make_unique<WatchdogRuleTemp>(
        wd_rules_config.temp_rule.min_val,
        wd_rules_config.temp_rule.max_val,
        wd_rules_config.temp_rule.alert_sec,
        wd_rules_config.temp_rule.hist_items,
        wd_rules_config.temp_rule.hist_age_sec
    );
    watchdog.addRule(MEAS_TEMPERATURE, std::move(temp_rule));

    auto spo2_rule = std::make_unique<WatchdogRuleSpO2>(
        wd_rules_config.spo2_rule.min_val,
        wd_rules_config.spo2_rule.max_val,
        wd_rules_config.spo2_rule.alert_sec,
        wd_rules_config.spo2_rule.hist_items,
        wd_rules_config.spo2_rule.hist_age_sec
    );
    watchdog.addRule(MEAS_SPO2, std::move(spo2_rule));

    auto bpm_rule = std::make_unique<WatchdogRuleBPM>(
        wd_rules_config.bpm_rule.min_val,
        wd_rules_config.bpm_rule.max_val,
        wd_rules_config.bpm_rule.alert_sec,
        wd_rules_config.bpm_rule.hist_items,
        wd_rules_config.bpm_rule.hist_age_sec
    );
    watchdog.addRule(MEAS_BPM, std::move(bpm_rule));

    logger.log(LOG_INFO, "== Sistema iniciado ==");
}

void loop() {
    uint32_t now = millis();

    debugMode = (digitalRead(DEBUG_PIN) == HIGH);
    if (debugMode != lastDebugMode) {
        logger.log(LOG_INFO, ">> %s debug mode", debugMode ? "ACTIVADO" : "DESACTIVADO");
        lastDebugMode = debugMode;
    }

    poxSensor->update(now);
    tempSensor->update(now);
    modem->update(now);

    Watchdog::instance().update(now);
}
