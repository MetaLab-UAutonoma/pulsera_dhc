// modem_sim7600.hpp
#pragma once
#include <Arduino.h>
#include "config.hpp"
#include "config/AppConfig.hpp"
#include "utils/logger.hpp"
#include "utils/shared.hpp"

/// =======================================================================
/// Category: Modem SIM7600 (POO)
/// =======================================================================
class Sim7600Modem {
public:

    enum class State : uint8_t {
        APAGADO,
        ESPERAR_RESP,
        ANALIZAR_RESP,
    };
    
    /** Constructor */
    Sim7600Modem(HardwareSerial& p_serial,
                 int p_rxPin,
                 int p_txPin,
                 const char* p_telefonoDestino,
                 const ModemConfig& config);

    /** Inicializa UART del módem (antes initModem) */
    void init();

    /**
     * Ejecuta la máquina de estados no bloqueante del módem
     * (antes cicloModem).
     * @param p_now tiempo actual en ms (millis()).
     */
    void update(uint32_t p_now);

    /** ¿El módem está activo? */
    bool isActive() const;

private:
    const ModemConfig& config_;
    HardwareSerial& serial_;
    int             rxPin_;
    int             txPin_;
    const char*     telefonoDestino_;
    bool            modemActivo_;
    uint32_t        tsModem_;
    uint32_t        tsModemCmd_;
    String          modemResp_;
    State           state_;
    const char* stateToString(State p_state);
};
