String AlertMessageBuilder::buildSMS(const MeasurementManager& mm) {
    float t = mm.getLast("temperature");
    float bpm = mm.getLast("bpm");
    float spo2 = mm.getLast("spo2");
    float bat = mm.getLast("battery_percent");

    String msg = "TEMP: "; msg += t;
    msg += "\nBPM: "; msg += bpm;
    msg += "\nSPO2: "; msg += spo2;
    msg += "\nBAT: ";  msg += bat;
    return msg;
}