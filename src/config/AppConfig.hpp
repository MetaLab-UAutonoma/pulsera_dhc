#pragma once
#include <string>
#include <stdint.h>
#include <cstddef>

// --- Configuración General y de Red ---
struct GeneralConfig {
    std::string telefono_destino;
};

struct NtpConfig {
    std::string server;
    long gmt_offset_sec;
    int daylight_offset_sec;
};

// --- INPUT ---
struct TempSensorConfig {
    float ciclo_monitoreo_seg;
    float duracion_lectura_seg;
    float timer_estabilizacion_seg;
};

struct PoxSensorConfig {
    float ciclo_monitoreo_seg;
    float duracion_lectura_seg;
    float timer_estabilizacion_seg;
};

struct BatterySensorConfig {
    float ciclo_monitoreo_seg;
    float duracion_lectura_seg;
    float timer_estabilizacion_seg;
};

struct InputConfig {
    TempSensorConfig temp_sensor;
    PoxSensorConfig pox_sensor;
    BatterySensorConfig bat_sensor;
};

// --- OUTPUT ---
struct ModemConfig {
    float timer_check_seg;
};

struct OutputConfig {
    ModemConfig modem;
};

// --- BUSINESS ---
struct RuleConfig {
    float min_val, max_val;
    uint32_t alert_sec;
    size_t hist_items;
    uint32_t hist_age_sec;
};

struct WatchdogConfig {
    uint32_t check_interval_ms;
    RuleConfig temp_rule;
    RuleConfig spo2_rule;
    RuleConfig bpm_rule;
    RuleConfig battery_rule;
    RuleConfig gps_rule;
};

struct BusinessConfig {
    WatchdogConfig watchdog;
};

// --- Estructura Raíz ---
struct AppConfig {
    GeneralConfig general;
    NtpConfig ntp;
    InputConfig input;
    OutputConfig output;
    BusinessConfig business;
};