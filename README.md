# Monitor de Signos Vitales - Guía para Desarrolladores

Bienvenido al proyecto de Monitoreo Remoto. Este documento es tu guía para entender la estructura, filosofía y flujo de trabajo del sistema de forma rápida y directa.

## ¿Qué Hacemos?

Este sistema monitorea signos vitales (temperatura, BPM, SpO2) de manera continua. Su principal característica es que no solo lee datos, sino que los **analiza en el tiempo** para generar alertas inteligentes, evitando falsos positivos causados por lecturas momentáneas.

---

## La Filosofía del Diseño: "Abierto/Cerrado"

La arquitectura de este proyecto se basa en un principio clave: el **Principio Abierto/Cerrado**. En términos simples:

* **Abierto a la Extensión:** Puedes añadir nuevas funcionalidades (como un nuevo sensor o una nueva regla de alerta) sin problemas.
* **Cerrado a la Modificación:** No necesitas modificar el código central existente para añadir esas nuevas funciones.

Esto hace que el sistema sea robusto, predecible y fácil de escalar. No tienes que "cazar" dónde hacer un cambio; simplemente añades un nuevo archivo que sigue un contrato establecido.

---

## Mapa del Proyecto: ¿Dónde está cada cosa?

El código fuente en la carpeta `src/` está organizado por responsabilidades. Así sabrás dónde buscar y dónde crear nuevos archivos.

src/
├── 📁 business/         -> El cerebro y la lógica del sistema.
│   └── 📁 rules/       -> Las reglas de negocio individuales para cada signo vital.
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

El sistema funciona como una cadena de montaje. Entender el rol de cada pieza principal te dará el control total.

**El Flujo:** `Sensor` → `MeasurementManager` → `Watchdog` → `Regla Específica` → `Alerta`

1.  **Sensores (`input/`):**
    * Su único trabajo es leer el hardware y entregar un valor numérico.
    * En su estado `ANALIZANDO`, llaman al `MeasurementManager` para registrar su dato.

2.  **MeasurementManager (`business/MeasurementManager.cpp`):**
    * **Función:** Es el "historiador". Recibe las mediciones y las guarda en una lista (`FifoList`) para cada tipo de signo vital.
    * No sabe nada de reglas ni alertas. Su única misión es **almacenar el historial reciente**.

3.  **Watchdog (`business/Watchdog.cpp`):**
    * **Función:** Es el "orquestador". No contiene lógica de negocio por sí mismo.
    * Periódicamente (cada 5 segundos), recorre una lista de "reglas" y les dice: "es tu turno, valida tus datos".

4.  **Las Reglas (`business/rules/`):**
    * **Función:** ¡Aquí vive la magia! Cada archivo (ej. `WatchdogRuleTemp.cpp`) contiene la lógica específica para un signo vital.
    * Una regla le pide su historial al `MeasurementManager`, lo analiza según sus propios criterios (umbrales, tiempo de persistencia) y decide si se debe generar una alerta.

---

## Guía Práctica: Configuración y Extensión

### ¿Cómo configuro las alertas?

Toda la configuración del sistema se realiza en `config.cpp`, dentro de la función `configureBusinessRules()`. Es el único archivo que necesitas tocar para ajustar los parámetros.

```cpp
// config.cpp

void configureBusinessRules() {
    // 1. Crear una regla para la Temperatura
    auto tempRule = std::make_unique<WatchdogRuleTemp>(
        /* min_val   */ 36.0f,    // Umbral mínimo
        /* max_val   */ 38.5f,    // Umbral máximo
        /* alert_sec */ 300,      // Cuántos segundos debe persistir para alertar
        /* hist_items*/ 60,       // Cuántas lecturas guardar
        /* hist_age  */ 3600      // Antigüedad máxima de una lectura
    );

    // 2. Entregar la regla al Watchdog
    Watchdog::instance().addRule(MEAS_TEMPERATURE, std::move(tempRule));

    // Repetir para BPM, SpO2, etc.
}