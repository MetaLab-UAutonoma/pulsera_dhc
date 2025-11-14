#include "business/MeasurementList.hpp"


void MeasurementList::add(float val) {
    cleanOldData();
    data_.push_back({time(nullptr), val});
    while (data_.size() > maxSize_) {
        data_.pop_front();
    }
}

void MeasurementList::cleanOldData() {
    time_t now = time(nullptr);
    data_.remove_if([this, now](const TimedValue& tv) {
        return (now - tv.timestamp) > maxAgeSeconds_;
    });
}

TimedValue MeasurementList::last(){
    return data_.back();
}