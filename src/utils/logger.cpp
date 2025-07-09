#include "logger.hpp"
#include <Arduino.h>

extern bool debugMode;  // Declaración externa: definida en main.cpp

/// Definición del singleton
Logger logger;

void Logger::init(const char* p_ntpServer, long p_gmtOffset_sec, int p_daylightOffset_sec) {
    // Configura el NTP para sincronizar la hora con Internet
    configTime(p_gmtOffset_sec, p_daylightOffset_sec, p_ntpServer);
}

const char* Logger::levelToString(LogLevel p_level) {
    // Convierte el nivel de log a una cadena legible
    switch (p_level) {
        case LOG_INFO:  return "INFO ";
        case LOG_DEBUG: return "DEBUG";
        case LOG_WARN:  return "WARN ";
        case LOG_ERROR: return "ERROR";
        default:        return "UNK  ";
    }
}

void Logger::log(LogLevel p_level, const char* p_fmt, ...) {
    // Si es un log DEBUG pero el modo debug no está activo, salta el mensaje
    if (p_level == LOG_DEBUG && !debugMode) return;

    // Formatea el mensaje a imprimir usando los argumentos variables
    char buf[128];
    va_list args;
    va_start(args, p_fmt);
    vsnprintf(buf, sizeof(buf), p_fmt, args);
    va_end(args);

    // Obtener la hora actual sincronizada con NTP
    time_t now = time(nullptr);

    // Si `now` es inválido (por ejemplo, 0), mostramos solo `millis()`
    if (now < 100000) {
        Serial.print("[");
        Serial.print(levelToString(p_level));
        Serial.print(" @ ");
        Serial.print(millis());
        Serial.print("ms] ");
        Serial.println(buf);
        return;
    }

    // Convertir a estructura de tiempo legible
    struct tm tminfo;
    localtime_r(&now, &tminfo);
    char timestr[32];
    strftime(timestr, sizeof(timestr), "%Y-%m-%d-%H:%M:%S", &tminfo);
    uint16_t ms = millis() % 1000;  // Agregar milisegundos

    // Imprimir en Serial con formato completo
    Serial.print(timestr);
    Serial.print("-");
    if (ms < 100) Serial.print("0");
    if (ms < 10)  Serial.print("0");
    Serial.print(ms);
    Serial.print(" [");
    Serial.print(levelToString(p_level));
    Serial.print("] ");
    Serial.println(buf);
}
