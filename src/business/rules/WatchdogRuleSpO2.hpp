#pragma once

#include "business/rules/IWatchdogRule.hpp"
#include "business/MeasurementManager.hpp"

// Adelante declaración si no la tienes
class Sim7600Modem;

class WatchdogRuleSpO2 : public IWatchdogRule {
public:
    WatchdogRuleSpO2(float min_val, float max_val, uint32_t alert_duration_sec,
                     size_t history_items, uint32_t history_age_sec,
                     Sim7600Modem* modem);  // ✅ DEBE ESTAR ESTE CONSTRUCTOR

    void configureManager(MeasurementManager& manager) override;
    void validate() override;

private:
    MeasurementManager& manager_;
    float min_threshold_;
    float max_threshold_;
    uint32_t alert_duration_sec_;
    size_t history_items_;
    uint32_t history_age_sec_;
    time_t alert_condition_since_ = 0;
    bool is_alert_active_ = false;
    Sim7600Modem* modem_;  // ✅ Asegúrate de tener este puntero como miembro
};
