// main.cpp
#include "main.hpp"

// ——— Instancias de los módulos (POO) ———
PulseOximeter pox;                                           // Para PoxSensor
PoxMax30100Sensor poxSensor(pox);

TempSensor    tempSensor(PIN_SENSOR_TEMP, 0.0f, 0.0f);       // Umbrales no usados por ahora

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

    // (Aquí podrías añadir lógica de salidas binarias si la encapsulas luego)
}
