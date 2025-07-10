#include "business/rules/WatchdogRuleBPM.hpp"
#include "utils/logger.hpp"
#include <Arduino.h> // para millis()

WatchdogRuleBPM::WatchdogRuleBPM(float min_val, float max_val, uint32_t alert_duration_sec,
                                 size_t history_items, uint32_t history_age_sec)
    : manager_(MeasurementManager::instance()),
      min_threshold_(min_val),
      max_threshold_(max_val),
      alert_duration_sec_(alert_duration_sec),
      history_items_(history_items),
      history_age_sec_(history_age_sec),
      alert_condition_since_(0),
      is_alert_active_(false)
{}

void WatchdogRuleBPM::configureManager(MeasurementManager& manager) {
    manager.configure(MEAS_BPM, history_items_, history_age_sec_);
}

void WatchdogRuleBPM::validate() {
    const MeasurementList* history = manager_.getHistory(MEAS_BPM);
    if (!history || history->getData().empty()) return;

    const auto& last = history->getData().back();
    const bool is_out_of_bounds = (last.value < min_threshold_ || last.value > max_threshold_);
    const char* name = "BPM";

    // Usamos tiempo relativo en segundos desde inicio
    uint32_t nowSec = millis() / 1000;

    if (is_out_of_bounds) {
        if (alert_condition_since_ == 0) {
            alert_condition_since_ = last.timestamp; // o podría ser nowSec, según lógica
            logger.log(LOG_INFO, "Watchdog [%s]: Condición iniciada (valor: %.1f)", name, last.value);
        } else if (!is_alert_active_ && (nowSec - alert_condition_since_) >= alert_duration_sec_) {
            is_alert_active_ = true;
            logger.log(LOG_WARN, "¡ALERTA [%s]! Valor fuera de rango por más de %u seg.", name, alert_duration_sec_);
        }
    } else {
        if (is_alert_active_) {
            logger.log(LOG_INFO, "Watchdog [%s]: Condición resuelta.", name);
        }
        is_alert_active_ = false;
        alert_condition_since_ = 0;
    }
}
