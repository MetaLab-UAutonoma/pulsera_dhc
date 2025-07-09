#include "WatchdogRuleSpO2.hpp"
#include "business/rules/IWatchdogRule.hpp"
#include "business/MeasurementManager.hpp"
#include "utils/logger.hpp"
#include "output/Sim7600Modem.hpp"

// Ya NO necesitas esto:
// extern Sim7600Modem modem;

WatchdogRuleSpO2::WatchdogRuleSpO2(float min_val, float max_val, uint32_t alert_duration_sec,
                                   size_t history_items, uint32_t history_age_sec,
                                   Sim7600Modem* modem)
    : manager_(MeasurementManager::instance()),
      min_threshold_(min_val),
      max_threshold_(max_val),
      alert_duration_sec_(alert_duration_sec),
      history_items_(history_items),
      history_age_sec_(history_age_sec),
      modem_(modem),
      alert_condition_since_(0),
      is_alert_active_(false)
{}

void WatchdogRuleSpO2::configureManager(MeasurementManager& manager) {
    manager.configure(MEAS_SPO2, history_items_, history_age_sec_);
}

void WatchdogRuleSpO2::validate() {
    Serial.println(">> validate() iniciado");

    const MeasurementList* history = manager_.getHistory(MEAS_SPO2);
    if (!history || history->getData().empty()) {
        Serial.println("No hay historial o está vacío. Salgo.");
        return;
    }

    const auto& last = history->getData().back();
    Serial.print("Última medición SpO2: ");
    Serial.println(last.value);

    const bool is_out_of_bounds = (last.value < min_threshold_ || last.value > max_threshold_);

    Serial.print("¿Fuera de rango? ");
    Serial.println(is_out_of_bounds ? "Sí" : "No");

    const char* name = "SpO2";

    uint32_t nowSec = millis() / 1000;

    if (is_out_of_bounds) {
        if (alert_condition_since_ == 0) {
            alert_condition_since_ = nowSec;  // Usar tiempo relativo consistente
            logger.log(LOG_INFO, "Watchdog [%s]: Condición iniciada (valor: %.1f)", name, last.value);
        } else if (!is_alert_active_ && (nowSec - alert_condition_since_) >= alert_duration_sec_) {
            is_alert_active_ = true;
            logger.log(LOG_WARN, "¡ALERTA [%s]! Valor fuera de rango por más de %u seg.", name, alert_duration_sec_);

            // ✅ Enviar alerta por SMS solo si módem está activo
            if (modem_ && modem_->isActive()) {
                String mensaje = String("ALERTA: SpO2 fuera de rango (") + String(last.value, 1) + "%). El paciente tiene un problema.";
                modem_->enviarAlerta(mensaje);
            } else {
                logger.log(LOG_WARN, "No se pudo enviar SMS: módem no disponible o inactivo.");
            }
        }
    } else {
        if (is_alert_active_) {
            logger.log(LOG_INFO, "Watchdog [%s]: Condición resuelta.", name);
        }
        is_alert_active_ = false;
        alert_condition_since_ = 0;
    }
}
