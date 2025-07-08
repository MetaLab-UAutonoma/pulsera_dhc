# Monitor de Signos Vitales - Guía para Desarrolladores

Este repositorio contiene el firmware para un prototipo de pulsera de monitoreo de salud construida sobre un ESP32. El sistema está diseñado para leer datos de sensores, evaluarlos localmente y transmitirlos a través de un módem celular.

El proyecto está desarrollado para el ecosistema de PlatformIO.

## ¿Qué Hace?

Este sistema monitorea signos vitales de personas en situación de calle (temperatura, BPM, SpO2) para alerta temprana a un equipo de rescatistas en caso de detectar medidas alarmantes.

---

## Hardware

Microcontrolador: ESP32

Sensor de Pulso/SpO2: MAX30100

Sensor de Temperatura: Transistor.

Módem Celular: SIM7600

## Mapa del Proyecto.

El código fuente en la carpeta `src/` está organizado por responsabilidades. Así sabrás dónde buscar y dónde crear nuevos archivos.

data

├── 📁 config/       

│   └── 📜 config.json   -> Variables configurables.

│

src/

├── 📁 business/         -> El cerebro y la lógica del sistema.

│   └── 📁 rules/        -> Las reglas de negocio individuales para cada signo vital.

│

├── 📁 input/            -> Clases para leer datos de los sensores.

│

├── 📁 output/           -> Clases para interactuar con el exterior (ej. Módem).

│

├── 📁 utils/            -> Herramientas compartidas (Logger, listas, etc.).

│

└── 📜 main.cpp          -> El punto de entrada. Aquí se "arma" y configura todo.

---

## Componentes Clave y Flujo de Datos

1.  **Sensores (`input/`):**
    * Su único trabajo es leer el hardware y entregar un valor numérico.
    * En su estado `ANALIZANDO`, llaman al `MeasurementManager` para registrar su dato.

2.  **MeasurementManager (`business/MeasurementManager.cpp`):**
    * **Función:** Es el "historiador". Recibe las mediciones y las guarda en una lista (`MeasurementList`) para cada tipo de signo vital.
    * No sabe nada de reglas ni alertas. Su única misión es **almacenar el historial reciente**.

3.  **Watchdog (`business/Watchdog.cpp`):**
    * **Función:** Es el "orquestador". No contiene lógica de negocio por sí mismo.
    * Periódicamente recorre una lista de "reglas" y les dice: "es tu turno, valida tus datos".

4.  **Las Reglas (`business/rules/`):**
    * **Función:** Cada archivo (ej. `WatchdogRuleTemp.cpp`) contiene la lógica específica para un signo vital.
    * Una regla le pide su historial al `MeasurementManager`, lo analiza según sus propios criterios (umbrales, tiempo de persistencia) y decide si se debe generar una alerta.
