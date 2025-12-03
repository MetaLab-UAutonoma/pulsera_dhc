#include "LoRaSender.hpp"
#include "utils/logger.hpp"
#include "business/MeasurementManager.hpp"
#include "output/AlertMessageBuilder.hpp"
#include "config.hpp"
#include "main.hpp" // Contiene las claves LORA_DEVEUI y el bloque extern "C"
#include <SPI.h>
#include <hal/hal.h>
// Esto resuelve el error "not declared in this scope"
extern "C" {
    #include <lmic.h> 
    // No incluir hal.h aquí, ya que main.hpp lo hace
}

// =======================================================
// Declaraciones de Eventos
// =======================================================
// La función onEvent debe ser global
void onEvent (ev_t ev) { // Usamos el alias de tipo 'ev_t' para compatibilidad C++
    // CORRECCIÓN 1: Usar 'client' en lugar de 'clientData' (resuelve error de struct)
    LoRaSender* sender = new LoRaSender();

    switch(ev) {
        case EV_JOINING:
            logger.log(LOG_INFO, "LoRaWAN: Uniéndose a la red (OTAA)...");
            break;
        case EV_JOINED:
            logger.log(LOG_INFO, "LoRaWAN: ¡UNIDO CON ÉXITO! (EV_JOINED)");
            sender->isJoined_ = true;
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
}
LoRaSender::LoRaSender(lmic_client_data_t _cliente) {
    // CORRECCIÓN 2: Usar 'client' para guardar el puntero de la instancia
    _cliente = _cliente;
}


void LoRaSender::init() 
{
    // 1. Definir la estructura de pines localmente (para evitar redefinición global)
    static const lmic_pinmap lmic_pins = {
        .nss = LORA_CS,
        .rxtx = LMIC_UNUSED_PIN,
        .rst = LORA_RST,
        .dio = {LORA_DIO0, LMIC_UNUSED_PIN, LMIC_UNUSED_PIN},
    };
    
    // 2. Inicializar SPI y LMIC
    SPI.begin(LORA_SCK, LORA_MISO, LORA_MOSI, LORA_CS);

    os_init();
    /*
    // LMIC_setPinMap AHORA ES VISIBLE gracias al bloque extern "C"
    LMIC_setPinMap(&lmic_pins);
    
    
    // 3. Establecer las claves de unión OTAA
    // LMIC_setSessionKeys AHORA ES VISIBLE
    LMIC_setSessionKeys(
        0, 
        0, 
        (xref2u1_t)LORA_APPEUI, // Aplicar cast
        (xref2u1_t)LORA_APPKEY  // Aplicar cast
    );
*/
    // 4. Iniciar el proceso
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
    uint8_t payload[32]; // Buffer de hasta 32 bytes
    // Esta función llama a MeasurementManager para obtener el GPS y los signos vitales
    size_t payload_len = AlertMessageBuilder::buildLoRaPayload(
        MeasurementManager::instance(), 
        payload
    );

    // 3. Enviar (Unconfirmed, FPort 1)
    if (LMIC_setTxData2(1, payload, payload_len, 0) == 0) {
        logger.log(LOG_INFO, "LoRaWAN: Payload binario enviado (%u bytes).", payload_len);
        return true;
    } else {
        logger.log(LOG_ERROR, "LoRaWAN: Error al configurar el envío (error de librería).");
        return false;
    }
}