#include "business/rules/ThresholdWatchdogRule.hpp"

ThresholdWatchdogRule::ThresholdWatchdogRule(
    MeasurementManager& manager, MeasurementType type, const char* name,
    float min_val, float max_val, uint32_t alert_duration_sec,
    size_t history_items, uint32_t history_age_sec)
    : manager_(manager),
      type_(type),
      name_(name),
      min_threshold_(min_val),
      max_threshold_(max_val),
      alert_duration_sec_(alert_duration_sec),
      history_items_(history_items),
      history_age_sec_(history_age_sec) {}


void ThresholdWatchdogRule::configureManager(MeasurementManager& manager) {
    manager.configure(type_, history_items_, history_age_sec_);
}


void ThresholdWatchdogRule::validate() {
    const MeasurementList* history = manager_.getHistory(type_);
    if (!history || history->getData().empty()) {
        return; 
    }

    const auto& last = history->getData().back();
    const bool is_out_of_bounds = (last.value < min_threshold_ || last.value > max_threshold_);

    if (is_out_of_bounds) {
        if (alert_condition_since_ == 0) {
            
            alert_condition_since_ = last.timestamp;
            logger.log(LOG_INFO, "Watchdog [%s]: Condición iniciada (valor: %.1f)", name_, last.value);
        } else {
            
            if (!is_alert_active_ && (time(nullptr) - alert_condition_since_) >= alert_duration_sec_) {
                is_alert_active_ = true;
                logger.log(LOG_WARN, "¡ALERTA [%s]! Valor fuera de rango por más de %u seg.", name_, alert_duration_sec_);
                
            }
        }
    } else {
        
        if (is_alert_active_) {
            logger.log(LOG_INFO, "Watchdog [%s]: Condición resuelta.", name_);
        }
        is_alert_active_ = false;
        alert_condition_since_ = 0;
    }
}