// Watchdog.hpp
#pragma once

#include "MeasurementManager.hpp"
#include "output/Sim7600Modem.hpp" // Para poder enviar alertas

struct WatchdogRule {
    // --- Configuración ---
    float       min_threshold;
    float       max_threshold;
    uint32_t    alert_duration_sec;
    size_t      history_items
;

    // --- Estado dinámico para la lógica de alerta ---
    bool     is_alert_active;
    time_t   alert_condition_since; // Timestamp de cuándo empezó la condición
};

/// Define las reglas de negocio y genera alertas basadas en el historial.
class Watchdog {
public:
    static Watchdog& instance();


    void setWatch(MeasurementType type, float min_val, float max_val,
                  uint32_t alert_duration_sec, size_t history_items,
                  uint32_t history_age_sec);
    void update(uint32_t now);

private:

    Watchdog();
    ~Watchdog() = default;
    Watchdog(const Watchdog&) = delete;
    Watchdog& operator=(const Watchdog&) = delete;

    void validate(MeasurementType type, WatchdogRule& rule);

    MeasurementManager& manager_;
    
    uint32_t tsLastCheck_ = 0;
    std::map<MeasurementType, WatchdogRule> rules_;
};