#pragma once

#include "business/rules/ThresholdWatchdogRule.hpp"

class WatchdogRuleSpO2 : public ThresholdWatchdogRule {
public:
    WatchdogRuleSpO2(float min_val, float max_val, uint32_t alert_duration_sec,
                     size_t history_items, uint32_t history_age_sec);

    /*void validate() override;
    void configureManager(MeasurementManager& manager) override;

private:
    MeasurementManager& manager_;
    float    min_threshold_;
    float    max_threshold_;
    uint32_t alert_duration_sec_;
    size_t   history_items_;
    uint32_t history_age_sec_;
    bool   is_alert_active_ = false;
    time_t alert_condition_since_ = 0;
    */
};