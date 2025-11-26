// input/sensor_temp.cpp
#include "input/TempSensor.hpp"
#include "business/MeasurementManager.hpp"
#include <DHT.h>
static float lastReadTemp = 0.0f; // <-- ¡Esta línea debe estar aquí!

TempSensor::TempSensor(int p_pinSensor, const TempSensorConfig& config)
  : pinSensor_(p_pinSensor)
  , config_(config)
  , active_(false)
  , tsTemp_(0)
  , tsWaitingTemp_(0)
  , tsLeerTemp_(0)
 /* , sumTemp_(0)
  , countTemp_(0)
  */
  , state_(State::APAGADO)
  , alertT_(false)
  , dht_(p_pinSensor, DHTTYPE)
{}

void TempSensor::init() {
    dht_.begin();
    active_ = true;
}

void TempSensor::update(uint32_t now) {
    logger.log(LOG_DEBUG, "Temp, estado=%s", Sensor::stateToString(state_));

    switch (state_) {
        case State::APAGADO:
            if (now - tsTemp_ >= SEG_A_MS(config_.ciclo_monitoreo_seg)) {
                logger.log(LOG_DEBUG, "APAGADO→WAITING");
                active_       = true;
                tsWaitingTemp_= now;
                tsTemp_       = now;
                lastReadTemp  = 0; // Reiniciamos la lectura
                sumTemp_      = 0;
                countTemp_    = 0;
                tsRetry_      = 0;
                state_        = State::WAITING;
            }
            break;

        case State::WAITING:
            if (now - tsWaitingTemp_ >= SEG_A_MS(config_.timer_estabilizacion_seg)) {
                logger.log(LOG_DEBUG, "WAITING→LEYENDO");
                tsLeerTemp_ = now;
                state_      = State::LEYENDO;
            }
            break;
        
        case State::LEYENDO:
            // Solo necesitamos una lectura en este estado
            if (active_ && lastReadTemp == 0 && (now - tsRetry_ >= 2000)) {
                tsRetry_    = now; 
                tsLeerTemp_ = now;
                float t = dht_.readTemperature(); // <-- LECTURA DIGITAL DHT
                
                if (isnan(t)) {
                    logger.log(LOG_WARN, "Fallo al leer del sensor DHT. Retentando...");
                } else {
                    lastReadTemp = t; 
                    logger.log(LOG_DEBUG, "Lectura DHT Exitosa %.2f C", lastReadTemp);
                }
            }
            
            // Si hay lectura o si el tiempo de lectura se agotó, pasa a analizar
            if (lastReadTemp != 0.0f || (now - tsTemp_ >= SEG_A_MS(config_.duracion_lectura_seg))) {
                logger.log(LOG_DEBUG, "LEYENDO→ANALIZANDO");
                state_ = State::ANALIZANDO;
            }
            break;

        case State::ANALIZANDO:
            if (lastReadTemp != 0.0f) {
                logger.log(LOG_INFO, "Temp DHT=%.2f°C", lastReadTemp);
                MeasurementManager::instance().addMeasurement(MEAS_TEMPERATURE, lastReadTemp);
                
                // OPCIONAL: Si quieres medir la humedad, se debe añadir un MEAS_HUMIDITY
                // float h = dht_.readHumidity();
                // if (!isnan(h)) MeasurementManager::instance().addMeasurement(MEAS_HUMIDITY, h);

            } else {
                logger.log(LOG_WARN, "No hubo lecturas válidas de DHT en el periodo.");
            }
            tsTemp_ = now;
            state_  = State::APAGADO;
            break;
    }
}


        /*case State::LEYENDO:
            if (active_) {
                tsLeerTemp_ = now;
                float v = analogRead(pinSensor_) * (3.3f / 4095.0f);
                float t = (0.7f - v)/0.002f + 25.0f;
                logger.log(LOG_DEBUG, "read t=%.2f", t);
                sumTemp_ += t;
                countTemp_++;
            }
            if (now - tsTemp_ >= SEG_A_MS(config_.duracion_lectura_seg)) {
                logger.log(LOG_DEBUG, "LEYENDO→ANALIZANDO");
                state_ = State::ANALIZANDO;
            }
            break;

        case State::ANALIZANDO:
            if (countTemp_ > 0) {
                float avgT = sumTemp_ / countTemp_;
                logger.log(LOG_INFO, "Temp avg=%.2f°C", avgT);
                // TODO Measurement Manager (e.g. envío SMS si fuera necesario)
                MeasurementManager::instance().addMeasurement(MEAS_TEMPERATURE, avgT);
            } else {
                logger.log(LOG_WARN, "No hubo lecturas de temp en el periodo.");
            }
            tsTemp_ = now;
            state_  = State::APAGADO;
            break;
    }
}*/
