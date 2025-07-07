// shared.hpp
#pragma once
#include <Arduino.h>
#include "config.hpp"
#include "utils/logger.hpp"

/// =======================================================================
/// Category: Shared singleton
/// =======================================================================
class Shared {
public:
    static String tsToString(time_t ts);
};

/// Instancia global para acceder en cualquier parte con `shared.*`
extern Shared shared;
