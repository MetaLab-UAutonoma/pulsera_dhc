#include "WatchdogRuleBattery.hpp"

WatchdogRuleBattery::WatchdogRuleBattery(MeasurementManager &manager)
    : _manager(manager) {}

WatchdogRule::RuleResult WatchdogRuleBattery::evaluate() {

    // Obtener mediciones desde MeasurementManager
    float voltage = _manager.getLast("battery_voltage");
    float percent = _manager.getLast("battery_percent");

    // Si no hay datos aún -> OK
    if (percent <= 0 || voltage <= 0) {
        return RuleResult::OK;
    }

    // ---- ALERTA CRÍTICA ----
    if (percent < 10 || voltage < 3.50f) {
        return RuleResult::CRITICAL;
    }

    // ---- ADVERTENCIA ----
    if (percent < 15 || voltage < 3.65f) {
        return RuleResult::WARNING;
    }

    // ---- BAJA --- (opcional)
    if (percent < 30) {
        return RuleResult::WARNING;
    }

    // ---- OK NORMAL ----
    return RuleResult::OK;
}