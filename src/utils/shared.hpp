// shared.hpp
#pragma once
#include <Arduino.h>
#include "utils/logger.hpp"
#define SEG_A_MS(x) ((uint32_t)((x) * 1000))

/// =======================================================================
/// Category: Shared singleton
/// =======================================================================
class Shared {
public:
    static String tsToString(time_t ts);
};

/// Instancia global para acceder en cualquier parte con `shared.*`
extern Shared shared;
