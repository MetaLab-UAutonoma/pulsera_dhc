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