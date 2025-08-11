# 📟 Simulación de GPIO + Debounce + TTY (Modo RAW)

Proyecto educativo del **Día 3** para simular un microcontrolador en PC y practicar:
- Control de **GPIO** (entradas/salidas digitales)
- Filtrado de rebotes (**debounce**)
- Lectura de teclado en **modo raw** y **no bloqueante**

Diseñado para ejecutarse en **Linux/macOS/WSL** y portable a microcontroladores reales sin modificar la lógica de aplicación.

---

## 🧠 Idea General

Separar el código en **capas independientes** para permitir portabilidad entre simulación y hardware real:

| Capa            | Función                                                     | Archivos                                          |
|-----------------|-------------------------------------------------------------|---------------------------------------------------|
| **Aplicación**  | Control de LED y botón con debounce.                        | `src/main_switch.c`, `src/main_toggle.c`          |
| **GPIO**        | Inicializar pines, leer/escribir, configurar resistencias.  | `include/gpio.h`, `src/gpio_sim.c`                |
| **Tiempo**      | Funciones de tiempo y retardo.                              | `include/timeutil.h`, `src/timeutil.c`            |
| **Debounce**    | Filtrar rebotes mecánicos de botones.                       | `include/debounce.h`, `src/debounce.c`            |
| **TTY**         | Lectura de teclado sin bloqueo y sin eco.                   | `include/tty.h`, `src/tty.c`                      |
| **Def. Pines**  | IDs lógicos de pines LED y botón.                           | `include/pins.h`                                  |

---

## 📂 Estructura del Proyecto

    Dia3/Simulacion_led_modular/
    ├─ include/
    │  ├─ gpio.h
    │  ├─ tty.h
    │  ├─ pins.h
    │  ├─ debounce.h
    │  └─ timeutil.h
    ├─ src/
    │  ├─ main_switch.c
    │  ├─ main_toggle.c
    │  ├─ gpio_sim.c
    │  ├─ tty.c
    │  ├─ debounce.c
    │  └─ timeutil.c
    ├─ makefile
    ├─ build/           # Archivos compilados
    └─ bin/             # Ejecutables

---

## 📄 Descripción de Archivos

| Archivo         | Tipo   | Descripción                                  | Propósito                          |
|-----------------|--------|----------------------------------------------|------------------------------------|
| `gpio.h`        | Header | API pública de GPIO.                         | Abstraer el acceso a pines.        |
| `gpio_sim.c`    | Código | Implementación simulada de GPIO en PC.       | Probar sin hardware real.          |
| `tty.h`         | Header | API para control de terminal.                | Evitar bloqueo/eco en teclado.     |
| `tty.c`         | Código | TTY modo raw + lectura no bloqueante.        | Lectura en tiempo real.            |
| `pins.h`        | Header | Definición de pines lógicos.                 | Facilitar cambios de mapeo.        |
| `debounce.h`    | Header | API de debounce.                             | Filtrar ruido mecánico.            |
| `debounce.c`    | Código | Implementación de debounce.                  | Lógica de filtrado de señal.       |
| `timeutil.h`    | Header | API de tiempo.                               | Medir ms y pausar ejecución.       |
| `timeutil.c`    | Código | Implementación POSIX de tiempo.              | Precisión en milisegundos.         |
| `main_switch.c` | App    | Modo SWITCH: LED sigue el botón.             | Debounce por nivel.                |
| `main_toggle.c` | App    | Modo TOGGLE: LED alterna en cada pulsación.  | Debounce por flanco.               |

---

## 🛠️ Estructuras y Tipos

### `gpio_mode_t` — Modo de pin

    typedef enum {
        GPIO_INPUT = 0,
        GPIO_OUTPUT = 1
    } gpio_mode_t;

### `gpio_pull_t` — Resistencia interna

    typedef enum {
        GPIO_NOPULL = 0,
        GPIO_PULLUP = 1,
        GPIO_PULLDOWN = 2
    } gpio_pull_t;

### `gpio_slot_t` — Estructura interna (simulación)

    typedef struct {
        gpio_mode_t mode;   // INPUT o OUTPUT
        gpio_pull_t pull;   // NOPULL, PULLUP, PULLDOWN
        int value;          // Valor si es OUTPUT
        int input_raw;      // Valor crudo si es INPUT
    } gpio_slot_t;

---

## 📜 Funciones Principales

