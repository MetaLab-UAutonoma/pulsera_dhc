/*#pragma once
#include <Arduino.h>
#include <TinyGPSPlus.h>

/// Sensor GPS Ublox NEO-8M v2 (no bloqueante)
class GpsUblox {
public:
    /// @param serial   UART usado (ej: Serial1 ó Serial2)
    /// @param rxPin    Pin RX de la ESP32 (entrada desde TX del GPS)
    /// @param txPin    Pin TX de la ESP32 (salida hacia RX del GPS, opcional)
    /// @param baud     Baud rate, típico 9600
    GpsUblox(HardwareSerial& serial, int rxPin, int txPin, uint32_t baud = 9600);

    /// Inicializa la UART
    void init();

    /// Actualiza el estado del GPS (no bloqueante)
    void update(uint32_t now);

    /// ¿Tiene fix válido reciente?
    bool hasFix() const { return hasFix_; }

    double latitude()  const { return lat_; }
    double longitude() const { return lon_; }
    double altitude()  const { return alt_; }
    double speedKmph() const { return speed_; }
    uint8_t satellites() const { return sats_; }

private:
    HardwareSerial& serial_;
    int rxPin_;
    int txPin_;
    uint32_t baud_;

    TinyGPSPlus gps_;

    uint32_t lastReadTs_;
    static constexpr uint32_t readIntervalMs_ = 200; // cada 200 ms analiza datos

    bool   hasFix_;
    double lat_;
    double lon_;
    double alt_;
    double speed_;
    uint8_t sats_;

    void processGpsData();
};
*/