#pragma once
#include <Arduino.h>

#include "config.hpp"
#include "config/AppConfig.hpp"

#include "utils/logger.hpp"
#include "input/Sensor.hpp"

#include "MAX30100_PulseOximeter.h"

class PoxMax30100Sensor : public Sensor {
public:
    explicit PoxMax30100Sensor(PulseOximeter& p_pox, const PoxSensorConfig& config);
    void init() override;
    void update(uint32_t p_now) override;

private:
    const PoxSensorConfig&  config_;
    PulseOximeter&          pox_;
    bool                    active_;
    uint32_t                tsPulso_;
    uint32_t                tsWaitingPulso_;
    uint32_t                tsLeerPulso_;
    float                   sumBpm_;
    float                   sumSpo2_;
    int                     countPulso_;
    State                   state_;
};
