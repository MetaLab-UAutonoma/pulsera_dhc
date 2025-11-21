#include "Sim7600Modem.hpp"
#include "utils/logger.hpp" // NECESARIO para logger.log()
#include <Arduino.h>      // NECESARIO para delay(), String, y Serial (UART)



Sim7600Modem::Sim7600Modem(HardwareSerial& p_serial,
                           int p_rxPin,
                           int p_txPin,
                           const char* p_telefonoDestino,
                           const ModemConfig& config)
  : serial_(p_serial)
  , rxPin_(p_rxPin)
  , txPin_(p_txPin)
  , telefonoDestino_(p_telefonoDestino)
  , config_(config)
  , modemActivo_(false)
  , tsModem_(0)
  , tsModemCmd_(0)
  , modemResp_()
  , state_(State::APAGADO)
{}

void Sim7600Modem::init() {

    tsModem_ = millis(); // Reinicia el contador de tiempo
    state_ = State::WAITING_BOOT; // <-- [2] TRANSICIÓN A ESPERA CRÍTICA
    logger.log(LOG_INFO, "SIM7600: Iniciado. Esperando arranque (30s)...");

    // Secuencia de encendido 
    pinMode(SIM_PWR_PIN, OUTPUT);
    digitalWrite(SIM_PWR_PIN, LOW);
    delay(100);
    digitalWrite(SIM_PWR_PIN, HIGH);
    delay(2500); // Presionar botón virtualmente por 2.5s
    digitalWrite(SIM_PWR_PIN, LOW);
    serial_.begin(115200, SERIAL_8N1, rxPin_, txPin_);
}

// CORRECCIÓN 1: Agregar corchetes {}
bool Sim7600Modem::isActive() const {
    return modemActivo_;
}

void Sim7600Modem::update(uint32_t p_now) {
    logger.log(LOG_DEBUG, "Modem, estado=%s", stateToString(state_));
    // ... (cuerpo de update, este estaba bien)
    switch (state_) {
        case State::APAGADO:
            if (p_now - tsModem_ >= SEG_A_MS(config_.timer_check_seg)) {
                logger.log(LOG_DEBUG, "Enviando AT");
                serial_.println("AT");
                tsModemCmd_ = p_now;
                modemResp_.clear();
                state_ = State::ESPERAR_RESP;
            }
            break;
        case State::WAITING_BOOT: // <-- [3] NUEVO ESTADO
            // Espera CRÍTICA de 30 segundos (o más, 30s es un buen mínimo)
            if (p_now - tsModem_ >= SEG_A_MS(30)) { 
                logger.log(LOG_INFO, "SIM7600: Fin de espera. Forzando chequeo AT.");
                // La transición a APAGADO con tsModem_ = 0 forzará el envío del primer AT en el siguiente ciclo.
                state_ = State::APAGADO;
                tsModem_ = 0; 
            }
            break;

        case State::ESPERAR_RESP:
            while (serial_.available()) {
                modemResp_ += char(serial_.read());
            }
            if (modemResp_.endsWith("\n") || p_now - tsModemCmd_ >= 2000) {
                logger.log(LOG_DEBUG, "Pasando a ANALIZAR_RESP");
                state_ = State::ANALIZAR_RESP;
            }
            break;

        case State::ANALIZAR_RESP:
            logger.log(LOG_DEBUG, "RESP=\"%s\"", modemResp_.c_str());
            modemActivo_ = (modemResp_.indexOf("OK") != -1);
            logger.log(modemActivo_ ? LOG_INFO : LOG_WARN,
                       "%s SIM7600", modemActivo_ ? "activo." : "no detectado.");
            tsModem_ = p_now;
            state_ = State::APAGADO;
            break;
    }
}

// CORRECCIÓN 2: Arreglar la definición de stateToString
const char* Sim7600Modem::stateToString(State p_state) {
    switch (p_state) {
        case State::APAGADO:      return "APAGADO";
        case State::WAITING_BOOT: return "WAITING_BOOT"; // <-- [3] NUEVO CASE
        case State::ESPERAR_RESP: return "ESPERAR_RESP";
        case State::ANALIZAR_RESP:return "ANALIZAR_RESP";
    }
    return "UNKNOWN";
}

// Nuevo método sendSMS
bool Sim7600Modem::sendSMS(const String& p_msg) {
    if (!modemActivo_) {
        logger.log(LOG_ERROR, "SMS: Módem no activo o fuera de ciclo de chequeo.");
        return false;
    }

    // 1. Configurar modo texto
    serial_.println("AT+CMGF=1");
    delay(500); 

    // 2. Comando de envío con número destino
    String cmd = "AT+CMGS=\"" + String(telefonoDestino_) + "\"";
    serial_.println(cmd);
    delay(500); // Esperar el prompt '>'

    // 3. Enviar cuerpo del mensaje
    serial_.print(p_msg);

    // 4. Terminar mensaje con Ctrl-Z (ASCII 26)
    serial_.write(26);

    delay(7000); 
    logger.log(LOG_INFO, "SMS: Comando de envío ejecutado para %s.", telefonoDestino_);

    return true; 
}