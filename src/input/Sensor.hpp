// input/Sensor.hpp
#pragma once
#include "config.hpp"

/// =======================================================================
/// Category: Sensor base class
/// =======================================================================
class Sensor {
public:
    /** Estados comunes a todos los sensores */
    enum class State : uint8_t {
        APAGADO,
        WAITING,
        LEYENDO,
        ANALIZANDO
    };

    virtual ~Sensor() = default;

    /** Inicialización propia del sensor */
    virtual void init() = 0;
    /**
     * Un paso de la máquina no bloqueante.
     * @param p_now milis() actual
     */
    virtual void update(uint32_t p_now) = 0;

    /** Convierte el estado a cadena */
    static const char* stateToString(State p_state);
};
