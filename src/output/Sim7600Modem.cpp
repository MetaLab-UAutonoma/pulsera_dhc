#include "Sim7600Modem.hpp"
#include "utils/logger.hpp" // NECESARIO para logger.log()
#include <Arduino.h>      // NECESARIO para delay(), String, y Serial (UART)
#include "business/MeasurementManager.hpp"



Sim7600Modem::Sim7600Modem(HardwareSerial& p_serial,
                           int p_rxPin,
                           int p_txPin,
                           const char* p_telefonoDestino,
                           const ModemConfig& config)
  : serial_(p_serial)
  , rxPin_(p_rxPin)
  , txPin_(p_txPin)
  , telefonoDestino_(p_telefonoDestino)
  , config_(config)
  , modemActivo_(false)
  , tsModem_(0)
  , tsModemCmd_(0)
  , modemResp_()
  , state_(State::APAGADO)
{}

void Sim7600Modem::init() {

    tsModem_ = millis(); // Reinicia el contador de tiempo
    state_ = State::WAITING_BOOT; // <-- [2] TRANSICIÓN A ESPERA CRÍTICA
    logger.log(LOG_INFO, "SIM7600: Iniciado. Esperando arranque (30s)...");

    // Secuencia de encendido 
    pinMode(SIM_PWR_PIN, OUTPUT);
    digitalWrite(SIM_PWR_PIN, LOW);
    delay(100);
    digitalWrite(SIM_PWR_PIN, HIGH);
    delay(2500); // Presionar botón virtualmente por 2.5s
    digitalWrite(SIM_PWR_PIN, LOW);
    serial_.begin(115200, SERIAL_8N1, rxPin_, txPin_);

    
    delay(1000);
    // COMANDO IMPORTANTE: Encender el motor GPS

    
    tsModem_ = millis();
    state_ = State::WAITING_BOOT;

}

// CORRECCIÓN 1: Agregar corchetes {}
bool Sim7600Modem::isActive() const {
    return modemActivo_;
}

void Sim7600Modem::update(uint32_t p_now) {
    logger.log(LOG_DEBUG, "Modem, estado=%s", stateToString(state_));
    // ... (cuerpo de update, este estaba bien)
    switch (state_) {
        case State::APAGADO:
            if (p_now - tsModem_ >= SEG_A_MS(config_.timer_check_seg)) {
                logger.log(LOG_DEBUG, "Enviando AT");
                serial_.println("AT");
                tsModemCmd_ = p_now;
                modemResp_ = "";
                state_ = State::ESPERAR_RESP;
            }
            else if (modemActivo_ && (p_now - lastGpsPoll_ >= gpsInterval_)) {
                logger.log(LOG_DEBUG, "Solicitando coordenadas GPS...");
                serial_.println("AT+CGPSINFO"); // Comando para pedir info
                tsModemCmd_ = p_now;
                modemResp_ = "";
                state_ = State::ESPERAR_RESP_GPS; // Vamos al nuevo estado
                lastGpsPoll_ = p_now;
            }
            break;
        case State::WAITING_BOOT: // <-- [3] NUEVO ESTADO
            // Espera CRÍTICA de 30 segundos (o más, 30s es un buen mínimo)
            if (p_now - tsModem_ >= SEG_A_MS(30)) { 
                logger.log(LOG_INFO, "SIM7600: Fin de espera. Forzando chequeo AT.");
                // La transición a APAGADO con tsModem_ = 0 forzará el envío del primer AT en el siguiente ciclo.
                state_ = State::APAGADO;
                tsModem_ = 0; 
            }
            break;

        case State::ESPERAR_RESP:
            while (serial_.available()) {
                modemResp_ += char(serial_.read());
            }
            if (modemResp_.endsWith("\n") || p_now - tsModemCmd_ >= 2000) {
                logger.log(LOG_DEBUG, "Pasando a ANALIZAR_RESP");
                state_ = State::ANALIZAR_RESP;
            }
            break;

            case State::ESPERAR_RESP_GPS:
            while (serial_.available()) {
                modemResp_ += char(serial_.read());
            }

            if (modemResp_.indexOf("ERROR") != -1) {
                 logger.log(LOG_WARN, "GPS devolvió ERROR (¿Aún apagado?)");
                 state_ = State::APAGADO;
                 tsModem_ = p_now;
            }
            // Si el tiempo de espera se cumple, o la respuesta ya contiene el fin de línea
            if (p_now - tsModemCmd_ >= 3000 || modemResp_.endsWith("\n")) { 
                
                // *** 🔑 CÓDIGO FALTANTE: RECEPCIÓN Y TRANSFORMACIÓN DEL DATO ***
                if (modemResp_.indexOf("+CGPSINFO:") != -1 && modemResp_.indexOf("ERROR") == -1) {
                    logger.log(LOG_DEBUG, "GPS: Respuesta completa. Analizando datos...");
                    parseGpsData(modemResp_); // <--- ¡Esta es la llamada a la transformación!
                } else if (modemResp_.indexOf("+CGPSINFO: ,,,,,,,,") != -1) {
                    logger.log(LOG_INFO, "GPS: Sin Fix (datos vacíos).");
                    MeasurementManager::instance().addMeasurement(MEAS_GPS_SATS, 0.0f);
                } else {
                    logger.log(LOG_WARN, "GPS: Timeout o respuesta inesperada.");
                }
                // ***************************************************************
                
                state_ = State::APAGADO;
                tsModem_ = p_now;
            }
            break;
            // Timeout de seguridad (2 seg)
            if (p_now - tsModemCmd_ >= 2000) {
                logger.log(LOG_WARN, "Timeout esperando GPS");
                state_ = State::APAGADO;
            }
            if (p_now - tsModemCmd_ >= 3000) { // Dale 3 segundos
                logger.log(LOG_WARN, "Timeout esperando GPS");
                state_ = State::APAGADO;
            }
            break;

        case State::ANALIZAR_RESP:
            logger.log(LOG_DEBUG, "RESP=\"%s\"", modemResp_.c_str());
            modemActivo_ = (modemResp_.indexOf("OK") != -1);
            
            if (modemActivo_) {
                logger.log(LOG_INFO, "SIM7600 activo.");
                
                // --- NUEVA LÓGICA: Encender GPS aquí ---
                if (!gpsStarted_) {
                    serial_.println("AT+CGPS=1");
                    logger.log(LOG_INFO, "SIM7600: Activando motor GPS ahora que el módem responde.");
                    gpsStarted_ = true; // Marcamos para no enviarlo mil veces
                }
                // ---------------------------------------
            } else {
                logger.log(LOG_WARN, "SIM7600 no detectado.");
            }

            tsModem_ = p_now;
            state_ = State::APAGADO;
            break;
    }
}

