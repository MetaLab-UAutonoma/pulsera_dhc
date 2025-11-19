// measurement_manager.cpp
#include "business/MeasurementManager.hpp"

const char* measurementTypeToString(MeasurementType t) {
    switch (t) {
        case MEAS_TEMPERATURE: return "Temp";
        case MEAS_SPO2:        return "SpO2";
        case MEAS_BPM:         return "BPM";
        case MEAS_BATTERY_PERCENT:  return "Battery %";
        case MEAS_BATTERY_VOLTAGE:  return "Battery V";
        case MEAS_GPS_LAT:        return "GPS_LAT";
        case MEAS_GPS_LON:        return "GPS_LON";
        case MEAS_GPS_SATS:       return "GPS_SATS";
        case MEAS_GPS_SPEED:      return "GPS_SPEED";
        default:               return "Unknown";
    }
}

// Claves ordenadas para validar en bucle
static const MeasurementType kAllTypes[] = {
    MEAS_TEMPERATURE,
    MEAS_SPO2,
    MEAS_BPM,
    MEAS_BATTERY_PERCENT,   
    MEAS_BATTERY_VOLTAGE,   
};

MeasurementManager& MeasurementManager::instance() {
    static MeasurementManager mgr;
    return mgr;
}

void MeasurementManager::configure(MeasurementType type, size_t max_items, uint32_t max_age_sec) {
    // Inserta o reemplaza la configuración para este tipo.
    entries_.erase(type);
    entries_.emplace(std::piecewise_construct,
                     std::forward_as_tuple(type),
                     std::forward_as_tuple(max_items, max_age_sec));
    logger.log(LOG_INFO, "Manager para '%s' configurado: max_items=%u, max_age=%usec",
        measurementTypeToString(type), max_items, max_age_sec);
}

void MeasurementManager::addMeasurement(MeasurementType type, float val) {
    auto it = entries_.find(type);
    if (it != entries_.end()) {
        it->second.add(val);
        logger.log(LOG_DEBUG, "addMeasurement(%s): %.2f", measurementTypeToString(type), val);
    } else {
        logger.log(LOG_WARN, "Manager para '%s' no configurado. Medición descartada.", measurementTypeToString(type));
    }
}

const MeasurementList* MeasurementManager::getHistory(MeasurementType type) const {
    auto it = entries_.find(type);
    if (it != entries_.end()) {
        return &it->second;
    }
    return nullptr;
}
