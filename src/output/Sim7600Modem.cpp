#include "Sim7600Modem.hpp"

#define SEG_A_MS(seg) ((uint32_t)((seg) * 1000))


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
    // idéntico a initModem(...)
    serial_.begin(115200, SERIAL_8N1, rxPin_, txPin_);
}

bool Sim7600Modem::isActive() const {
    return modemActivo_;
}

void Sim7600Modem::update(uint32_t p_now) {
    logger.log(LOG_DEBUG, "Modem, estado=%s", stateToString(state_));
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
    
const char* Sim7600Modem::stateToString(State p_state) {
    switch (p_state) {
        case State::APAGADO:      return "APAGADO";
        case State::ESPERAR_RESP: return "ESPERAR_RESP";
        case State::ANALIZAR_RESP:return "ANALIZAR_RESP";
    }
    return "UNKNOWN";
}
void Sim7600Modem::enviarAlerta(const String& mensaje) {
    if (!modemActivo_) {
        logger.log(LOG_WARN, "No se puede enviar alerta: módem inactivo.");
        return;
    }

    logger.log(LOG_INFO, "Enviando alerta: %s", mensaje.c_str());

    // Limpiar buffer antes de enviar comando
    while (serial_.available()) serial_.read();

//prueba

// === Iniciar chequeos de estado ===
logger.log(LOG_DEBUG, "Chequeando estado de la SIM con AT+CPIN?");
serial_.println("AT+CPIN?");
delay(500);
while (serial_.available()) {
    String resp = serial_.readStringUntil('\n');
    logger.log(LOG_DEBUG, "RESP (CPIN): %s", resp.c_str());
}

logger.log(LOG_DEBUG, "Chequeando intensidad de señal con AT+CSQ");
serial_.println("AT+CSQ");
delay(500);
while (serial_.available()) {
    String resp = serial_.readStringUntil('\n');
    logger.log(LOG_DEBUG, "RESP (CSQ): %s", resp.c_str());
}

logger.log(LOG_DEBUG, "Chequeando registro en red con AT+CREG?");
serial_.println("AT+CREG?");
delay(500);
while (serial_.available()) {
    String resp = serial_.readStringUntil('\n');
    logger.log(LOG_DEBUG, "RESP (CREG): %s", resp.c_str());
}

logger.log(LOG_DEBUG, "Configurando charset con AT+CSCS=\"GSM\"");
serial_.println("AT+CSCS=\"GSM\"");
delay(500);
while (serial_.available()) {
    String resp = serial_.readStringUntil('\n');
    logger.log(LOG_DEBUG, "RESP (CSCS): %s", resp.c_str());
}

logger.log(LOG_DEBUG, "Poniendo en modo texto con AT+CMGF=1");
serial_.println("AT+CMGF=1");
delay(500);
while (serial_.available()) {
    String resp = serial_.readStringUntil('\n');
    logger.log(LOG_DEBUG, "RESP (CMGF): %s", resp.c_str());
}



    // Poner módem en modo texto
    serial_.println("AT+CMGF=1");
    delay(1000);
    while (serial_.available()) {
        String resp = serial_.readStringUntil('\n');
        logger.log(LOG_DEBUG, "RESP (CMGF): %s", resp.c_str());
    }

    // Limpiar buffer antes de nuevo comando
    while (serial_.available()) serial_.read();

    // Comando para enviar SMS
    serial_.print("AT+CMGS=\"");
    serial_.print(telefonoDestino_);
    serial_.println("\"");

    // Esperar prompt '>'
    uint32_t start = millis();
    bool promptReceived = false;
    while (millis() - start < 5000) {
        if (serial_.available()) {
            String resp = serial_.readStringUntil('\n');
            logger.log(LOG_DEBUG, "RESP (CMGS): %s", resp.c_str());
            if (resp.indexOf('>') != -1) {
                promptReceived = true;
                break;
            }
        }
        delay(10);
    }

    if (!promptReceived) {
        logger.log(LOG_WARN, "No se recibió prompt '>' para enviar mensaje");
        return;
    }

    // Enviar mensaje
    serial_.print(mensaje);

    // Ctrl+Z para terminar
    serial_.write(26);

    // Esperar respuesta final
    start = millis();
    while (millis() - start < 10000) {
        if (serial_.available()) {
            String resp = serial_.readStringUntil('\n');
            logger.log(LOG_INFO, "RESP: %s", resp.c_str());

            if (resp.indexOf("OK") != -1) {
                logger.log(LOG_INFO, "Mensaje enviado correctamente");
                serial_.println("mensaje enviado");
                break;
            }
            if (resp.indexOf("ERROR") != -1) {
                logger.log(LOG_WARN, "Error al enviar mensaje: %s", resp.c_str());
                break;
            }
        }
        delay(10);
    }
}
