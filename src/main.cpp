// main.cpp
#include "main.hpp"
#include "output/SDStorage.hpp"

//bool a = ConfigManager::instance().loadFromFile("/config/config.json");
const AppConfig& config = ConfigManager::instance().getConfig();

// ——— Instancias de los módulos (POO) ———
PulseOximeter pox;                                           // Para PoxSensor

PoxMax30100Sensor poxSensor(pox, config.input.pox_sensor);
TempSensor    tempSensor(PIN_SENSOR_TEMP, config.input.temp_sensor);
BatterySensor batterySensor(PIN_BATTERY_ADC, config.input.bat_sensor);

Sim7600Modem  modem(simSerial, SIM_RX, SIM_TX, config.general.telefono_destino.c_str(), config.output.modem);
GpsUblox      gpsSensor(gpsSerial, GPS_RX, GPS_TX);
//BatterySensorAdapter batteryInput(batterySensor);


void setup() {
    Serial.begin(115200);
    ConfigManager::instance().loadFromFile("/config/config.json");

    logger.init(config.ntp.server.c_str(), config.ntp.gmt_offset_sec, config.ntp.daylight_offset_sec);

    pinMode(DEBUG_PIN, INPUT);
    Wire.begin();
    modem.init();
   auto sms_sender = std::unique_ptr<SmsSender>(new SmsSender(modem));
    AlertDispatcher::instance().addSender(std::move(sms_sender));
    SDStorage::instance().init(config.output.sd);
    gpsSensor.init(); 

    Watchdog& watchdog = Watchdog::instance();
    const auto& wd_rules_config = config.business.watchdog; // Alias para legibilidad

    // Crear Regla de Temperatura
    auto temp_rule = std::make_unique<WatchdogRuleTemp>(
        wd_rules_config.temp_rule.min_val,
        wd_rules_config.temp_rule.max_val,
        wd_rules_config.temp_rule.alert_sec,
        wd_rules_config.temp_rule.hist_items,
        wd_rules_config.temp_rule.hist_age_sec
    );
    watchdog.addRule(MEAS_TEMPERATURE, std::move(temp_rule));

    // Crear Regla de SpO2
    auto spo2_rule = std::make_unique<WatchdogRuleSpO2>(
        wd_rules_config.spo2_rule.min_val,
        wd_rules_config.spo2_rule.max_val,
        wd_rules_config.spo2_rule.alert_sec,
        wd_rules_config.spo2_rule.hist_items,
        wd_rules_config.spo2_rule.hist_age_sec
    );
    watchdog.addRule(MEAS_SPO2, std::move(spo2_rule));

     auto battery_rule = std::make_unique<WatchdogRuleBattery>(
        wd_rules_config.battery_rule.min_val,
        wd_rules_config.battery_rule.max_val,
        wd_rules_config.battery_rule.alert_sec,
        wd_rules_config.battery_rule.hist_items,
        wd_rules_config.battery_rule.hist_age_sec
    );
    watchdog.addRule(MEAS_BATTERY_PERCENT, std::move(battery_rule));
    MeasurementManager::instance().configure(MEAS_BATTERY_VOLTAGE, 60, 3600);


    // Crear Regla de BPM
    auto bpm_rule = std::make_unique<WatchdogRuleBPM>(
        wd_rules_config.bpm_rule.min_val,
        wd_rules_config.bpm_rule.max_val,
        wd_rules_config.bpm_rule.alert_sec,
        wd_rules_config.bpm_rule.hist_items,
        wd_rules_config.bpm_rule.hist_age_sec
    );
    watchdog.addRule(MEAS_BPM,std::move(bpm_rule));

     // Crear Regla de Gps
    auto gps_rule = std::make_unique<WatchdogRuleGps>(
        wd_rules_config.gps_rule.min_val,
        wd_rules_config.gps_rule.max_val,
        wd_rules_config.gps_rule.alert_sec,
        wd_rules_config.gps_rule.hist_items,
        wd_rules_config.gps_rule.hist_age_sec
    );
    watchdog.addRule(MEAS_GPS_SATS, std::move(gps_rule));
    

    logger.log(LOG_INFO, "== Sistema iniciado ==");
}

void loop() {
    uint32_t now = millis();

    debugMode = (digitalRead(DEBUG_PIN) == HIGH);
    if (debugMode != lastDebugMode) {
        logger.log(LOG_INFO, ">> %s debug mode", debugMode ? "ACTIVADO" : "DESACTIVADO");
        lastDebugMode = debugMode;
    }

    // Delego la lógica a los módulos POO
    poxSensor.update(now);
    tempSensor.update(now);
    batterySensor.update(now);
    gpsSensor.update(now);
    
    modem.update(now);

    Watchdog::instance().update(now);
}
