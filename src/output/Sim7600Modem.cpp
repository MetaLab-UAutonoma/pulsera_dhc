#include "Sim7600Modem.hpp"

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
