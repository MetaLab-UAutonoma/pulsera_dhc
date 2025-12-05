#include "config.hpp"

// ——— Definiciones de los extern ——— un cambio
HardwareSerial simSerial(1);
HardwareSerial gpsSerial(2); 
const char*    telefonoDestino = "+56950506913";

bool  debugMode = false, lastDebugMode = false;

