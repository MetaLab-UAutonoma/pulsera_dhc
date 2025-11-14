#pragma once 

#include "business/rules/WatchdogRuleBattery.hpp"

WatchdogRuleBattery::WatchdogRuleBattery(float min_val, float max_val, uint32_t alert_duration_sec,
                                 size_t history_items, uint32_t history_age_sec)
    : manager_(MeasurementManager::instance()),
      min_threshold_(min_val),
      max_threshold_(max_val),
      alert_duration_sec_(alert_duration_sec),
      history_items_(history_items),
      history_age_sec_(history_age_sec) {}

void WatchdogRuleBattery::validate() {

    // Obtener mediciones desde MeasurementManager
    float voltage = manager_.getLast(MeasurementType::MEAS_BATTERY_VOLTAGE);
    float percent = manager_.getLast(MeasurementType::MEAS_BATTERY_PERCENT);
    /*
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
    */   
}

void WatchdogRuleBattery::configureManager(MeasurementManager& manager) {
    //manager.configure(MEAS_BPM, history_items_, history_age_sec_);
}