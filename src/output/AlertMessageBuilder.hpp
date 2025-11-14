#pragma once

#include <Arduino.h>
#include "business/MeasurementManager.hpp"

/**
 * AlertMessageBuilder
 *
 * Su responsabilidad única (SRP):
 *  - Construir mensajes de alerta a partir del estado actual del sistema
 *  - No envía mensajes
 *  - No conoce modem, ni HTTP, ni SMS
 *
 * Mantiene el principio DRY:
 *  - Toda la lógica de armado de mensajes está centralizada aquí.
 */
class AlertMessageBuilder {
public:

    /**
     * Construye un mensaje SMS simple, apto para enviar por el SIM7600.
     * Incluye:
     *  - Temperatura
     *  - BPM
     *  - SpO2
     *  - Batería (% y voltaje)
     */
    static String buildSMS(const MeasurementManager& mm);

    /**
     * Construye un mensaje en formato JSON.
     * Este mensaje sirve para HTTP POST u otras integraciones.
     */
    static String buildJSON(const MeasurementManager& mm);
};