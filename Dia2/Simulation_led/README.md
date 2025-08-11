# 🖥️ Simulación de LED y Botón en C (Polling + Debounce)

Este proyecto simula el funcionamiento de un botón y un LED **sin necesidad de un microcontrolador**, usando el teclado de tu PC como entrada.  
Está pensado como ejercicio de **sistemas embebidos**, implementando **polling** y **debounce**.

---

## 📚 Conceptos Clave

### 1. Polling
- **Definición:** El programa lee el estado del botón cada cierto intervalo de tiempo configurado (`POLL_MS`).
- **Comparación con interrupciones:**
  - En interrupciones, el hardware "avisa" cuando el botón cambia.
  - En polling, el software pregunta constantemente.
- **Ventajas:**
  1. Simple y fácil de entender.
- **Desventajas:**
  1. Consume CPU constantemente.
  2. Puede perder eventos si el intervalo de muestreo es demasiado grande.

---

### 2. Debounce
- Los botones mecánicos generan **rebotes eléctricos**: transiciones rápidas entre 0 y 1 al presionarlos.
- **Problema:** Sin debounce, un solo toque puede registrarse como múltiples pulsos.
- **Solución:** Exigir que el nuevo estado sea estable durante al menos `DEBOUNCE_MS` milisegundos antes de aceptarlo.

---

### 3. Flancos
- **Flanco de subida:** Cambio de `0 → 1` (presión del botón).
- **Flanco de bajada:** Cambio de `1 → 0` (soltar el botón).
- En este proyecto, la lógica reacciona en el **flanco de subida** para alternar el LED en la versión **toggle**.

---

## 🖥️ Simulación en PC
En vez de hardware real:
- **GPIO_BUTTON** → variable que representa el estado del botón (`0 = suelto`, `1 = presionado`).
- **GPIO_LED** → variable que representa el estado del LED (`0 = apagado`, `1 = encendido`).
- Teclado:
  - `1` = presionar botón.
  - `0` = soltar botón.
  - `q` = salir del programa.
- El bucle principal:
  1. Lee el teclado en modo **no bloqueante** (sin esperar Enter).
  2. Aplica **polling** cada `POLL_MS` milisegundos.
  3. Aplica **debounce** para filtrar rebotes.
  4. Cambia el estado del LED según la versión seleccionada (**switch** o **toggle**).

---

## 📂 Estructura del Proyecto

Dia2/
│
└── scr/
├── main.c # Versión "switch": el LED sigue el estado directo del botón.
├── main_toggle.c # Versión "toggle": cada pulsación alterna el estado del LED.
└── Makefile # Compila ambas versiones

yaml
Copy
Edit

---

## ⚙️ Funciones Principales

### `now_ms()`
Devuelve el tiempo en milisegundos desde que inició el reloj **monotónico** del sistema (no afectado por cambios de hora).

### `sleep_ms(ms)`
Pausa la ejecución la cantidad de milisegundos indicada usando `nanosleep()`.

### `tty_raw_enable()` / `tty_raw_disable()`
Configura la terminal en **modo raw** y sin bloqueo para leer teclas sin esperar Enter y sin detener el bucle principal.

### `debounce_press(raw, stable_ms)`
Filtra cambios rápidos en la señal del botón:
- Guarda el último estado estable (`last_stable`).
- Acepta cambios solo si se mantienen por al menos `stable_ms` milisegundos.
- Devuelve `true` en el flanco de subida (`0 → 1`).

### `led_write(value)`
Cambia el estado del LED (`0` apagado, `1` encendido) y lo imprime en pantalla.

---

## 🔄 Diferencias entre versiones

### **Switch (`main.c`)**
- El LED está encendido mientras el botón está presionado (`1`).
- Al soltar el botón (`0`), el LED se apaga.

### **Toggle (`main_toggle.c`)**
- Cada **pulsación** (flanco de subida) invierte el estado del LED.
- Funciona como un interruptor:  
  - 1ª pulsación → encendido.  
  - 2ª pulsación → apagado.  

---

## 🛠️ Compilación y Ejecución

### Compilar todo:
```bash
make
Esto generará:

nginx
Copy
Edit
boton_switch   # Versión directa (switch)
boton_toggle   # Versión con toggle
Ejecutar versión switch:
bash
Copy
Edit
./boton_switch
Ejecutar versión toggle:
bash
Copy
Edit
./boton_toggle
📋 Ejemplo de uso
nginx
Copy
Edit
Simulador de polling de botón + debounce.
Teclas: '1' = presiona, '0' = suelta, 'q' = salir.

LED estado: APAGADO
[Presiono '1'] → LED estado: ENCENDIDO
[Presiono '0'] → LED estado: APAGADO

