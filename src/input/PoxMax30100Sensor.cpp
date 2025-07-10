// input/sensor_max30100.cpp
#include "PoxMax30100Sensor.hpp"
#include "business/MeasurementManager.hpp"


#define SEG_A_MS(x) ((uint32_t)((x) * 1000))

PoxMax30100Sensor::PoxMax30100Sensor(PulseOximeter& p_pox, const PoxSensorConfig& config)
  : pox_(p_pox)
  , config_(config)
  , active_(false)
  , tsPulso_(0)
  , tsWaitingPulso_(0)
  , tsLeerPulso_(0)
  , sumBpm_(0)
  , sumSpo2_(0)
  , countPulso_(0)
  , state_(State::APAGADO)
{}

void PoxMax30100Sensor::init() {
    // idéntico a initSensorPox(...)
    if (pox_.begin(PULSEOXIMETER_DEBUGGINGMODE_NONE)) {
        pox_.setIRLedCurrent(MAX30100_LED_CURR_7_6MA);
        active_ = true;
        logger.log(LOG_INFO, "MAX30100 listo.");
    } else {
        active_ = false;
        logger.log(LOG_ERROR, "MAX30100 FAIL.");
    }
    // reiniciar contadores y timestamps
    tsPulso_         = millis();
    tsWaitingPulso_  = tsPulso_;
    sumBpm_          = sumSpo2_ = 0;
    countPulso_      = 0;
}

void PoxMax30100Sensor::update(uint32_t p_now) {
    logger.log(LOG_DEBUG, "Pulso, estado=%s", Sensor::stateToString(state_));

    float b = 0.0f, s = 0.0f;
    switch (state_) {
        case State::APAGADO:
            if (p_now - tsPulso_ >= SEG_A_MS(config_.ciclo_monitoreo_seg)) {
                logger.log(LOG_DEBUG, "APAGADO→WAITING");
                init();
                state_ = State::WAITING;
            }
            break;

        case State::WAITING:
            if (p_now - tsWaitingPulso_ >= SEG_A_MS(config_.timer_estabilizacion_seg)) {
                logger.log(LOG_DEBUG, "WAITING→LEYENDO");
                tsLeerPulso_ = p_now;
                state_       = State::LEYENDO;
            }
            pox_.update();
            b = pox_.getHeartRate();
            s = pox_.getSpO2();
            break;

        case State::LEYENDO:
            if (active_) {
                tsLeerPulso_ = p_now;
                pox_.update();
                b = pox_.getHeartRate();
                s = pox_.getSpO2();
                logger.log(LOG_DEBUG, "Pulso, b=%.2f s=%.2f", b, s);
                if (!isnan(b) && b > 0) sumBpm_  += b;
                if (!isnan(s) && s > 0) sumSpo2_ += s;
                countPulso_++;
            }
            if (p_now - tsPulso_ >= SEG_A_MS(config_.duracion_lectura_seg)) {
                logger.log(LOG_DEBUG, "LEYENDO→ANALIZANDO");
                state_ = State::ANALIZANDO;
            }
            break;

        case State::ANALIZANDO:
            if (countPulso_ > 0) {
                float avgB = sumBpm_ / countPulso_;
                float avgS = sumSpo2_ / countPulso_;
                logger.log(LOG_INFO, "BPM avg=%.2f  SpO2 avg=%.2f", avgB, avgS);
                MeasurementManager::instance().addMeasurement(MEAS_BPM, avgB);
                MeasurementManager::instance().addMeasurement(MEAS_SPO2, avgS);
            } else {
                logger.log(LOG_WARN, "No hubo lecturas de pulso en el periodo.");
            }
            tsPulso_ = p_now;
            state_   = State::APAGADO;
            break;
    }
}
