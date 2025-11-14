#include "AlertMessageBuilder.hpp"

String AlertMessageBuilder::buildSMS(const MeasurementManager& mm) {
    float t = mm.getLast(MeasurementType::MEAS_TEMPERATURE);
    float bpm = mm.getLast(MeasurementType::MEAS_BPM);
    float spo2 = mm.getLast(MeasurementType::MEAS_SPO2);
    float bat = mm.getLast(MeasurementType::MEAS_BATTERY_PERCENT);

    String msg = "TEMP: "; msg += t;
    msg += "\nBPM: "; msg += bpm;
    msg += "\nSPO2: "; msg += spo2;
    msg += "\nBAT: ";  msg += bat;
    return msg;
}