// measurement_manager.hpp
#pragma once

#include <Arduino.h>
#include <map>
#include "utils/shared.hpp"
#include "utils/logger.hpp"

/// Tipos de medición
enum MeasurementType {
    MEAS_TEMPERATURE,
    MEAS_SPO2,
    MEAS_BPM
};

const char* measurementTypeToString(MeasurementType t);

/// Almacena la última medición y sus límites
struct MeasurementEntry {
    String ts;      // timestamp "YYYY-MM-DD hh:mm:ss-mmm"
    float  value;   // último valor medido
    float  min;     // umbral mínimo
    float  max;     // umbral máximo
};

/// Singleton que gestiona todas las mediciones y validaciones
class MeasurementManager {
public:
    /// Devuelve la instancia única
    static MeasurementManager& instance();

    /// Establece los rangos para cada tipo antes de empezar
    void setRange(MeasurementType type, float lo, float hi);

    /// Agrega o actualiza la última medición (timestamp + valor)
    void addMeasurement(MeasurementType type, const String& ts, float val);

    /// Recorre todas las mediciones y alerta si fuera de rango
    void validateAll(bool modemActivo);

    /// Opcional: acceder a las entradas para exportar JSON, etc.
    const std::map<MeasurementType, MeasurementEntry>& data() const;

private:
    MeasurementManager() = default;
    ~MeasurementManager() = default;

    // No permitimos copiar ni mover
    MeasurementManager(const MeasurementManager&) = delete;
    MeasurementManager& operator=(const MeasurementManager&) = delete;
    MeasurementManager(MeasurementManager&&) = delete;
    MeasurementManager& operator=(MeasurementManager&&) = delete;

    std::map<MeasurementType, MeasurementEntry> entries_;
};
