#include "GpsUblox.hpp"
#include "business/MeasurementManager.hpp" 

GpsUblox::GpsUblox(HardwareSerial& serial, int rxPin, int txPin, uint32_t baud)
    : serial_(serial)
    , rxPin_(rxPin)
    , txPin_(txPin)
    , baud_(baud)
    , lastReadTs_(0)
    , hasFix_(false)
    , lat_(0.0)
    , lon_(0.0)
    , alt_(0.0)
    , speed_(0.0)
    , sats_(0)
{
}

void GpsUblox::init() {
    serial_.begin(baud_, SERIAL_8N1, rxPin_, txPin_);
}

void GpsUblox::update(uint32_t now) {
    if (now - lastReadTs_ < readIntervalMs_) {
        return;
    }
    lastReadTs_ = now;

    // Leer todos los bytes disponibles del módulo GPS
    while (serial_.available()) {
        char c = static_cast<char>(serial_.read());
        gps_.encode(c);
    }

    processGpsData();
}

void GpsUblox::processGpsData() {
    MeasurementManager& mm = MeasurementManager::instance();
    // Ubicación
    if (gps_.location.isValid() && gps_.location.age() < 5000) {
        hasFix_ = true;
        lat_ = gps_.location.lat();
        lon_ = gps_.location.lng();
        mm.addMeasurement(MEAS_GPS_LAT, lat_);
        mm.addMeasurement(MEAS_GPS_LON, lon_);
    } else {
        hasFix_ = false;
        lat_ = 0.0;
        lon_ = 0.0;
    }

    // Altura
    if (gps_.altitude.isValid()) {
        alt_ = gps_.altitude.meters();
    }

    // Satélites
    if (gps_.satellites.isValid()) {
        sats_ = gps_.satellites.value();
        mm.addMeasurement(MEAS_GPS_SATS, (float)sats_);
    }
    

    // Velocidad
    if (gps_.speed.isValid()) {
        speed_ = gps_.speed.kmph();
        mm.addMeasurement(MEAS_GPS_SPEED, speed_);
    }
}
