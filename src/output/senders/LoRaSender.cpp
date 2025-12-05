#include "LoRaSender.hpp"
#include "utils/logger.hpp"
#include "business/MeasurementManager.hpp"
#include "output/AlertMessageBuilder.hpp"
#include "config.hpp"
#include "main.hpp" // Contiene LORA_DEVEUI y extern "C"
#include <SPI.h>
#include <hal/hal.h>

// Esto resuelve problemas de visibilidad con LMIC
extern "C" {
    #include <lmic.h> 
}

// Puntero global a la instancia activa (necesario para onEvent)
static LoRaSender* g_lora_sender_instance = nullptr;

// =======================================================
// Declaraciones de Eventos (Global)
// =======================================================
void onEvent (ev_t ev) {
    LoRaSender* sender = g_lora_sender_instance; 
    
    switch(ev) {
        case EV_JOINING:
            logger.log(LOG_INFO, "LoRaWAN: Uniéndose a la red (OTAA)...");
            break;
        case EV_JOINED:
            logger.log(LOG_INFO, "LoRaWAN: ¡UNIDO CON ÉXITO! (EV_JOINED)");
            if(sender) sender->isJoined_ = true;
            break;
        case EV_TXCOMPLETE:
            logger.log(LOG_INFO, "LoRaWAN: Envío completado.");
            break;
        default:
            logger.log(LOG_DEBUG, "LoRaWAN: Evento desconocido: %d", ev);
            break;
    }
}

// =======================================================
// Implementación de LoRaSender
// =======================================================

LoRaSender::LoRaSender() {
    g_lora_sender_instance = this;
}

LoRaSender::LoRaSender(lmic_client_data_t _cliente) {
    // Constructor alternativo si lo usas
    (void)_cliente; // Evitar warning de variable no usada
    g_lora_sender_instance = this;
}

void LoRaSender::init() 
{
    // 1. Inicializar SPI
    // IMPORTANTE: Esto debe estar DENTRO de la función init()
    SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);

    // 2. Inicializar el sistema operativo de LMIC
    os_init();

    // 3. Resetear el estado MAC y descartar datos pendientes
    LMIC_reset();

    // NOTA: No definimos 'lmic_pins' ni 'LMIC_setSessionKeys' aquí
    // porque 'lmic_pins' está en main.cpp y usamos OTAA.
    
    // 4. Iniciar el proceso de unión
    LMIC_startJoining();
    isJoined_ = false;

    logger.log(LOG_INFO, "LoRaWAN: Inicializado. Iniciando proceso de Union...");
}

void LoRaSender::update(uint32_t now) {
    // Ejecutar el ciclo de eventos de la pila LoRaWAN.
    os_runloop_once(); 
}

bool LoRaSender::send(const String& msg) {
    if (!isJoined_) {
        logger.log(LOG_WARN, "LoRaWAN: Falló envío - No unido a la red. Reintentando unir...");
        LMIC_startJoining();
        return false;
    }

    // 1. Verificar si la cola de transmisión está ocupada
    if (LMIC.opmode & OP_TXRXPEND) {
        logger.log(LOG_WARN, "LoRaWAN: Falló envío - Ya hay un mensaje pendiente en cola.");
        return false;
    }

    // 2. Construir el Payload Binario
    uint8_t payload[32]; 
    size_t payload_len = AlertMessageBuilder::buildLoRaPayload(
        MeasurementManager::instance(), 
        payload
    );

    // 3. Enviar (Unconfirmed, FPort 1)
    // LMIC_setTxData2 prepara el paquete para el siguiente ciclo
    if (LMIC_setTxData2(1, payload, payload_len, 0) == 0) {
        logger.log(LOG_INFO, "LoRaWAN: Payload binario enviado (%u bytes).", payload_len);
        return true;
    } else {
        logger.log(LOG_ERROR, "LoRaWAN: Error al configurar el envío (error de librería).");
        return false;
    }
}