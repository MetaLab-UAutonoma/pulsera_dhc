#include "business/rules/WatchdogRuleSpO2.hpp"
#include "utils/logger.hpp"

WatchdogRuleSpO2::WatchdogRuleSpO2(float min_val, float max_val, uint32_t alert_duration_sec,
                                   size_t history_items, uint32_t history_age_sec)
    : ThresholdWatchdogRule(
          MeasurementManager::instance(), // 1. Inyectamos la dependencia del Singleton aquí
          MEAS_TEMPERATURE,               // 2. Definimos el TIPO de medición específico
          "Temp",                         // 3. Definimos el NOMBRE para los logs
          min_val,                        // 4. Pasamos el resto de umbrales
          max_val,
          alert_duration_sec,
          history_items,
          history_age_sec) 
{
    
}