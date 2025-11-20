#pragma once

#include "business/rules/IWatchdogRule.hpp"
#include "business/MeasurementManager.hpp"
#include "utils/logger.hpp"
#include "output/AlertDispatcher.hpp"


class ThresholdWatchdogRule : public IWatchdogRule {
public:
    /**
     * Constructor para la regla de umbral genérica.
     * @param manager Instancia del MeasurementManager (Inyección de Dependencia).
     * @param type El tipo de medición que esta regla validará (ej. MEAS_TEMPERATURE).
     * @param name El nombre para usar en los logs (ej. "Temp", "BPM").
     * @param min_val Umbral mínimo.
     * @param max_val Umbral máximo.
     * @param alert_duration_sec Segundos de persistencia para alertar.
     * @param history_items Cantidad de datos para el historial.
     * @param history_age_sec Antigüedad máxima de los datos.
     */
    ThresholdWatchdogRule(MeasurementManager& manager, MeasurementType type, const char* name,
                          float min_val, float max_val, uint32_t alert_duration_sec,
                          size_t history_items, uint32_t history_age_sec);
    
    void validate() override;
    void configureManager(MeasurementManager& manager) override;

protected:
    MeasurementManager& manager_;
    MeasurementType     type_;
    const char* name_; 

    float    min_threshold_;
    float    max_threshold_;
    uint32_t alert_duration_sec_;
    size_t   history_items_;
    uint32_t history_age_sec_;


    bool   is_alert_active_ = false;
    time_t alert_condition_since_ = 0;
};