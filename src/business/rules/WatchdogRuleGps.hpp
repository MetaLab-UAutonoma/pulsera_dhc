#pragma once

#include "business/rules/ThresholdWatchdogRule.hpp"


class WatchdogRuleGps : public ThresholdWatchdogRule {
public:    
    WatchdogRuleGps(float min_val, float max_val, uint32_t alert_duration_sec,
                     size_t history_items, uint32_t history_age_sec);
    
   
};