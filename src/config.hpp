#pragma once
#include <Arduino.h>

// ——— Pines y UART ———
#define PIN_SENSOR_TEMP 4
#define SIM_TX          27
#define SIM_RX          26
#define DEBUG_PIN       35
#define PIN_BATTERY_ADC 34
/*#define GPS_TX          27 
#define GPS_RX          26 
*/
#define SIM_PWR_PIN     2
extern HardwareSerial simSerial;
extern HardwareSerial gpsSerial;

// Conversión a milisegundos
#define SEG_A_MS(x) ((uint32_t)((x) * 1000UL))

// ——— Flags globales ———
extern bool debugMode, lastDebugMode;
