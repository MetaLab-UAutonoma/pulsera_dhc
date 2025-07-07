// business/rules/IWatchdogRule.hpp
#pragma once

#include "business/MeasurementManager.hpp"

/**
 * Interfaz (clase base abstracta) para todas las reglas de negocio del Watchdog.
 * Define el contrato que cada regla específica (temperatura, BPM, etc.) debe cumplir.
 */
class IWatchdogRule {
public:
    virtual ~IWatchdogRule() = default;

    /**
     * Valida el historial de mediciones contra la lógica específica de esta regla.
     * Este es el método que será llamado polimórficamente por el Watchdog principal.
     */
    virtual void validate() = 0;

    /**
     * Configura el historial de mediciones necesario para esta regla.
     * El Watchdog llamará a este método para que la regla le diga al
     * MeasurementManager cómo debe configurarse.
     */
    virtual void configureManager(MeasurementManager& manager) = 0;
};