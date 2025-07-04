// logger.hpp
#pragma once
#include <Arduino.h>
#include <stdarg.h>
#include <time.h>

/// Niveles de log
enum LogLevel {
  LOG_INFO,
  LOG_DEBUG,
  LOG_WARN,
  LOG_ERROR
};

/// Logger orientado a objetos (singleton)
class Logger {
public:
    /** Inicializa el reloj NTP; llama a configTime igual que antes */
    void init(const char* p_ntpServer, long p_gmtOffset_sec, int p_daylightOffset_sec);

    /** Mismo logMessage de siempre, ahora como método */
    void log(LogLevel p_level, const char* p_fmt, ...);

private:
    /** Traduce un nivel a cadena (antes logLevelToString) */
    const char* levelToString(LogLevel p_level);
};

/// Instancia global para que sigas usando logger.log(...)
extern Logger logger;