// CORRECCIÓN 2: Arreglar la definición de stateToString
const char* Sim7600Modem::stateToString(State p_state) {
    switch (p_state) {
        case State::APAGADO:      return "APAGADO";
        case State::WAITING_BOOT: return "WAITING_BOOT"; // <-- [3] NUEVO CASE
        case State::ESPERAR_RESP: return "ESPERAR_RESP";
        case State::ESPERAR_RESP_GPS: return "ESPERAR_RESP_GPS"; 
        case State::ANALIZAR_RESP:return "ANALIZAR_RESP";
    }
    return "UNKNOWN";
}

// Nuevo método sendSMS
bool Sim7600Modem::sendSMS(const String& p_msg) {
    if (!modemActivo_) {
        logger.log(LOG_ERROR, "SMS: Módem no activo o fuera de ciclo de chequeo.");
        return false;
    }
    state_ = State::APAGADO;
    while (serial_.available()) serial_.read();

    // 1. Configurar modo texto
    serial_.println("AT+CMGF=1");
    delay(500); 

    // 2. Comando de envío con número destino
    String cmd = "AT+CMGS=\"" + String(telefonoDestino_) + "\"";
    serial_.println(cmd);
    delay(500); // Esperar el prompt '>'

    // 3. Enviar cuerpo del mensaje
    serial_.print(p_msg);

    // 4. Terminar mensaje con Ctrl-Z (ASCII 26)
    serial_.write(26);

    delay(7000); 
    logger.log(LOG_INFO, "SMS: Comando de envío ejecutado para %s.", telefonoDestino_);

    return true; 
}
    double Sim7600Modem::convertToDecimal(String raw, String hemi) {
    if (raw.length() == 0) return 0.0;
    
    // Formato raw: DDDMM.MMMM (ej: 3322.1234)
    // Los últimos 7 caracteres aprox son los minutos. 
    // Punto decimal está en raw.indexOf('.')
    
    int dotPos = raw.indexOf('.');
    if (dotPos < 2) return 0.0;

    // Separamos grados de minutos. Los minutos son siempre los 2 dígitos antes del punto + decimales
    double rawVal = raw.toDouble();
    int degrees = (int)(rawVal / 100); 
    double minutes = rawVal - (degrees * 100);
    
    double result = degrees + (minutes / 60.0);

    // Corregir signo según hemisferio
    if (hemi == "S" || hemi == "W") {
        result *= -1.0;
    }
    return result;
}

void Sim7600Modem::parseGpsData(String resp) {
    // Ejemplo respuesta: +CGPSINFO: 3322.1234,S,07033.5678,W,251124,123456,10.0,...
    // Si no hay señal devuelve: +CGPSINFO: ,,,,,,,,
    
    int idxStart = resp.indexOf("+CGPSINFO: ");
    if (idxStart == -1) return;
    
    String data = resp.substring(idxStart + 11); // Quitar encabezado
    
    // Parseo manual por comas
    int firstComma = data.indexOf(',');
    if (firstComma == 0) {
        logger.log(LOG_DEBUG, "GPS: Sin señal (Fix not available)");
        return; // Datos vacíos
    }

    // LATITUD
    String latRaw = data.substring(0, firstComma);
    int secondComma = data.indexOf(',', firstComma + 1);
    String latHemi = data.substring(firstComma + 1, secondComma);
    
    // LONGITUD
    int thirdComma = data.indexOf(',', secondComma + 1);
    String lonRaw = data.substring(secondComma + 1, thirdComma);
    int fourthComma = data.indexOf(',', thirdComma + 1);
    String lonHemi = data.substring(thirdComma + 1, fourthComma);
    
    // CONVERSIÓN
    double finalLat = convertToDecimal(latRaw, latHemi);
    double finalLon = convertToDecimal(lonRaw, lonHemi);

    if (finalLat != 0.0 && finalLon != 0.0) {
        logger.log(LOG_INFO, "GPS SIM7600: Lat=%.6f Lon=%.6f", finalLat, finalLon);
        
        // ENVIAR AL MEASUREMENT MANAGER
        // Asegúrate de incluir "business/MeasurementManager.hpp" arriba en el .cpp
        MeasurementManager::instance().addMeasurement(MEAS_GPS_LAT, (float)finalLat);
        MeasurementManager::instance().addMeasurement(MEAS_GPS_LON, (float)finalLon);
        MeasurementManager::instance().addMeasurement(MEAS_GPS_SATS, 8.0f);
        
        // Opcional: Podrías parsear velocidad también si sigues la cadena de comas
    }
    else {
        // Opcional: Si quieres reportar pérdida de señal
        MeasurementManager::instance().addMeasurement(MEAS_GPS_SATS, 0.0f);
    }
}
