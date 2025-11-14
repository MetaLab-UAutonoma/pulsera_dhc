#pragma once
#include <Arduino.h>

/**
 * Sensor para leer el nivel de batería Li-ion (3.0V–4.2V)
 * mediante un divisor resistivo conectado al ADC del ESP32.
 */
class BatterySensor {
public:
    BatterySensor(int adcPin,
                  float r1 = 100000.0f,
                  float r2 = 100000.0f);

    void begin();

    // Voltaje real de batería (no dividido)
    float readVoltage();

    // Porcentaje estimado (0%–100%)
    int readPercentage();

private:
    int _adcPin;
    float _r1;
    float _r2;

    float _rawToBatteryVoltage(int raw);
};