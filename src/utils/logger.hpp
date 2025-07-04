#pragma once
#include <Arduino.h>
#include <stdarg.h>
#include <time.h>

enum LogLevel {
  LOG_INFO,
  LOG_DEBUG,
  LOG_WARN,
  LOG_ERROR
};

/// Inicializa el reloj NTP. Llamar una vez en setup().
void initLogger(const char* ntpServer, long gmtOffset_sec, int daylightOffset_sec);

/// Traduce un nivel de log a cadena.
const char* logLevelToString(LogLevel lvl);

/// Imprime un mensaje formateado con timestamp YYYY-MM-DD-HH:MM:SS-mmm y nivel.
/// LOG_DEBUG solo se emite si debugMode==true (extern bool).
void logMessage(LogLevel level, const char* fmt, ...);
