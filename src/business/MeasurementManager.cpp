// measurement_manager.cpp
#include "business/MeasurementManager.hpp"

const char* measurementTypeToString(MeasurementType t) {
    switch (t) {
        case MEAS_TEMPERATURE: return "Temp";
        case MEAS_SPO2:        return "SpO2";
        case MEAS_BPM:         return "BPM";
        default:               return "Unknown";
    }
}

// Claves ordenadas para validar en bucle
static const MeasurementType kAllTypes[] = {
    MEAS_TEMPERATURE,
    MEAS_SPO2,
    MEAS_BPM
};

MeasurementManager& MeasurementManager::instance() {
    static MeasurementManager mgr;
    return mgr;
}

void MeasurementManager::setRange(MeasurementType type, float lo, float hi) {
    auto& e = entries_[type];
    e.min   = lo;
    e.max   = hi;
    e.value = NAN;   // sin valor inicial
    e.ts    = "";
    logger.log(LOG_INFO,
      "Rango %d configurado [%.2f .. %.2f]", type, lo, hi);
}

void MeasurementManager::addMeasurement(MeasurementType type,
                                        const String& ts,
                                        float val) {
    auto &e = entries_[type];
    e.ts    = ts;
    e.value = val;
    logger.log(LOG_DEBUG,
      "addMeasurement(%d): [%s] = %.2f",
      type, ts.c_str(), val);
}

void MeasurementManager::validateAll(bool modemActivo) {
    for (auto type : kAllTypes) {
        auto it = entries_.find(type);
        if (it == entries_.end()) continue;
        const auto &e = it->second;
        if (isnan(e.value)) {
            logger.log(LOG_WARN,
                "No hay lecturas para tipo %d", type);
            continue;
        }
        const char* name = measurementTypeToString(type);

        static std::map<MeasurementType,bool> alerted;

        // TODO verificarYAlertar
    }
}

const std::map<MeasurementType, MeasurementEntry>&
MeasurementManager::data() const {
    return entries_;
}
