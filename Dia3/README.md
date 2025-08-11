# 📌 Día 3  
#GPIO y Modos de Operación

## 1️⃣ ¿Qué es GPIO?
GPIO = **General Purpose Input/Output**  
Son pines de propósito general en un microcontrolador que pueden configurarse como:
- **Entrada** (leer señales externas).
- **Salida** (enviar señales).

---

## 2️⃣ Modos principales de un GPIO

### 🟢 Entrada (INPUT)
- El pin recibe señales (0 o 1 lógico).
- No genera voltaje por sí mismo.
- Útil para leer botones, sensores, etc.

### 🔴 Salida (OUTPUT)
- El pin envía señales (0 o 1 lógico).
- Puede encender un LED, activar un relé, mover un motor, etc.

---

## 3️⃣ Entrada con Pull-Up
- **Pull-up interno:** el pin está normalmente en `HIGH` gracias a una resistencia interna (ej. 10kΩ) conectada a Vcc.
- Cuando conectas el botón a GND → el pin pasa a `LOW`.
- **Ventaja:** no necesitas resistencia externa.

---

## 4️⃣ Entrada con Pull-Down
- **Pull-down interno:** el pin está normalmente en `LOW` gracias a una resistencia interna conectada a GND.
- Cuando conectas el botón a Vcc → el pin pasa a `HIGH`.
- Menos común que el pull-up, pero útil en algunos circuitos.

---

## 5️⃣ Qué es un "pin" en código
En un microcontrolador real:
- El número de pin es un índice que corresponde a un **registro** del micro.
- Cada pin puede tener varias funciones (GPIO, UART, PWM, etc.) y hay que configurarlo antes de usarlo.

En PC (simulación):
- El pin será simplemente un **índice en un array** o una **variable global**.

---
## 6️⃣ Funciones típicas
```c
gpio_mode(pin, mode);      // Configura el modo (INPUT, OUTPUT, etc.)
gpio_write(pin, value);    // Escribe 0 o 1 en un pin (si es salida)
int val = gpio_read(pin);  // Lee 0 o 1 de un pin (si es entrada)
```
---
##7 Ejemplos
```c
//lectura(entrada)
if(GPIO_READ(READ_SESNOR)){
    //Hay senal en el sensor
}

//Escritura(Salida)
GPIO_WRITE(PIN_MOTOR, 1); //Enciende el motor
```
---
##8 Buenas practicas
- Siempre configurar el modo antes de usar el pin
- Evitar dejar un Pin de entrada flotante
- Usar nombres descriptivos(PIN_LED_ROJO) en vez de 3
- Documentar si el pin esat invertido (LOW = encendido)

---
#Recursos utiles

1. [What is GPIO?](https://www.youtube.com/watch?v=QxvdmzKxEeg)
2. [Arduino Docuemntation](https://docs.arduino.cc/learn/microcontrollers/digital-pins/)


---
#Temporizacion
---
##1 Temporizacion basica
- En C (PC o Micro) podemos usar:
```c
usleep(1000);  //milisegundos
sleep(1);      //segundos
nanosleep(...) //nanosegundos
```
- o en micros:
1. delay(ms) -> Arduino
2. HAL_Delay(ms) -> STM32

- Simple
- Bloquea el CPU, no se pueden hacer otras tareas mientras tanto.

---
##2 Problemas de los temporizadores bloquiantes.
- Como ya se menciono un temporizador bloquiante evita que se haga otra tarea mientras este en el temporizador.

e.j. 
```c
led_on();
sleep(5); //5 segundos
led_off();
```
- Minetras el CPU esta en Sleep(5), no lee botones, y no procesa comunicaciones, no hace nada mas
- por eso en FW profesionales se usan *Temporizadores no bloquiantes*

---
##3 Temporizadores no bloquiantes
En lugar de pausar el programa, se mide el tie,po actualy se decide si ha pasado el tiempo esperado.
- como lo que hicimos en [Dia2 C](https://github.com/Sagaz0501/System_Embeded-Course-30Days/blob/main/Dia2/Simulation_led/scr/main.c)

Ejemplo usando now_ms():
```c
long long t0 = now_ms();
long long interval = 500; //500ms

while(1){
    if(now_ms() - t0 >= interval){
        toggle_led();
        t0 = now_ms(); //se reinicia el conteo
    }
}
```
## ⏱ Cómo funciona `now_ms()` y la temporización

- `now_ms()` devuelve el tiempo actual en **milisegundos** desde que arrancó el sistema/programa.  
- No reinicia ni pausa el reloj, solo toma la “marca de tiempo” del momento.  
- Guardas esa marca en una variable (`t0`) y luego comparas:  

```c
if (now_ms() - t0 >= intervalo) {
    // Acción a ejecutar
    t0 = now_ms(); // Reinicia el conteo
}
```
La resta indica cuántos ms han pasado desde t0.

Si supera el intervalo, haces la acción y reinicias t0 con now_ms() para volver a contar.

📌 Esto permite temporizar sin bloquear el programa (no usar delay() o sleep() largos).

---
##4 Multiples tareas en paralelo
```c
long long t_led = now_ms();
long long t_sensor = now_ms();

while (1) {
    if (now_ms() - t_led >= 500) {
        toggle_led();
        t_led = now_ms();
    }
    if (now_ms() - t_sensor >= 100) {
        leer_sensor();
        t_sensor = now_ms();
    }
}
```
---
#Sumulacion
/*se hara una simulacion*/ de un led modular 
[Simulacion_led_modular](github.com/Sagaz0501/System_Embeded_Course-30Days/tree/main/Dia3/Simulacion_led_modular)
