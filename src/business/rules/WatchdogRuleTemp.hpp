#pragma once

#include "business/rules/IWatchdogRule.hpp"

class WatchdogRuleTemp : public IWatchdogRule {
public:
    /**
     * Constructor para la regla de temperatura.
     * @param min_val Umbral mínimo.
     * @param max_val Umbral máximo.
     * @param alert_duration_sec Segundos de persistencia para alertar.
     * @param history_items Cantidad de datos para el historial.
     * @param history_age_sec Antigüedad máxima de los datos.
     */
    WatchdogRuleTemp(float min_val, float max_val, uint32_t alert_duration_sec,
                     size_t history_items, uint32_t history_age_sec);

    void validate() override;
    void configureManager(MeasurementManager& manager) override;

private:
    // Dependencia
    MeasurementManager& manager_;

    // Configuración específica de la regla
    float    min_threshold_;
    float    max_threshold_;
    uint32_t alert_duration_sec_;
    size_t   history_items_;
    uint32_t history_age_sec_;

    // Estado interno
    bool   is_alert_active_ = false;
    time_t alert_condition_since_ = 0;
};