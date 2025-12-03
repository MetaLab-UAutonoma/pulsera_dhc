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
// =======================================================
// 3. IMPLEMENTACIÓN DE buildLoRaPayload (Formato Binario)
// =======================================================
size_t AlertMessageBuilder::buildLoRaPayload(const MeasurementManager& mm, uint8_t* payloadBuffer) {
    // 1. Recuperar los últimos valores (incluyendo GPS)
    float t = mm.getLast(MeasurementType::MEAS_TEMPERATURE);
    float bpm = mm.getLast(MeasurementType::MEAS_BPM);
    float spo2 = mm.getLast(MeasurementType::MEAS_SPO2);
    float bat_pct = mm.getLast(MeasurementType::MEAS_BATTERY_PERCENT);
    float lat = mm.getLast(MeasurementType::MEAS_GPS_LAT);
    float lon = mm.getLast(MeasurementType::MEAS_GPS_LON);

    // 2. Codificación Binaria (Total: 16 bytes)
    size_t offset = 0;

    // a) Temperatura (x10 para 1 decimal de precisión) - 2 bytes
    uint16_t temp_val = (uint16_t)(t * 10.0f);
    memcpy(&payloadBuffer[offset], &temp_val, sizeof(uint16_t));
    offset += sizeof(uint16_t);

    // b) BPM - 2 bytes
    uint16_t bpm_val = (uint16_t)bpm;
    memcpy(&payloadBuffer[offset], &bpm_val, sizeof(uint16_t));
    offset += sizeof(uint16_t);

    // c) SpO2 - 2 bytes
    uint16_t spo2_val = (uint16_t)spo2;
    memcpy(&payloadBuffer[offset], &spo2_val, sizeof(uint16_t));
    offset += sizeof(uint16_t);

    // d) Batería % - 2 bytes
    uint16_t bat_pct_val = (uint16_t)bat_pct;
    memcpy(&payloadBuffer[offset], &bat_pct_val, sizeof(uint16_t));
    offset += sizeof(uint16_t);

    // e) Latitud (x1000000 para 6 decimales de precisión) - 4 bytes
    int32_t lat_val = (int32_t)(lat * 1000000.0f);
    memcpy(&payloadBuffer[offset], &lat_val, sizeof(int32_t));
    offset += sizeof(int32_t);

    // f) Longitud (x1000000 para 6 decimales de precisión) - 4 bytes
    int32_t lon_val = (int32_t)(lon * 1000000.0f);
    memcpy(&payloadBuffer[offset], &lon_val, sizeof(int32_t));
    offset += sizeof(int32_t);

    return offset; // Retorna 16 bytes

 }

