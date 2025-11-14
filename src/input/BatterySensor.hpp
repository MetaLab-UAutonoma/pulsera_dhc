#pragma once
#include <Arduino.h>

#include "config.hpp"
#include "config/AppConfig.hpp"

#include "utils/logger.hpp"
#include "input/Sensor.hpp"

/**
 * Sensor para leer el nivel de batería Li-ion (3.0V–4.2V)
 * mediante un divisor resistivo conectado al ADC del ESP32.
 */
class BatterySensor : public Sensor{
public:
    BatterySensor(int adcPin, const BatterySensorConfig& config);

    void begin();

    float readVoltage();
    int readPercentage();

    void init() override;
    void update(uint32_t p_now) override;


private:
    int _adcPin;
    float _r1;
    float _r2;
    bool active_;

    uint32_t tsBat_;
    uint32_t tsWaitingBat_;
    uint32_t tsLeerBat_;

    State state_;

    const BatterySensorConfig&  config_;

    float _rawToBatteryVoltage(int raw); 
};