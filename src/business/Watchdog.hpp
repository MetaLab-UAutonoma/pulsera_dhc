// Watchdog.hpp
#pragma once

#include <map>
#include <memory>
#include "business/MeasurementManager.hpp"
#include "output/Sim7600Modem.hpp" // Para poder enviar alertas
#include "business/rules/IWatchdogRule.hpp"

/// Define las reglas de negocio y genera alertas basadas en el historial.
class Watchdog {
public:
    static Watchdog& instance();

    void addRule(MeasurementType type, std::unique_ptr<IWatchdogRule> rule);
    void update(uint32_t now);

private:

    Watchdog();
    ~Watchdog() = default;
    Watchdog(const Watchdog&) = delete;
    Watchdog& operator=(const Watchdog&) = delete;

    void validate(MeasurementType type, IWatchdogRule& rule);

    MeasurementManager& manager_;
    std::map<MeasurementType, std::unique_ptr<IWatchdogRule>> rules_;
    
    uint32_t tsLastCheck_ = 0;
};