#include "business/Watchdog.hpp"
#include "utils/logger.hpp"

// Intervalo para revisar las reglas
#define WATCHDOG_CHECK_INTERVAL_MS 5000

Watchdog& Watchdog::instance() {
    static Watchdog wd_instance; // Se crea una sola vez
    return wd_instance;
}

Watchdog::Watchdog() : manager_(MeasurementManager::instance()) {
}

void Watchdog::addRule(MeasurementType type, std::unique_ptr<IWatchdogRule> rule) {
    if (rule) {
        rule->configureManager(manager_);
        rules_[type] = std::move(rule);
        logger.log(LOG_INFO, "Regla para '%s' añadida al Watchdog.", measurementTypeToString(type));
    }
}

void Watchdog::update(uint32_t now) {
    if (now - tsLastCheck_ >= WATCHDOG_CHECK_INTERVAL_MS) {
        tsLastCheck_ = now;
        logger.log(LOG_DEBUG, "Watchdog: Ejecutando validaciones de reglas...");

        for (const auto& pair : rules_) {
            pair.second->validate(); // <-- ¡Llamada polimórfica!
        }
    }
}