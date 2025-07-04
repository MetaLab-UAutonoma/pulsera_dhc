#pragma once
#include <Arduino.h>

// ——— Pines y UART ———
#define PIN_SENSOR_TEMP 34
#define SIM_TX          17
#define SIM_RX          16
#define DEBUG_PIN       25

extern HardwareSerial simSerial;      // declarado en main.cpp
extern const char* telefonoDestino;  // idem

// ——— Parámetros de tiempo (segundos) ———
#define CICLO_MONITOREO_SEG     30.0f
#define DURACION_SENSOR_SEG     20.0f
#define CHEQUEO_MODEM_SEG        5.0f
#define REPORTING_PERIOD_MS     1000UL

// Retrasos de estabilización (segundos)
#define WAIT_PULSO_SEG    10.0f
#define WAIT_TEMP_SEG      0.5f

// Conversión a milisegundos
#define SEG_A_MS(x) ((uint32_t)((x) * 1000UL))
#define CHEQUEO_MODEM_MS SEG_A_MS(CHEQUEO_MODEM_SEG)
#define WAIT_PULSO_MS    SEG_A_MS(WAIT_PULSO_SEG)
#define WAIT_TEMP_MS     SEG_A_MS(WAIT_TEMP_SEG)

// ——— Flags globales ———
extern bool debugMode, lastDebugMode;
