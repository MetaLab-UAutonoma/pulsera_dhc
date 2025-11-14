# Copilot Instructions for pulsera_dhc

## Project Overview
This is an **ESP32-based health monitoring wearable firmware** that reads vital signs (temperature, SpO2, BPM, battery) from sensors, stores them locally, applies rule-based validation, and sends alerts via cellular modem (SIM7600) to emergency contacts.

**Build System:** PlatformIO (see `platformio.ini`)  
**Framework:** Arduino  
**Language:** C++14  
**Key Libraries:** ArduinoJson, Arduino-MAX30100

---

## Architecture & Data Flow

### Core Components Pattern

The codebase uses **separation of concerns** across three layers:

1. **Input Layer** (`src/input/`)
   - **Pattern:** Abstract `Sensor` base class with state machine (APAGADO → WAITING → LEYENDO → ANALIZANDO)
   - **Responsibility:** Non-blocking sensor reading; calls `MeasurementManager::addMeasurement()` when measurement is ready
   - **Example:** `TempSensor`, `PoxMax30100Sensor` read hardware, accumulate values, emit only when stable
   - **Key Insight:** Sensors are event-driven, not polled directly—they trigger measurements via callback

2. **Business Logic Layer** (`src/business/`)
   - **MeasurementManager:** Singleton that stores timestamped historical data in `MeasurementList` (FIFO with max size/age)
   - **Watchdog:** Singleton orchestrator that periodically (`check_interval_ms`) calls registered `IWatchdogRule` instances
   - **Rules** (`src/business/rules/`): Each `WatchdogRule*` implements business logic for one vital sign
     - Requests history from `MeasurementManager`
     - Applies thresholds (min/max) with **persistence** (alert only after `alert_sec` of continuous violation)
     - Triggers `AlertDispatcher` if conditions met

3. **Output Layer** (`src/output/`)
   - **Sim7600Modem:** Non-blocking cellular modem wrapper
   - **AlertDispatcher:** Singleton that broadcasts alerts to registered `IMessageSender` implementations (e.g., `SmsSender`)

### Data Flow Example: Temperature Alert
```
TempSensor.update() → adds value to MeasurementManager
   ↓
Watchdog.update() → calls WatchdogRuleTemp.validate()
   ↓
WatchdogRuleTemp → fetches history from MeasurementManager
   ↓
Rule checks: is_violation && (now - violation_start) >= alert_sec
   ↓
AlertDispatcher.sendAlert() → SmsSender → Sim7600Modem
```

---

## Configuration System

**Source:** `data/config/config.json` → loaded via `ConfigManager` (singleton)

**Structure:** Nested JSON with distinct sections:
- **general:** Phone number for alerts
- **ntp:** Time server for timestamped logs
- **input:** Sensor cycle/read/stabilization timings (seconds)
- **output:** Modem polling interval
- **business.watchdog:** Rule thresholds, alert persistence, history window size

**Usage Pattern in Code:**
```cpp
const AppConfig& config = ConfigManager::instance().getConfig();
// Access as: config.input.temp_sensor.ciclo_monitoreo_seg
```

**Adding New Settings:**
1. Extend struct in `src/config/AppConfig.hpp`
2. Update JSON schema in `data/config/config.json`
3. Update `ConfigManager::loadFromFile()` (uses ArduinoJson deserialization)

---

## Singleton Pattern & Lifecycle

This codebase heavily uses **singletons** for shared state:
- `Logger logger` (global instance)
- `ConfigManager::instance()`
- `MeasurementManager::instance()`
- `Watchdog::instance()`
- `AlertDispatcher::instance()`

**Setup Flow** (see `main.cpp`):
1. ConfigManager loads JSON
2. Create sensor instances with config
3. Register rules to Watchdog (via `addRule()` with move semantics)
4. Main loop calls `.update(now_millis)` on all components

**Key Principle:** All components use `update(uint32_t now)` for non-blocking execution.

---

## Project-Specific Patterns

### 1. Non-Blocking State Machines
Sensors use `Sensor::State` enum and `update()` instead of blocking delays:
```cpp
// BAD: delay(1000);
// GOOD: if (now - tsLastCheck_ > 1000) { /* do work */ }
```

