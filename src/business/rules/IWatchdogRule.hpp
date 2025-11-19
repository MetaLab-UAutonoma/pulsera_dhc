#pragma once

#include "business/MeasurementManager.hpp"

class IWatchdogRule {
public:
    virtual ~IWatchdogRule() = default;

    virtual void validate() = 0;

    virtual void configureManager(MeasurementManager& manager) = 0;
};