// logger.cpp
#include "logger.hpp"
#include <Arduino.h>

extern bool debugMode;  // sigue siendo definido en main.cpp

/// Definición de la instancia singleton
Logger logger;

void Logger::init(const char* p_ntpServer, long p_gmtOffset_sec, int p_daylightOffset_sec) {
    // idéntico a tu initLogger
    configTime(p_gmtOffset_sec, p_daylightOffset_sec, p_ntpServer);
}

const char* Logger::levelToString(LogLevel p_level) {
    // misma lógica de tu función original
    switch (p_level) {
        case LOG_INFO:  return "INFO ";
        case LOG_DEBUG: return "DEBUG";
        case LOG_WARN:  return "WARN ";
        case LOG_ERROR: return "ERROR";
        default:        return "UNK  ";
    }
}

void Logger::log(LogLevel p_level, const char* p_fmt, ...) {
    // mismo filtro de DEBUG y debugMode
    if (p_level == LOG_DEBUG && !debugMode) return;

    // formateo de mensaje tal cual
    char buf[128];
    va_list args;
    va_start(args, p_fmt);
    vsnprintf(buf, sizeof(buf), p_fmt, args);
    va_end(args);

    // misma obtención de hora y timestamp
    time_t now = time(nullptr);
    struct tm tminfo;
    localtime_r(&now, &tminfo);
    char timestr[32];
    strftime(timestr, sizeof(timestr), "%Y-%m-%d-%H:%M:%S", &tminfo);
    uint16_t ms = millis() % 1000;

    // idénticas impresiones por Serial
    Serial.print(timestr);
    Serial.print("-");
    if (ms < 100) Serial.print("0");
    if (ms <  10) Serial.print("0");
    Serial.print(ms);
    Serial.print(" [");
    Serial.print(levelToString(p_level));
    Serial.print("] ");
    Serial.println(buf);
}
