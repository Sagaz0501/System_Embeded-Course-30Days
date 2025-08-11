# Día 2 – GPIO (Salida y Entrada)

## Conceptos a ver:
1. Entender GPIO en modo salida y entrada (LED y botón).
2. Hacer polling (consulta) del botón.
3. Implementar debounce por software.
4. Compilar con `gcc` y ejecutar.

---

## Entender GPIO

Un **GPIO** (*General-Purpose Input/Output*) en modo salida es un pin del microcontrolador que puede forzar un nivel **alto** (`1`) o **bajo** (`0`) para mandar voltaje/corriente a un componente electrónico (LED, transmisor, buzzer, etc.).

### Alto (1) y Bajo (0)
- La mayoría de MCUs modernas usan **3.3V** como Vdd.
- **Alto (1)** = Vdd (3.3V). Ej.: el pin envía corriente (activo-alto).
- **Bajo (0)** = 0V (GND).
- ⚠️ No todos los pines soportan el mismo voltaje, revisa el datasheet antes de conectar.

---

### Modos eléctricos de salida
1. **Push-Pull** (el común):
   - Dos transistores internos: uno conecta el pin a Vdd (sourcing) y el otro a GND (sinking).
   - Sirve para manejar directamente LEDs, buzzers, etc.
2. **Open-Drain / Open-Collector**:
   - El pin solo tira a GND; para tener “alto” necesitas resistencia *pull-up* externa a Vdd.
   - Útil para líneas compartidas (I²C) o interfazar con voltajes distintos.

> Para LEDs y pruebas básicas se usa **push-pull**.

---

### Source vs Sink
- **Source (suministrar):** pin en `1` → entrega corriente al LED hacia GND.
- **Sink (absorber):** pin en `0` → la corriente entra por el LED desde Vdd y el pin la absorbe a GND.
- Muchas MCUs **sinkean** más corriente de la que **sourcean**. Revisa el datasheet (típico: 6–20 mA por pin).

---

### LED con resistencia (indispensable)
Siempre usa una resistencia en serie para limitar la corriente.

**Fórmula:**
\[
R = \frac{V_{out} - V_f}{I}
\]

- \(V_{out}\) = Voltaje de salida del GPIO (3.3 V o 5 V).
- \(V_f\) = Voltaje de caída del LED (depende del color).
- \(I\) = Corriente deseada (en amperios).

**Ejemplo (MCU 3.3 V, LED rojo \(V_f \approx 2.0\) V, \(I=10\) mA):**
\[
R = \frac{3.3 - 2.0}{0.01} = 130\ \Omega
\]
Usa **150 Ω comercial** para seguridad.

**Para LED verde moderno** \(V_f \approx 2.2–2.4\) V → con 3.3 V y 10 mA, **100–120 Ω**.

**Regla de oro:** si no estás seguro, usa **220–330 Ω** (menos brillo pero más seguro).

---

### Cableados típicos

**Active-high (source):**
Pin → Resistencia → LED → GND
(Pin = 1 enciende)

markdown
Copy
Edit

**Active-low (sink):**
Vdd → Resistencia → LED → Pin
(Pin = 0 enciende)

yaml
Copy
Edit

> Define en el código si el LED es activo-alto o activo-bajo para evitar confusiones.

---

### Pasos para configurar un pin como salida (MCU genérico)
1. Habilitar clock del puerto (algunas familias lo requieren).
2. Configurar multiplexado para usarlo como GPIO.
3. Dirección: salida (`output`).
4. Tipo: push-pull (por defecto) u open-drain si lo necesitas.
5. Pull-ups/pull-downs internos: desactivados en salida.
6. Velocidad: baja o media para LEDs.
7. Estado inicial seguro: apagado.

---

## Ejemplos de código

### Genérico (macros)
```c
#define LED_ON()   (GPIO_ODR |=  (1u << LED_PIN))   // activo-alto
#define LED_OFF()  (GPIO_ODR &= ~(1u << LED_PIN))
#define LED_TOG()  (GPIO_ODR ^=  (1u << LED_PIN))

int main(void) {
    gpio_clock_enable(GPIOX);
    gpio_set_mode_output(GPIOX, LED_PIN, PUSH_PULL, LOW_SPEED);
    LED_OFF();
    while (1) {
        LED_TOG();
        delay_ms(500);
    }
}
AVR (ATmega328P / Arduino)
c
Copy
Edit
// LED en PB5 (pin 13 Arduino Uno)
DDRB  |= (1 << DDB5);   // salida
PORTB &= ~(1 << PORTB5); // LED OFF (activo-alto)

while (1) {
    PORTB ^= (1 << PORTB5);
    _delay_ms(500);
}
STM32 (sin librerías)
c
Copy
Edit
RCC->AHB2ENR |= RCC_AHB2ENR_GPIOAEN;         
GPIOA->MODER &= ~(3u << (5*2));               
GPIOA->MODER |=  (1u << (5*2));               
GPIOA->OTYPER &= ~(1u << 5);                  
GPIOA->OSPEEDR &= ~(3u << (5*2));             
GPIOA->PUPDR &= ~(3u << (5*2));               

while (1) {
    GPIOA->ODR ^= (1u << 5);
    delay_ms(500);
}
En PC, sin MCU, simula el GPIO con variables y printf.

Buenas prácticas
Leer el datasheet para conocer límites de corriente.

Estado inicial seguro (LED apagado).

No usar delays largos en sistemas reales; usar timers o RTOS.

Para cargas grandes: transistor/MOSFET + diodo flyback (si es inductivo).

Evitar EMI: baja velocidad y cables cortos.

Errores comunes
No usar resistencia → pin en corto.

LED invertido → no enciende.

Configurar como entrada por error.

Confundir active-bajo con “al revés”.

Exceder la corriente máxima.

Mini-tareas
Probar LED en active-alto y active-bajo.

Cambiar resistencia (220 Ω vs 330 Ω) y observar.

Implementar LED_ON(), LED_OFF(), LED_TOG() con macros.

(Opcional) Medir voltaje en el pin cuando está en 1 y 0. 
​
 



