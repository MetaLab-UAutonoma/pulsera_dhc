#include "BatterySensor.hpp"

BatterySensor::BatterySensor(int adcPin, float r1, float r2)
    : _adcPin(adcPin), _r1(r1), _r2(r2) {}

void BatterySensor::begin() {
    analogReadResolution(12); // ADC del ESP32 (0–4095)
    pinMode(_adcPin, INPUT);
}

float BatterySensor::_rawToBatteryVoltage(int raw) {
    // ADC → voltaje en pin ADC
    float vAdc = (3.3f * raw) / 4095.0f;

    // Invertir divisor resistivo
    // Vbat = Vadc * ((R1 + R2) / R2)
    return vAdc * ((_r1 + _r2) / _r2);
}

float BatterySensor::readVoltage() {
    int raw = analogRead(_adcPin);

    // Pequeño filtro (4 lecturas promediadas)
    for (int i = 0; i < 3; i++) {
        raw = (raw + analogRead(_adcPin)) / 2;
        delay(2);
    }

    return _rawToBatteryVoltage(raw);
}

int BatterySensor::readPercentage() {
    float v = readVoltage();

    // Tabla aproximada estándar Li-ion
    if      (v >= 4.20f) return 100;
    else if (v >= 4.10f) return 90;
    else if (v >= 4.00f) return 80;
    else if (v >= 3.90f) return 70;
    else if (v >= 3.80f) return 60;
    else if (v >= 3.75f) return 50;
    else if (v >= 3.70f) return 40;
    else if (v >= 3.65f) return 30;
    else if (v >= 3.55f) return 20;
    else if (v >= 3.40f) return 10;
    else                 return 5;
}
