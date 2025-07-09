#include "TimeManager.hpp"
#include <WiFi.h>
#include <esp_sntp.h>
#include <Arduino.h>

TimeManager& TimeManager::instance() {
    static TimeManager instance;
    return instance;
}

void TimeManager::begin() {
    // Configurar la zona horaria y servidores NTP
    configTime(-14400, 0, "cl.pool.ntp.org", "time.nist.gov"); // GMT-4, sin DST
    Serial.println("Sincronizando hora con NTP...");

    time_t now = time(nullptr);
    int retry = 0;
    while (now < 8 * 3600 * 2 && retry < 10) { // Espera que la hora esté por lo menos después de 1970-01-01
        delay(2000);
        Serial.print(".");
        now = time(nullptr);
        retry++;
    }
    if (retry == 10) {
        Serial.println("\nNo se pudo sincronizar hora NTP");
    } else {
        Serial.println("\nHora sincronizada correctamente");
    }
}

time_t TimeManager::getCurrentTime() const {
    return time(nullptr);
}

bool TimeManager::isTimeSet() const {
    return (time(nullptr) > 8 * 3600 * 2);
}
