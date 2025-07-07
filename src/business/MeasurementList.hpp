// MeasurementList.hpp
#pragma once
#include <list>
#include <Arduino.h>

struct TimedValue {
    time_t timestamp;
    float  value;
};

class MeasurementList {
public:
    MeasurementList(size_t max_size, uint32_t max_age_seconds)
      : maxSize_(max_size), maxAgeSeconds_(max_age_seconds) {}


    void add(float val);
    const std::list<TimedValue>& getData() const {
        return data_;
    }

private:
    void cleanOldData();

    std::list<TimedValue> data_;
    size_t   maxSize_;
    uint32_t maxAgeSeconds_;
};