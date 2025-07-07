#include <Arduino.h>
#include <Wire.h>

#include "utils/logger.hpp"
#include "utils/shared.hpp"

#include "business/Watchdog.hpp"

#include "input/PoxMax30100Sensor.hpp"
#include "input/TempSensor.hpp"

#include "output/Sim7600Modem.hpp"