// input/sensor_temp.hpp
#pragma once
#include <Arduino.h>
#include "config.hpp"
#include "utils/logger.hpp"
#include "input/Sensor.hpp"

class TempSensor : public Sensor {
public:
    /**
     * @param p_pinSensor pin analógico
     * @param p_tempMin   umbral mínimo (a futuro)
     * @param p_tempMax   umbral máximo (a futuro)
     */
    TempSensor(int p_pinSensor, float p_tempMin, float p_tempMax);

    void init() override;
    void update(uint32_t p_now) override;

private:
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
