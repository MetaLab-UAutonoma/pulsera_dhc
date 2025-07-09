#include "business/MeasurementList.hpp"
#include <Arduino.h>  // para millis()

void MeasurementList::add(float val) {
    cleanOldData();
    uint32_t nowSec = millis() / 1000;
    data_.emplace_back(nowSec, val);  // más eficiente
    while (data_.size() > maxSize_) {
        data_.pop_front();
    }
}

void MeasurementList::cleanOldData() {
    uint32_t nowSec = millis() / 1000;
    data_.remove_if([this, nowSec](const TimedValue& tv) {
        return (nowSec - tv.timestamp) > maxAgeSeconds_;
    });
}
