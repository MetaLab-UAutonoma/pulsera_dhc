#pragma once

#include <Arduino.h>
#include <HardwareSerial.h>
// ——— Pines y UART ———
#define PIN_SENSOR_TEMP 34
#define SIM_TX          17
#define SIM_RX          16
#define DEBUG_PIN       25

// ——— Flags globales ———
extern HardwareSerial simSerial; 

extern bool debugMode, lastDebugMode;


extern String telefonoDestino;


extern bool debugMode;
extern bool lastDebugMode;

extern float timerCheckSeg;

bool cargarConfiguracion();

// Conversión a milisegundos
#define SEG_A_MS(x) ((uint32_t)((x) * 1000UL))


