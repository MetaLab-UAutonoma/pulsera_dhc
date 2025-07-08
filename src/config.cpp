#include "config.hpp"
#include <LittleFS.h>
#include <ArduinoJson.h>
#include <HardwareSerial.h>
#include "utils/logger.hpp"  // si usas logger para debug


// ——— Definiciones de los extern ———


// Variables para debug mode
bool debugMode = false;
bool lastDebugMode = false;

// Variable para timer de chequeo (por ejemplo, para modem)
float timerCheckSeg = 5.0f;  // valor por defecto

HardwareSerial simSerial(1);
//const char*    telefonoDestino = "+56921686892";
String telefonoDestino = "";  // inicialización real


bool cargarConfiguracion() {
    LittleFS.format();  // ⚠️ Esto borra todo el sistema de archivos

    if (!LittleFS.begin()) {
        Serial.println("Error montando LittleFS");
        return false;
    }

    File file = LittleFS.open("/config.json", "r");
    if (!file) {
        Serial.println("No se pudo abrir /config.json");
        return false;
    }

    DynamicJsonDocument doc(4096);
    DeserializationError error = deserializeJson(doc, file);
    file.close();

    if (error) {
        Serial.print("Error leyendo JSON: ");
        Serial.println(error.f_str());
        return false;
    }

    // Leer telefono destino
    if (doc.containsKey("general") && doc["general"].containsKey("telefono_destino")) {
        telefonoDestino = doc["general"]["telefono_destino"].as<String>();
    }

    // Leer timer check seg para modem
    if (doc.containsKey("output") && doc["output"].containsKey("Sim7600Modem") &&
        doc["output"]["Sim7600Modem"].containsKey("timer_check_seg")) {
        timerCheckSeg = doc["output"]["Sim7600Modem"]["timer_check_seg"].as<float>();
    }

    Serial.println("Configuración cargada correctamente:");
    Serial.print("Telefono destino: ");
    Serial.println(telefonoDestino);
    Serial.print("Timer check seg: ");
    Serial.println(timerCheckSeg);

    return true;
}

