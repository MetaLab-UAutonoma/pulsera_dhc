#pragma once

#include "business/rules/ThresholdWatchdogRule.hpp"

class WatchdogRuleSpO2 : public ThresholdWatchdogRule {
public:
    WatchdogRuleSpO2(float min_val, float max_val, uint32_t alert_duration_sec,
                     size_t history_items, uint32_t history_age_sec);

    
};