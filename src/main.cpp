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

    Watchdog::instance().setWatch(
        /* type      */ MEAS_TEMPERATURE,
        /* min_val   */ 36.0f,
        /* max_val   */ 38.5f,
        /* alert_sec */ 300,      // Alerta tras 5 minutos
        /* hist_items*/ 60,       // 30 mins de historial (1 med c/30s)
        /* hist_age  */ 3600      // Max 1 hora de antigüedad
    );

    Watchdog::instance().setWatch(
        /* type      */ MEAS_SPO2,
        /* min_val   */ 92.0f,
        /* max_val   */ 100.0f,
        /* alert_sec */ 120,      // Alerta tras 2 minutos
        /* hist_items*/ 60,
        /* hist_age  */ 3600
    );

    Watchdog::instance().setWatch(
        /* type      */ MEAS_BPM,
        /* min_val   */ 50.0f,
        /* max_val   */ 110.0f,
        /* alert_sec */ 180,      // Alerta tras 3 minutos
        /* hist_items*/ 60,
        /* hist_age  */ 3600
    );

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
