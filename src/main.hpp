#include <Arduino.h>
#include <Wire.h>
#include <memory>
#include <utility>



#include "utils/logger.hpp"
#include "utils/shared.hpp"

#include "business/Watchdog.hpp"
#include "business/rules/IWatchdogRule.hpp"
#include "business/rules/WatchdogRuleBPM.hpp"
#include "business/rules/WatchdogRuleSpO2.hpp"
#include "business/rules/WatchdogRuleTemp.hpp"

#include "input/PoxMax30100Sensor.hpp"
#include "input/TempSensor.hpp"

#include "output/Sim7600Modem.hpp"
namespace std {
    template<typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args&&... args) {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }
}