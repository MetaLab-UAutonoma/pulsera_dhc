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

// --- Pines para Módulo LoRa (¡ADAPTAR A TU DISEÑO PCB FINAL!) ---
#define LORA_CS         18  // Chip Select (NSS)
#define LORA_RST        14  // Reset
#define LORA_DIO0       26  // Pin de Interrupción (DIO0)
#define LORA_SCK        5   // Clock
#define LORA_MISO       19  // Master In Slave Out
#define LORA_MOSI       27  // Master Out Slave In
// -----------------------------------------------------------------

extern HardwareSerial simSerial;
extern HardwareSerial gpsSerial;

// Conversión a milisegundos
#define SEG_A_MS(x) ((uint32_t)((x) * 1000UL))

// ——— Flags globales ———
extern bool debugMode, lastDebugMode;
