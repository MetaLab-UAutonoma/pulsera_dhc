// input/Sensor.cpp
#include "input/Sensor.hpp"
#include "config.hpp"

/// =======================================================================
/// Category: Sensor base implementation
/// =======================================================================
const char* Sensor::stateToString(State p_state) {
    switch (p_state) {
        case State::APAGADO:    return "APAGADO";
        case State::WAITING:    return "WAITING";
        case State::LEYENDO:    return "LEYENDO";
        case State::ANALIZANDO: return "ANALIZANDO";
        default:         return "ESTADO_DESCONOCIDO";
    }
}
