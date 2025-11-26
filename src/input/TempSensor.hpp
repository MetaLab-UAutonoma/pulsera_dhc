#pragma once
#include <Arduino.h>
#include "config.hpp"
#include "config/AppConfig.hpp"
#include "utils/logger.hpp"
#include "input/Sensor.hpp"

// --- CAMBIO: Librerías para DS18B20 ---
#include <OneWire.h>
#include <DallasTemperature.h>

class TempSensor : public Sensor {
public:
    TempSensor(int p_pinSensor, const TempSensorConfig& config);

    void init() override;
    void update(uint32_t p_now) override;

private:
    const TempSensorConfig& config_;
    int           pinSensor_;
    bool          active_;
    uint32_t      tsTemp_;
    uint32_t      tsWaitingTemp_;
    uint32_t      tsRetry_ = 0;
    uint32_t      tsLeerTemp_;
    float         lastReadTemp_ = 0.0f; 
    State         state_;

    // --- CAMBIO: Objetos para DS18B20 ---
    OneWire oneWire_;
    DallasTemperature sensors_;
};