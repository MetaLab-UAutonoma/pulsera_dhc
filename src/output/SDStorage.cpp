// src/output/SDStorage.cpp
#include "output/SDStorage.hpp"
#include "utils/logger.hpp"
#include "utils/shared.hpp" // Para timestamp string
#include <SPI.h>

SDStorage& SDStorage::instance() {
    static SDStorage inst;
    return inst;
}

bool SDStorage::init(const SDConfig& config) {
    enabled_ = config.enabled;
    if (!enabled_) {
        logger.log(LOG_INFO, "SD: Deshabilitada por configuración.");
        return false;
    }
    SPI.begin(18, 19, 23, config.cs_pin);
    
    if (!SD.begin(config.cs_pin, SPI)) {
        logger.log(LOG_ERROR, "SD: Falló inicialización (Card Mount Failed).");
        return false;
    }

    uint8_t cardType = SD.cardType();
    if (cardType == CARD_NONE) {
        logger.log(LOG_ERROR, "SD: No hay tarjeta insertada.");
        return false;
    }

    logger.log(LOG_INFO, "SD: Inicializada correctamente.");
    return true;
}

void SDStorage::saveMeasurement(MeasurementType type, float value) {
    if (!enabled_) return;

    // Creamos una línea CSV: FECHA-HORA, TIPO, VALOR
    String line = shared.tsToString(time(nullptr));
    line += ",";
    line += measurementTypeToString(type); // Función global en MeasurementManager.cpp
    line += ",";
    line += String(value);
    line += "\n";

    // Opción A: Un solo archivo grande 'data.csv'
    appendFile("/data.csv", line.c_str());
    
    // Opción B: Si prefieres separar por tipo, descomenta esto:
    // String path = "/" + String(measurementTypeToString(type)) + ".csv";
    // appendFile(path.c_str(), line.c_str());
}

void SDStorage::appendFile(const char* path, const char* message) {
    File file = SD.open(path, FILE_APPEND);
    if (!file) {
        logger.log(LOG_WARN, "SD: No se pudo abrir %s para escribir", path);
        return;
    }
    if (!file.print(message)) {
        logger.log(LOG_WARN, "SD: Falló escritura en %s", path);
    }
    file.close();
}