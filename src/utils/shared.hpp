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
};

/// Instancia global para acceder en cualquier parte con `shared.*`
extern Shared shared;
