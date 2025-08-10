# 30-Day Embedded Systems Bootcamp

Repositorio con el progreso del plan de 30 días para aprender desarrollo de software embebido desde cero hasta un proyecto final completo.

---

## Requisitos

Antes de empezar, debes tener instalado:

- **GCC y Make** (compilador y herramientas de build)
- **VS Code** con extensiones:  
  - C/C++ (Microsoft)  
  - Makefile Tools  
  - Code Runner  
- **QEMU** (simulador de hardware)  
- **Git**  
- **(Opcional) Proteus** para simulación gráfica

---

## Estructura del Repo

Cada día tendrá su carpeta con:
- `README.md` → teoría, conceptos y pasos del día
- `src/` → código y ejemplos
- Archivos extra → diagramas, scripts, etc.

System_Embedded/
├─ Dia01/
│ ├─ README.md
│ └─ src/
├─ Dia02/
│ ├─ README.md
│ └─ src/
...
└─ README.md (este archivo)

yaml
Copy

---

## Plan de 30 Días

| Día  | Tema Principal | Resultado Esperado |
|------|----------------|--------------------|
| 1 | Fundamentos, microcontrolador, firmware vs software, real-time | Entorno listo y teoría en README |
| 2 | Entradas/salidas digitales: LED y botón | LED controlado por botón |
| 3 | Memoria RAM, Flash, Stack, Heap | Ejemplo de stack overflow |
| 4 | Ciclo de vida de app embebida, Makefile básico | Compilación sin IDE |
| 5 | Interrupciones vs Polling | Comparativa de latencias |
| 6 | Timers: software vs hardware | LED parpadeante preciso |
| 7 | Proyecto 1: LED + Botón + Timer | Proyecto documentado en README |
| 8 | UART: concepto y driver básico | Comunicación serial simulada |
| 9 | SPI: configuración y driver | Simulación de sensor SPI |
| 10 | I2C: configuración y driver | Comunicación maestro-esclavo |
| 11 | ADC/DAC: lectura y escritura | Lectura de potenciómetro simulada |
| 12 | PWM: control de brillo/motor | LED con brillo variable |
| 13 | Structs, unions, enums, typedef | Drivers refactorizados |
| 14 | Proyecto 2: sensor + comunicación | Proyecto documentado |
| 15 | Introducción a RTOS, multitarea | Blinky multitarea |
| 16 | Semáforos, mutex, deadlocks | Ejemplo de sincronización |
| 17 | Debug con GDB, core dumps | Crash analizado |
| 18 | Memoria dinámica y fragmentación | Ejemplo de leak y solución |
| 19 | Logger en C | Sistema de logs con niveles |
| 20 | Bajo consumo, sleep/wakeup | Sistema que despierta con botón |
| 21 | Proyecto 3: RTOS + sensores + bajo consumo | Proyecto documentado |
| 22 | BLE: GATT y servicios | Beacon con datos |
| 23 | WiFi embebido: servidor web | Página mostrando sensor |
| 24 | Bootloader y OTA | Simulación de actualización |
| 25 | Protocolos industriales: Modbus/CAN | Comunicación entre nodos |
| 26 | Seguridad embebida: buffer overflow | Ataque simulado y solución |
| 27 | Integración de sensores reales | Ejemplo con DHT22 o MPU6050 |
| 28 | Planificación de proyecto final | Diagrama y especificaciones |
| 29 | Proyecto Final: sistema completo | Integración total |
| 30 | Documentación y publicación | Repo listo y presentable |

---

## Uso

Clonar repo:
```bash
git clone https://github.com/usuario/System_Embedded.git
cd System_Embedded
Agregar nuevo día:

bash
Copy
mkdir Dia04
touch Dia04/README.md
mkdir Dia04/src
Contribuciones
Mejorar documentación

Añadir ejemplos

Sugerir ejercicios

Traducir contenidos

Licencia
MIT – Uso libre, con atribución.
