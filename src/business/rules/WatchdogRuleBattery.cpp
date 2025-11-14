#pragma once

#include "../MeasurementManager.hpp"
#include "../WatchdogRule.hpp"

/**
 * Regla de batería para alerta temprana.
 * 
 * - WARNING  < 30%
 * - CRITICAL < 15%
 * - ALERT    < 10%  (dispara alerta)
 */
class WatchdogRuleBattery : public WatchdogRule {
public:
    WatchdogRuleBattery(MeasurementManager &manager);

    virtual RuleResult evaluate() override;

private:
    MeasurementManager &_manager;
};