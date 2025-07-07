// input/sensor_temp.hpp
#pragma once
#include <Arduino.h>
#include "config.hpp"
#include "config/AppConfig.hpp" // Para tener la struct
#include "utils/logger.hpp"
#include "input/Sensor.hpp"

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
    uint32_t      tsLeerTemp_;
    float         sumTemp_;
    int           countTemp_;
    State         state_;
    float         tempMin_;
    float         tempMax_;
    bool          alertT_;
};
