#include "logger.hpp"
#include <Arduino.h>

extern bool debugMode;  // definido en main.cpp

void initLogger(const char* ntpServer, long gmtOffset_sec, int daylightOffset_sec) {
  configTime(gmtOffset_sec, daylightOffset_sec, ntpServer);
}

const char* logLevelToString(LogLevel lvl) {
  switch (lvl) {
    case LOG_INFO:  return "INFO ";
    case LOG_DEBUG: return "DEBUG";
    case LOG_WARN:  return "WARN ";
    case LOG_ERROR: return "ERROR";
    default:        return "UNK  ";
  }
}

void logMessage(LogLevel level, const char* fmt, ...) {
  if (level == LOG_DEBUG && !debugMode) return;

  // Formateo del mensaje
  char buf[128];
  va_list args;
  va_start(args, fmt);
  vsnprintf(buf, sizeof(buf), fmt, args);
  va_end(args);

  // Hora actual
  time_t now = time(nullptr);
  struct tm tminfo;
  localtime_r(&now, &tminfo);
  char timestr[32];
  strftime(timestr, sizeof(timestr), "%Y-%m-%d-%H:%M:%S", &tminfo);
  uint16_t ms = millis() % 1000;

  // Imprimir
  Serial.print(timestr);
  Serial.print("-");
  if (ms < 100) Serial.print("0");
  if (ms <  10) Serial.print("0");
  Serial.print(ms);
  Serial.print(" [");
  Serial.print(logLevelToString(level));
  Serial.print("] ");
  Serial.println(buf);
}
