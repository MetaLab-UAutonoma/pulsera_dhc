// Watchdog.cpp
#include "Watchdog.hpp"
#include "utils/logger.hpp"

// Intervalo para revisar las reglas
#define WATCHDOG_CHECK_INTERVAL_MS 5000

Watchdog& Watchdog::instance() {
    static Watchdog wd_instance; // Se crea una sola vez
    return wd_instance;
}

Watchdog::Watchdog() : manager_(MeasurementManager::instance()) {
}

void Watchdog::setWatch(MeasurementType type, float min_val, float max_val,
                        uint32_t alert_duration_sec, size_t history_items,
                        uint32_t history_age_sec) {
                            
    manager_.configure(type, history_items, history_age_sec);
    
    WatchdogRule new_rule = {
        .min_threshold = min_val,
        .max_threshold = max_val,
        .alert_duration_sec = alert_duration_sec,
        .is_alert_active = false,
        .alert_condition_since = 0
    };
    rules_[type] = new_rule;

    logger.log(LOG_INFO, "Watchdog para '%s' configurado: min=%.1f, max=%.1f, alert_after=%usec",
        measurementTypeToString(type), min_val, max_val, alert_duration_sec);
}

void Watchdog::update(uint32_t now) {
    if (now - tsLastCheck_ >= WATCHDOG_CHECK_INTERVAL_MS) {
        tsLastCheck_ = now;
        logger.log(LOG_DEBUG, "Watchdog: Ejecutando validaciones...");

        for (auto& pair : rules_) {
            validate(pair.first, pair.second);
        }
    }
}

void Watchdog::validate(MeasurementType type, WatchdogRule& rule) {
    const MeasurementList* history = manager_.getHistory(type);
    if (!history || history->getData().empty()) {
        return;
    }

    const auto& last_measurement = history->getData().back();
    const char* name = measurementTypeToString(type);

    // Verificar si el último valor está fuera de los umbrales
    bool is_out_of_bounds = (last_measurement.value < rule.min_threshold ||
                             last_measurement.value > rule.max_threshold);

    if (is_out_of_bounds) {
        if (rule.alert_condition_since == 0) {
            rule.alert_condition_since = last_measurement.timestamp;
            logger.log(LOG_INFO, "Watchdog [%s]: Condición de alerta iniciada (valor: %.1f)", name, last_measurement.value);
        } else {
            if (!rule.is_alert_active && (time(nullptr) - rule.alert_condition_since) >= rule.alert_duration_sec) {
                rule.is_alert_active = true;
                logger.log(LOG_WARN, "¡ALERTA [%s]! Valor (%.1f) fuera de rango (%.1f-%.1f) por más de %u seg.",
                           name, last_measurement.value, rule.min_threshold, rule.max_threshold, rule.alert_duration_sec);
            }
        }
    } else {
        if (rule.is_alert_active) {
            logger.log(LOG_INFO, "Watchdog [%s]: Condición de alerta resuelta.", name);
        }
        rule.is_alert_active = false;
        rule.alert_condition_since = 0;
    }
}