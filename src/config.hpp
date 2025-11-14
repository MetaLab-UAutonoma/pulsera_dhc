#pragma once
#include <Arduino.h>

// ——— Pines y UART ———
#define PIN_SENSOR_TEMP 34
#define SIM_TX          17
#define SIM_RX          16
#define DEBUG_PIN       25
#define PIN_BATTERY_ADC 35

extern HardwareSerial simSerial; 

// Conversión a milisegundos
#define SEG_A_MS(x) ((uint32_t)((x) * 1000UL))

// ——— Flags globales ———
extern bool debugMode, lastDebugMode;
