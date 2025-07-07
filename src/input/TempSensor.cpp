// input/sensor_temp.cpp
#include "input/TempSensor.hpp"

TempSensor::TempSensor(int p_pinSensor, const TempSensorConfig& config)
  : pinSensor_(p_pinSensor)
  , config_(config)
  , active_(false)
  , tsTemp_(0)
  , tsWaitingTemp_(0)
  , tsLeerTemp_(0)
  , sumTemp_(0)
  , countTemp_(0)
  , state_(State::APAGADO)
  , alertT_(false)
{}

void TempSensor::init() {
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
                sumTemp_      = 0;
                countTemp_    = 0;
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
            } else {
                logger.log(LOG_WARN, "No hubo lecturas de temp en el periodo.");
            }
            tsTemp_ = now;
            state_  = State::APAGADO;
            break;
    }
}
