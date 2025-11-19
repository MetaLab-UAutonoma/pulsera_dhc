#include "AlertMessageBuilder.hpp"

String AlertMessageBuilder::buildSMS(const MeasurementManager& mm) {
    float t = mm.getLast(MeasurementType::MEAS_TEMPERATURE);
    float bpm = mm.getLast(MeasurementType::MEAS_BPM);
    float spo2 = mm.getLast(MeasurementType::MEAS_SPO2);
    float bat = mm.getLast(MeasurementType::MEAS_BATTERY_PERCENT);
    float lat = mm.getLast(MeasurementType::MEAS_GPS_LAT);
    float lon = mm.getLast(MeasurementType::MEAS_GPS_LON);

    String msg = "TEMP: "; msg += t;
    msg += "\nBPM: "; msg += bpm;
    msg += "\nSPO2: "; msg += spo2;
    msg += "\nBAT: ";  msg += bat;
    msg += "\n\nGPS Lat: "; msg += lat; 
    msg += "\nGPS Lon: "; msg += lon;  
    return msg;
}