#include "AlertMessageBuilder.hpp"
#include <time.h> // Necesario para time(nullptr)

// =======================================================
// 1. IMPLEMENTACIÓN DE buildSMS (Mensaje de texto simple)
// =======================================================
String AlertMessageBuilder::buildSMS(const MeasurementManager& mm) {
    // Recuperar los últimos valores para incluirlos en el SMS
    float t = mm.getLast(MeasurementType::MEAS_TEMPERATURE);
    float bpm = mm.getLast(MeasurementType::MEAS_BPM);
    float spo2 = mm.getLast(MeasurementType::MEAS_SPO2);
    float bat_pct = mm.getLast(MeasurementType::MEAS_BATTERY_PERCENT);
    float bat_v = mm.getLast(MeasurementType::MEAS_BATTERY_VOLTAGE);

    float lat = mm.getLast(MeasurementType::MEAS_GPS_LAT);
    float lon = mm.getLast(MeasurementType::MEAS_GPS_LON);

    // Formato de texto simple
    String msg = "ALARMA DHC:";
    msg += "\nTemp: " + String(t, 2) + "C";
    msg += "\nBPM: " + String(bpm, 0);
    msg += "\nSpO2: " + String(spo2, 0) + "%";
    msg += "\nBateria: " + String(bat_pct, 0) + "% (" + String(bat_v, 2) + "V)";
    msg += "\n\nGPS Lat: " + String(lat, 6); 
    msg += "\nGPS Lon: " + String(lon, 6);  
    
    return msg;
}


// =======================================================
// 2. IMPLEMENTACIÓN DE buildJSON (Formato para HTTP POST)
// =======================================================
String AlertMessageBuilder::buildJSON(const MeasurementManager& mm) {
    // Recuperar los últimos valores (Mismos que para SMS)
    float t = mm.getLast(MeasurementType::MEAS_TEMPERATURE);
    float bpm = mm.getLast(MeasurementType::MEAS_BPM);
    float spo2 = mm.getLast(MeasurementType::MEAS_SPO2);
    float bat_pct = mm.getLast(MeasurementType::MEAS_BATTERY_PERCENT);
    float bat_v = mm.getLast(MeasurementType::MEAS_BATTERY_VOLTAGE);
    float lat = mm.getLast(MeasurementType::MEAS_GPS_LAT);
    float lon = mm.getLast(MeasurementType::MEAS_GPS_LON);

    // Formatear la cadena JSON.
    String json = "{";
    json += "\"timestamp\": " + String(time(nullptr)) + ",";
    json += "\"temp\": " + String(t, 2) + ",";
    json += "\"bpm\": " + String(bpm, 0) + ",";
    json += "\"spo2\": " + String(spo2, 0) + ",";
    json += "\"battery_percent\": " + String(bat_pct, 0) + ",";
    json += "\"battery_voltage\": " + String(bat_v, 2) + ",";
    json += "\"latitude\": " + String(lat, 6) + ",";
    json += "\"longitude\": " + String(lon, 6);
    json += "}";
    
    return json;
}