// main.cpp
#include "main.hpp"

// ——— Instancias de los módulos (POO) ———
PulseOximeter pox;                                           // Para PoxSensor
PoxMax30100Sensor poxSensor(pox);
TempSensor    tempSensor(PIN_SENSOR_TEMP, 0.0f, 0.0f);
Sim7600Modem  modem(simSerial, SIM_RX, SIM_TX, telefonoDestino);

// ——— Pines de salida (no encapsulados) ———
const int outputPins[2] = {32, 33};

void setup() {
    Serial.begin(115200);
    logger.init("pool.ntp.org", -4 * 3600, 0);

    pinMode(DEBUG_PIN, INPUT);
    Wire.begin();

    modem.init();

    for (auto p : outputPins) {
        pinMode(p, OUTPUT);
    }

    Watchdog& watchdog = Watchdog::instance();

    auto temp_rule = std::make_unique<WatchdogRuleTemp>(
        /* min_val   */ 36.0f,
        /* max_val   */ 38.5f,
        /* alert_sec */ 300,
        /* hist_items*/ 60,
        /* hist_age  */ 3600
    );
    watchdog.addRule(MEAS_TEMPERATURE,std::move(temp_rule));

    auto spo2_rule = std::make_unique<WatchdogRuleSpO2>(
        /* min_val   */ 92.0f,
        /* max_val   */ 100.0f,
        /* alert_sec */ 120,      // Alerta tras 2 minutos
        /* hist_items*/ 60,
        /* hist_age  */ 3600
    );
    watchdog.addRule(MEAS_SPO2,std::move(spo2_rule));

    auto bpm_rule = std::make_unique<WatchdogRuleBPM>(
        /* min_val   */ 50.0f,
        /* max_val   */ 110.0f,
        /* alert_sec */ 180,      // Alerta tras 3 minutos
        /* hist_items*/ 60,
        /* hist_age  */ 3600
    );
    watchdog.addRule(MEAS_BPM,std::move(bpm_rule));

    logger.log(LOG_INFO, "== Sistema iniciado ==");
}

void loop() {
    uint32_t now = millis();

    // Mantenimiento de debugMode (queda en main)
    debugMode = (digitalRead(DEBUG_PIN) == HIGH);
    if (debugMode != lastDebugMode) {
        logger.log(LOG_INFO, ">> %s debug mode",
                   debugMode ? "ACTIVADO" : "DESACTIVADO");
        lastDebugMode = debugMode;
    }

    // Delego la lógica a los módulos POO
    poxSensor.update(now);
    tempSensor.update(now);
    modem.update(now);

    Watchdog::instance().update(now);
}