### 2. Measurement Persistence
Rules don't alert on first threshold violation—they require sustained violation:
```cpp
// From WatchdogRuleTemp logic:
if (violationDetected && !is_alert_active_) {
    alert_condition_since_ = now;
    is_alert_active_ = true;
}
if (is_alert_active_ && (now - alert_condition_since_) >= alert_duration_sec_) {
    // Send alert
}
```

### 3. History Window Configuration
Rules configure `MeasurementManager` with tuples of (history_items, max_age_sec):
```cpp
// Keep last 60 measurements, discard older than 3600 seconds
manager.configure(MEAS_TEMPERATURE, 60, 3600);
```

### 4. Template-Free, Explicit Type Enum
Uses `MeasurementType` enum instead of templates:
```cpp
enum MeasurementType { MEAS_TEMPERATURE, MEAS_SPO2, MEAS_BPM, MEAS_BATTERY_PERCENT, MEAS_BATTERY_VOLTAGE };
```

### 5. Move Semantics for Rules
Rules are stored as `unique_ptr`:
```cpp
auto rule = std::make_unique<WatchdogRuleTemp>(...);
watchdog.addRule(MEAS_TEMPERATURE, std::move(rule));
```

---

## Adding New Features

### Adding a New Sensor Type
1. Create `src/input/NewSensor.hpp/.cpp` extending `Sensor`
2. Implement `init()` and `update(now)` with non-blocking state machine
3. Call `MeasurementManager::instance().addMeasurement(MEAS_TYPE, value)` when ready
4. Add config struct to `AppConfig.hpp`
5. Instantiate in `main.cpp` setup

### Adding a New Watchdog Rule
1. Create `src/business/rules/WatchdogRuleNewVital.hpp/.cpp` implementing `IWatchdogRule`
2. Define thresholds, alert persistence, history size in constructor
3. Implement `validate()` (check history, trigger alerts) and `configureManager()` (set history window)
4. Register in `main.cpp`:
   ```cpp
   auto rule = std::make_unique<WatchdogRuleNewVital>(...config params...);
   watchdog.addRule(MEAS_NEW_TYPE, std::move(rule));
   ```

### Adding a New Alert Sender
1. Create `src/output/senders/NewSender.hpp/.cpp` implementing `IMessageSender`
2. Register in `main.cpp`:
   ```cpp
   AlertDispatcher::instance().addSender(std::make_unique<NewSender>(...));
   ```

---

## Build & Debug

**Build Command:**
```bash
pio run
```

**Upload to Device:**
```bash
pio run -t upload
```

**Monitor Serial Output:**
```bash
pio device monitor --baud 115200
```

**Debug Mode:** Controlled by `DEBUG_PIN` (GPIO 25) in real hardware—pull HIGH to enable verbose logging.

---

## Key Files Reference

| File | Purpose |
|------|---------|
| `src/main.cpp` | System assembly & lifecycle (setup/loop) |
| `src/config/AppConfig.hpp` | Configuration schema |
| `src/business/MeasurementManager.hpp` | Measurement storage (singleton) |
| `src/business/Watchdog.hpp` | Rule orchestrator (singleton) |
| `src/business/rules/IWatchdogRule.hpp` | Rule interface |
| `src/input/Sensor.hpp` | Sensor base class (state machine) |
| `src/output/AlertDispatcher.hpp` | Alert broadcaster |
| `src/utils/logger.hpp` | Timestamped logging (singleton) |
| `platformio.ini` | Build configuration |
| `data/config/config.json` | Runtime configuration |

---

## Common Patterns & Pitfalls

✅ **DO:**
- Use `uint32_t now = millis()` and delta timing in `update()`
- Call `configureManager()` in rule constructor to set history window
- Store rule state (e.g., `is_alert_active_`, `alert_condition_since_`) in rule class
- Use `logger.log(LOG_INFO/WARN/ERROR, fmt, ...)` for debugging

❌ **DON'T:**
- Use `delay()` or blocking calls—breaks sensor responsiveness
- Store persistent state in `Watchdog` itself—put it in the specific rule
- Create multiple instances of singletons—use `::instance()`
- Assume sensors auto-trigger; always call `update(now)` in main loop

---

## Testing Notes

- No formal test framework; testing is manual via serial monitor
- Toggle `DEBUG_PIN` to enable verbose logging without recompiling
- Configuration changes require reflashing `config.json` to device filesystem
- Watchdog check interval is configurable to speed up alert testing
