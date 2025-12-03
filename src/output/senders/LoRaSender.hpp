#pragma once
#include "IMessageSender.hpp"
#include <Arduino.h>
#include <lmic.h> 


class LoRaSender : public IMessageSender {
public:
    LoRaSender();
    LoRaSender(lmic_client_data_t _cliente);

    /** Inicializa la pila LMIC y comienza el proceso de unión (OTAA) */
    void init();

    /** Se llama desde loop() para correr la máquina de estados de LMIC */
    void update(uint32_t now);

    /** * Lógica de envío. Ignora el String del mensaje, genera el payload binario 
     * a partir de MeasurementManager y lo envía por LoRaWAN.
     */
    virtual bool send(const String& msg) override;

    // Se usa un puntero a la clase en el stack de LMIC para acceder al estado
    bool isJoined_ = false;

private:
    /** Callback de eventos de LMIC */
    void onEvent(const ev_t& event);

    lmic_client_data_t cliente;

};