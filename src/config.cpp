#include "config.hpp"

// ——— Definiciones de los extern ——— un cambio
HardwareSerial simSerial(1);
const char*    telefonoDestino = "+56912345678";

bool  debugMode = false, lastDebugMode = false;