| Función                                    | Archivo       | Descripción                                         |
|--------------------------------------------|---------------|-----------------------------------------------------|
| `void gpio_init(void)`                      | `gpio_sim.c`  | Inicializa todos los pines (INPUT, NOPULL, 0).      |
| `void gpio_mode(int pin, gpio_mode_t mode)` | `gpio_sim.c`  | Configura modo del pin (INPUT/OUTPUT).              |
| `void gpio_set_pull(int pin, gpio_pull_t)`  | `gpio_sim.c`  | Configura resistencia interna si es entrada.        |
| `void gpio_write(int pin, int value)`       | `gpio_sim.c`  | Escribe 0/1 en un pin de salida.                    |
| `int  gpio_read(int pin)`                   | `gpio_sim.c`  | Lee valor lógico del pin (0/1).                     |
| `void gpio_simulate_input(int pin,int val)` | `gpio_sim.c`  | Inyecta valor crudo en un pin de entrada (sim).     |
| `void tty_raw_enable(void)`                 | `tty.c`       | Activa modo raw + O_NONBLOCK en stdin.              |
| `void tty_raw_disable(void)`                | `tty.c`       | Restaura configuración original del terminal.       |
| `int  tty_getch_nonblock(void)`             | `tty.c`       | Lee tecla sin bloquear (−1 si no hay).              |
| `int  debounce_state(int raw,long long ms)` | `debounce.c`  | Devuelve nivel estable tras ms de estabilidad.      |
| `int  debounce_press(int raw,long long ms)` | `debounce.c`  | 1 solo en flanco 0→1 estable (una vez).             |
| `long long now_ms(void)`                    | `timeutil.c`  | Tiempo actual en milisegundos.                      |
| `void sleep_ms(int ms)`                     | `timeutil.c`  | Pausa ejecución en milisegundos.                    |

---

## 🖥️ Programas de Ejemplo

### `main_switch.c` — Modo SWITCH

- Teclas: `'1'` → LED ON, `'0'` → LED OFF, `'q'` → salir  
- Usa `tty_getch_nonblock()` (teclado no bloqueante).  
- Aplica `debounce_state()` para seguir el **nivel estable**.

Parámetros:

    POLL_MS     = 40 ms
    DEBOUNCE_MS = 50 ms

### `main_toggle.c` — Modo TOGGLE

- Teclas: `'1'` → alterna LED ON/OFF (pulso virtual 0→1→0), `'q'` → salir  
- Usa `tty_getch_nonblock()` y `debounce_press()` (flanco 0→1 estable).  

Parámetros:

    POLL_MS         = 5 ms
    DEBOUNCE_MS     = 50 ms
    PULSE_MARGIN_MS = 5 ms

---

## ⚙️ Compilación y Ejecución

    make clean && make

    ./bin/boton_switch
    # '1' encender LED, '0' apagar, 'q' salir

    ./bin/boton_toggle
    # '1' alterna LED, 'q' salir

**Requisitos:** Linux/macOS/WSL (usa `termios` y POSIX I/O).

---

## 📈 Decisiones de Diseño

- API de GPIO idéntica para simulación y hardware real.
- Determinismo en `GPIO_NOPULL` (devuelve 0 para simplicidad).
- Lectura no bloqueante para mantener el loop de polling activo.
- Restauración automática de terminal con `atexit(tty_raw_disable)`.

---

## 🛠️ Errores Corregidos Durante el Desarrollo

- `static intpin_is_valid` → `static int pin_is_valid` (typo crítico).
- Implementación faltante de `gpio_read(int)` (enlace fallaba).
- Desfase `gpio_sim_set_input` ↔ `gpio_simulate_input` (alias y unificación).
- Reemplazo de `getchar()` bloqueante por `tty_getch_nonblock()`.
- Protección de `_GNU_SOURCE` en `tty.c` para evitar redefinición.

---

## 🔄 Portar a Hardware Real

1. Crear `src/gpio_hw.c` con las **mismas firmas** de `include/gpio.h`.
2. Implementar acceso a registros del MCU (modo, pull, ODR/IDR).
3. Ajustar `include/pins.h` a pines físicos reales.
4. Eliminar `gpio_simulate_input` (no aplica en HW).
5. Compilar con el toolchain del MCU (ej. `arm-none-eabi-gcc`) y su HAL/SDK.

---

## 📄 Licencia

Código educativo para prácticas de **GPIO**, **debounce** y **I/O no bloqueante**.  
Libre uso y modificación para fines de aprendizaje.
