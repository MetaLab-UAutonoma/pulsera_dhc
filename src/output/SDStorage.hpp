// src/output/SDStorage.hpp
#pragma once
#include <Arduino.h>
#include <SD.h>
#include <SPI.h>
#include "config/AppConfig.hpp"
#include "business/MeasurementManager.hpp" // Para el enum MeasurementType

class SDStorage {
public:
    static SDStorage& instance();

    // Inicializa la tarjeta SD
    bool init(const SDConfig& config);

    // Guarda una medición en formato CSV: Timestamp,Tipo,Valor
    void saveMeasurement(MeasurementType type, float value);

private:
    SDStorage() = default;
    bool enabled_ = false;
    String getFileName(MeasurementType type);
    void appendFile(const char* path, const char* message);
};