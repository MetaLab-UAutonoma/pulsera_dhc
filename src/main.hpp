#include <Arduino.h>
#include <Wire.h>
#include <memory>
#include <utility>

#include "config/ConfigManager.hpp"
#include "config/AppConfig.hpp"

#include "utils/logger.hpp"
#include "utils/shared.hpp"

#include "business/Watchdog.hpp"
#include "business/rules/IWatchdogRule.hpp"
#include "business/rules/thresholdWatchdogRule.hpp" 
#include "business/rules/WatchdogRuleBPM.hpp"
#include "business/rules/WatchdogRuleSpO2.hpp"
#include "business/rules/WatchdogRuleTemp.hpp"
#include "business/rules/WatchdogRuleBattery.hpp" 
#include "business/rules/WatchdogRuleGps.hpp" 

#include "input/PoxMax30100Sensor.hpp"
#include "input/TempSensor.hpp"
#include "input/BatterySensor.hpp" 
// --- NUEVAS LIBRERÍAS DHT ---
#include <OneWire.h>
#include <DallasTemperature.h>


#include "output/Sim7600Modem.hpp"
#include "output/senders/SmsSender.hpp" 

// --- NUEVAS LIBRERÍAS LORAWAN ---

#include "output/senders/LoRaSender.hpp"
#include "output/senders/SmsSender.hpp"
#include <SPI.h>
#include <config.h>
#include "config.hpp" // Para las definiciones LORA_*




/// --- CLAVES LORAWAN (¡REEMPLAZAR!) ---
// Obtener del Servidor de Red (TTS). Deben estar en formato little-endian (bytes invertidos)
// DevEUI (8 bytes)
static const uint8_t LORA_DEVEUI[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x01 };
// AppEUI / JoinEUI (8 bytes)
static const uint8_t LORA_APPEUI[8] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x02 };
// AppKey (16 bytes)
static const uint8_t LORA_APPKEY[16] = { 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x03 };


namespace std {
    template<typename T, typename... Args>
    std::unique_ptr<T> make_unique(Args&&... args) {
        return std::unique_ptr<T>(new T(std::forward<Args>(args)...));
    }
}