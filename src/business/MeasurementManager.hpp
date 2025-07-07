// measurement_manager.hpp
#pragma once

#include <Arduino.h>
#include <map>
#include "utils/shared.hpp"
#include "utils/logger.hpp"
#include "business/MeasurementList.hpp"

/// Tipos de medición
enum MeasurementType {
    MEAS_TEMPERATURE,
    MEAS_SPO2,
    MEAS_BPM
};

const char* measurementTypeToString(MeasurementType t);

/// Almacena la última medición y sus límites
struct MeasurementEntry {
    time_t ts;      // timestamp 
    float  value;   // último valor medido
};

/// Singleton que gestiona todas las mediciones y validaciones
class MeasurementManager {
public:
    /// singleton
    static MeasurementManager& instance();

    void addMeasurement(MeasurementType type, float val);
    const MeasurementList* getHistory(MeasurementType type) const;
    void configure(MeasurementType type, size_t max_items, uint32_t max_age_sec);


private:
    MeasurementManager() = default;
    ~MeasurementManager() = default;

    // No permitimos copiar ni mover
    MeasurementManager(const MeasurementManager&) = delete;
    MeasurementManager& operator=(const MeasurementManager&) = delete;
    MeasurementManager(MeasurementManager&&) = delete;
    MeasurementManager& operator=(MeasurementManager&&) = delete;

    std::map<MeasurementType, MeasurementList> entries_;
};
