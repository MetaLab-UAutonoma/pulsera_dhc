#include "WatchdogRuleGps.hpp"


WatchdogRuleGps::WatchdogRuleGps(float min_val, float max_val, uint32_t alert_duration_sec,
                                 size_t history_items, uint32_t history_age_sec)
    : ThresholdWatchdogRule(
          MeasurementManager::instance(), // 1. Inyectamos la dependencia del Singleton aquí
          MEAS_GPS_SATS,                  // <--- CORREGIDO: Monitorea el número de satélites
          "GPS Sats",                     // 3                        // 3. Definimos el NOMBRE para los logs
          min_val,                        // 4. Pasamos el resto de umbrales
          max_val,
          alert_duration_sec,
          history_items,
          history_age_sec) 
    
{

}
