#pragma once

#include <Arduino.h>
#include <stdint.h>
#include <string>

/**
 * Interfaz para enviar mensajes a través de diferentes medios (SMS, WhatsApp, HTTP, etc.)
 * Cumple con los principios SOLID:
 *  - SRP: responsabilidad única (solo define el contrato)
 *  - OCP: se pueden agregar nuevos senders sin modificar este archivo
 */
class IMessageSender {
public:
    virtual ~IMessageSender() {}

    /**
     * Enviar mensaje de alerta
     * @param text Texto completo del mensaje
     * @return true si se envió correctamente
     */
    virtual bool send(const String& text) = 0;
};