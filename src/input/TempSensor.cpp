#include "input/TempSensor.hpp"
#include "business/MeasurementManager.hpp"

// Constructor: Inicializamos OneWire y DallasTemperature
TempSensor::TempSensor(int p_pinSensor, const TempSensorConfig& config)
  : pinSensor_(p_pinSensor)
  , config_(config)
  , active_(false)
  , tsTemp_(0)
  , tsWaitingTemp_(0)
  , tsLeerTemp_(0)
  , state_(State::APAGADO)
  , oneWire_(p_pinSensor) // Vinculamos OneWire al pin
  , sensors_(&oneWire_)   // Pasamos la referencia a DallasTemp
{}

void TempSensor::init() {
    sensors_.begin(); // Iniciar el bus del sensor
    active_ = true;
    logger.log(LOG_INFO, "Sensor DS18B20 inicializado en Pin %d", pinSensor_);
}

void TempSensor::update(uint32_t now) {
    // logger.log(LOG_DEBUG, "Temp, estado=%s", Sensor::stateToString(state_));

    switch (state_) {
        case State::APAGADO:
            if (now - tsTemp_ >= SEG_A_MS(config_.ciclo_monitoreo_seg)) {
                active_       = true;
                tsWaitingTemp_= now;
                tsTemp_       = now;
                lastReadTemp_ = 0; 
                tsRetry_      = 0;
                state_        = State::WAITING;
            }
            break;

        case State::WAITING:
            if (now - tsWaitingTemp_ >= SEG_A_MS(config_.timer_estabilizacion_seg)) {
                tsLeerTemp_ = now;
                state_      = State::LEYENDO;
            }
            break;
        
        case State::LEYENDO:
            // Intentamos leer cada 2 segundos si aún no tenemos dato válido
            if (active_ && lastReadTemp_ == 0 && (now - tsRetry_ >= 2000)) {
                tsRetry_ = now; 
                
                // 1. Pedir al sensor que mida
                sensors_.requestTemperatures(); 
                
                // 2. Leer el resultado (Index 0 porque solo hay un sensor en el cable)
                float t = sensors_.getTempCByIndex(0);

                // El sensor devuelve -127.00 si hay error o desconexión
                if (t == DEVICE_DISCONNECTED_C) {
                    logger.log(LOG_WARN, "Fallo DS18B20: Desconectado o error (-127).");
                } else {
                    lastReadTemp_ = t; 
                    logger.log(LOG_INFO, "Lectura DS18B20 Exitosa: %.2f C", lastReadTemp_);
                }
            }
            
            // Si ya leímos O se acabó el tiempo
            if (lastReadTemp_ != 0.0f || (now - tsTemp_ >= SEG_A_MS(config_.duracion_lectura_seg))) {
                state_ = State::ANALIZANDO;
            }
            break;

        case State::ANALIZANDO:
            if (lastReadTemp_ != 0.0f) {
                // Guardar en el sistema
                MeasurementManager::instance().addMeasurement(MEAS_TEMPERATURE, lastReadTemp_);
            } else {
                logger.log(LOG_WARN, "No hubo lecturas válidas de Temperatura.");
            }
            tsTemp_ = now;
            state_  = State::APAGADO;
            break;
    }
}