#include "BatterySensor.hpp"
#include "business/MeasurementManager.hpp"
BatterySensor::BatterySensor(int adcPin, const BatterySensorConfig& config)
    : _adcPin(adcPin), 
    _r1(0), 
    _r2(0), 
    config_(config),
    active_(false),
    tsBat_(0),
    tsWaitingBat_(0),
    tsLeerBat_(0),
    state_(State::APAGADO)
   {}

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


// pulsera_dhc/src/input/BatterySensor.cpp (Dentro de init())

void BatterySensor::init() {
    // Código de inicialización de la batería debe ir aquí
    begin(); // Llamar a la inicialización del ADC
    logger.log(LOG_INFO, "Sensor de Batería listo.");
}

void BatterySensor::update(uint32_t p_now) {
    logger.log(LOG_DEBUG, "Bat, estado=%s", Sensor::stateToString(state_));

    float b = 0.0f, s = 0.0f;
    switch (state_) {
        case State::APAGADO:
            if (p_now - tsBat_ >= SEG_A_MS(config_.ciclo_monitoreo_seg)) {
                logger.log(LOG_DEBUG, "APAGADO→WAITING");
                init();
                state_ = State::WAITING;
            }
            break;

        case State::WAITING:
            if (p_now - tsWaitingBat_ >= SEG_A_MS(config_.timer_estabilizacion_seg)) {
                logger.log(LOG_DEBUG, "WAITING→LEYENDO");
                state_       = State::LEYENDO;
            }
            break;

        case State::LEYENDO:
            if (active_) {
            }
            if (p_now - tsBat_ >= SEG_A_MS(config_.duracion_lectura_seg)) {
                logger.log(LOG_DEBUG, "LEYENDO→ANALIZANDO");
                state_ = State::ANALIZANDO;
            }
            break;


        case State::ANALIZANDO:
            float voltage = readVoltage();
            int percent = readPercentage();
            
            logger.log(LOG_INFO, "Batería: %.2fV / %d%%", voltage, percent);
            
            // --- CÓDIGO AÑADIDO ---
            MeasurementManager::instance().addMeasurement(MEAS_BATTERY_VOLTAGE, voltage);
            MeasurementManager::instance().addMeasurement(MEAS_BATTERY_PERCENT, (float)percent);
            // ----------------------

            state_   = State::APAGADO;
            break;
    }
}


