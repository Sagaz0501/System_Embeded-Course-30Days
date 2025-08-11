# 🖥️ Simulación de GPIO + Debounce + TTY (Modo RAW)

Proyecto didáctico para simular un **microcontrolador en PC** y practicar:

- **GPIO**
- **Debounce**
- **Lectura de teclado en modo raw / no bloqueante**

Se separa la **lógica de aplicación** de la **capa de hardware**, permitiendo portar el código a un MCU real sin modificar la capa de alto nivel.

---

## 📂 Estructura del proyecto

```
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
├─ build/
└─ bin/
```

---

## 🔌 Módulos

### 1. **GPIO (Simulado)**  
**Archivos:** `include/gpio.h` + `src/gpio_sim.c`

```c
void gpio_init(void);
void gpio_mode(int pin, gpio_mode_t mode);
void gpio_set_pull(int pin, gpio_pull_t pull);
void gpio_write(int pin, int value);
int  gpio_read(int pin);
void gpio_simulate_input(int pin, int value); // solo simulación
```

**Detalles:**
- Modela cada pin con `mode`, `pull`, `value`, `input_raw`.
- `gpio_read` devuelve el valor según modo y resistencia pull.
- Alias: `gpio_sim_set_input` → `gpio_simulate_input`.

**Fixes:**
- `intpin_is_valid` → `int pin_is_valid`
- Implementación de `gpio_read`.
- Corrección de alias `gpio_sim_set_input`.

---

### 2. **TTY Raw + No Bloqueante**  
**Archivos:** `include/tty.h` + `src/tty.c`

```c
void tty_raw_enable(void);
void tty_raw_disable(void);
int  tty_getch_nonblock(void);
```

**Detalles:**
- Guarda/restaura estado original del terminal (`termios`).
- Configura **modo raw** (`cfmakeraw`) y lectura **no bloqueante** (`O_NONBLOCK`).
- `setvbuf(stdout, NULL, _IONBF, 0)` para impresión inmediata.

---

### 3. **Debounce**  
**Archivos:** `include/debounce.h` + `src/debounce.c`

```c
int debounce_state(int raw, long long window_ms);
int debounce_press(int raw, long long window_ms);
```

---

### 4. **Utilidades de Tiempo**  
**Archivos:** `include/timeutil.h` + `src/timeutil.c`

```c
long long now_ms(void);
void sleep_ms(int ms);
```

---

### 5. **Definición de Pines**  
**Archivo:** `include/pins.h`

```c
#define PIN_LED     13
#define PIN_BUTTON  12
#define PIN_COUNT   64
```

---

## 🖥️ Programas de ejemplo

### `main_switch.c`
- `'1'` → LED ON
- `'0'` → LED OFF
- `'q'` → salir  
Usa **debounce por nivel**.

**Parámetros:**
```ini
POLL_MS     = 40 ms
DEBOUNCE_MS = 50 ms
```

---

### `main_toggle.c`
- `'1'` → alterna LED ON/OFF con pulso virtual.
- `'q'` → salir  

**Parámetros:**
```ini
POLL_MS         = 5 ms
DEBOUNCE_MS     = 50 ms
PULSE_MARGIN_MS = 5 ms
```

---

## ⚙️ Compilación y ejecución

```bash
make clean && make

./bin/boton_switch
# '1' encender LED, '0' apagar, 'q' salir

./bin/boton_toggle
# '1' alterna LED, 'q' salir
```

**Requisitos:** Linux, macOS o WSL (usa `termios` y POSIX I/O).

---

## 💡 Decisiones de diseño
- API de GPIO idéntica para simulación y hardware real.
- Determinismo en `NOPULL` (devuelve `0` en vez de valor aleatorio).
- Uso de TTY en modo raw + lectura no bloqueante.
- Restauración automática de la terminal con `atexit(tty_raw_disable)`.

---

## 🛠️ Errores corregidos
- `intpin_is_valid` → `int pin_is_valid`
- Implementación faltante de `gpio_read`
- Alias `gpio_sim_set_input` corregido
- Sustitución de `getchar()` bloqueante por `tty_getch_nonblock()`
- Protección `_GNU_SOURCE` para evitar redefinición

---

## 🔄 Portar a hardware real
1. Crear `gpio_hw.c` con mismas firmas que `gpio.h`.
2. Implementar acceso a registros reales del MCU.
3. Ajustar `pins.h` a pines reales.
4. Eliminar `gpio_simulate_input`.
5. Compilar con toolchain del MCU.

---

## 🧪 Pruebas recomendadas
- Cambiar `GPIO_PULLUP / GPIO_PULLDOWN` y observar.
- Variar `DEBOUNCE_MS` y `POLL_MS` para medir tolerancia.
- Probar pulsos cortos en `toggle` y ver si pasan el debounce.

---

## 📝 Troubleshooting
- **No responde teclas:** verifica `tty_getch_nonblock` y `tty_raw_enable`.
- **LED no cambia:** revisa `pull` y valores simulados.
- **Errores linker:** confirmar coincidencia de firmas con `gpio.h` y recompilar.

---

## 🚀 Roadmap
- Simulación de ruido en pines sin pull.
- Soporte de interrupciones simuladas.
- Port multiplataforma (WinAPI).
- Dashboard visual para LED/botón.
