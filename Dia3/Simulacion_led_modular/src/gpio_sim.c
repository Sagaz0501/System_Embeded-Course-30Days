/*
    gpio_sim.c — IMPLEMENTACIÓN **SIMULADA** DE LA CAPA GPIO

    ¿QUÉ ES ESTO?
    -------------
    - En un microcontrolador real, GPIO se maneja escribiendo/leyendo REGISTROS
      (memoria mapeada) de los puertos de E/S (p. ej., DDRx, PORTx, PINx en AVR,
      o MODER/ODR/IDR en STM32).
    - Como NO tenemos micro ahora, simulamos ese hardware con una ESTRUCTURA en RAM.
      El resto del "firmware" (main_switch.c / main_toggle.c / debounce.c) llama
      a esta capa como si fuera un driver real.

    ¿CÓMO SE PORTA LUEGO A HW REAL?
    -------------------------------
    - Mantienes la interfaz (gpio_init, gpio_mode, gpio_set_pull, gpio_write, gpio_read).
    - Creas otro archivo, por ejemplo gpio_hw.c, y ahí tocas los registros reales.
    - El resto del código NO CAMBIA. Esa es la gracia de la abstracción.
*/


#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "gpio.h"
#include "pins.h"

/*==========================================================
=           REPRESENTACIÓN INTERNA (SIMULADA)              =
==========================================================*/

/*
   Cada pin "físico" del micro lo modelamos como una "ranura" (slot)
   con los datos mínimos que nos interesan para simular GPIO:

   - mode: si el pin está configurado como entrada o salida (GPIO_INPUT/OUTPUT).
   - pull: qué resistencia interna tiene cuando está en modo INPUT
           (GPIO_NOPULL / GPIO_PULLUP / GPIO_PULLDOWN).
   - value: último valor "efectivo"
       * Si es OUTPUT: lo que escribimos con gpio_write (0/1).
       * Si es INPUT : NO lo tocamos aquí; lo leemos combinando input_raw + pull.
   - input_raw: la "fuente cruda" para entradas, que viene del "mundo externo".
       * En simulación lo cambiamos desde teclado con gpio_sim_set_input().
       * En hardware real, ese "crudo" viene del pin físico (IDR, PINx, etc.).
*/

typedef struct{
    gpio_mode_t mode; //GPIO_INPUT o GPIO_OUTPUT
    gpio_pull_t pull; //GPIO_NOPULL, GPIO_PULLUP, GPIO_PULLDOWN
    int value; //Ultimo valor efectivo escrito (output) o leido (input)
    int input_raw; //valor "crudo" de la entrada (antes de debounce)
} gpio_slot_t;

/*
   Arreglo con un slot por pin. La cantidad de pines viene de pins.h (PIN_COUNT).
   IMPORTANTE: en una placa real, PIN_COUNT y los IDs (PIN_LED, PIN_BUTTON) los
   defines tú según tu hardware.
*/

static gpio_slot_t  g[PIN_COUNT]; //array de pines g[PIN_COUNT] hace referencia a los pines logicos definidos en pins.h

/*==========================================================
=                 FUNCIONES AUXILIARES (privadas)          =
==========================================================*/

/* Validación simple de rango para evitar accesos fuera del arreglo.
   En C el acceso fuera de rango es UB (undefined behavior), así que chequeamos. */

static int pin_is_valid(int pin){
    return (pin >= 0 && pin < PIN_COUNT);
}

/*==========================================================
=                    API PÚBLICA (SIMULADA)                =
==========================================================*/

/*
   gpio_init()
   -----------
   Pone todos los pines en estado seguro por defecto:
   - Modo:    INPUT
   - Pull:    NOPULL
   - value:   0
   - raw:     0

   En HW REAL:
   - Aquí habilitarías los clocks de los GPIO (RCC en STM32, por ej.),
     pondrías estados iniciales seguros, etc.
*/

void gpio_init(void){
    //limpiamos toda la estructura a 0
    memset(g, 0, sizeof(g));
    for (int i =0; i < PIN_COUNT; i++){
        g[i].mode = GPIO_INPUT; //por defecto, todos los pines son entradas
        g[i].pull = GPIO_NOPULL; //sin pull-up ni pull-down
        g[i].value = 0; //valor inicial 0
        g[i].input_raw = 0; //entrada cruda inicializada a 0
    }
}

/*
   gpio_mode(int pin, gpio_mode_t mode)
   ------------------------------------
   Configura el modo de un pin (entrada/salida).
   En HW REAL: aquí configurarías los registros de modo (DDR, MODER, etc.).

   Si el pin no es válido, no hace nada.
*/
void gpio_mode(int pin, gpio_mode_t mode){
    /*
         Validamos el pin antes de configurarlo.
         Si no es válido, mostramos un error y salimos sin hacer nada.
    */
    if(!pin_is_valid(pin)){
        fprintf(stderr, "gpio_mode: Pin %d no es válido.\n", pin);
        return;
    }
    g[pin].mode = mode; //configuramos el modo del pin
}

/*
   gpio_set_pull(pin, pull)
   ------------------------
   Define la resistencia interna cuando el pin está en INPUT.
   - GPIO_NOPULL   : el pin puede "flotar" si no hay nada conectado
   - GPIO_PULLUP   : el pin tenderá a 1 (HIGH) si está suelto
   - GPIO_PULLDOWN : el pin tenderá a 0 (LOW) si está suelto

   En HW REAL:
   - Escribirías en registros PUPDR (STM32) u opciones equivalentes.
*/
void gpio_set_pull(int pin, gpio_pull_t pull){
    //Validamos el pin antes de onfigurarlo.
    if(!pin_is_valid(pin)){
        fprintf(stderr, "gpio_set_pull: Pin %d no es válido.\n", pin);
        return;
    }
    if (g[pin].mode != GPIO_INPUT) {
        fprintf(stderr, "gpio_set_pull: Pin %d no está configurado como entrada.\n", pin);
        return;
    }
    g[pin].pull = pull; //configuramos la resistencia interna del pin
}

/*
   gpio_write(pin, value)
   ----------------------
   Escribe un 0 o 1 en un pin CONFIGURADO COMO SALIDA.

   - Si el pin NO es OUTPUT, ignoramos (en HW podrías forzar/avisar error).
   - Normalizamos "value" a 0/1 (todo diferente de 0 cuenta como 1).
   - Guardamos en g[pin].value como "cache" del estado de salida.

   En HW REAL:
   - Escribirías el bit correspondiente en el registro ODR/PORTx.
*/
void gpio_write(int pin, int value){
    //Validamos el pin antes de escribir en él.
    if(!pin_is_valid(pin)){
        fprintf(stderr, "gpio_write: Pin %d no es válido.\n", pin);
        return;
    }
    if (g[pin].mode != GPIO_OUTPUT) {
        fprintf(stderr, "gpio_write: Pin %d no está configurado como salida.\n", pin);
        return;
    }
    g[pin].value = (value != 0) ? 1 : 0; //normalizamos value a 0 o 1
}

/*
   gpio_read(pin)
   --------------
   Devuelve el valor lógico "observado" en el pin (0/1).
   - Si el pin es OUTPUT: retornamos el "cache" (lo que escribimos).
     (En HW podrías leer ODR o IDR, dependiendo de la familia.)
   - Si el pin es INPUT :
       -> combinamos la "fuente cruda" (input_raw) con el pull interno.

   LÓGICA DE SIMULACIÓN PARA INPUT:
   - Si input_raw == 1   -> retorna 1 (hay señal externa)
   - Si input_raw == 0   -> depende del pull:
        * PULLUP     -> 1  (tira hacia 1 por defecto)
        * PULLDOWN   -> 0  (tira hacia 0 por defecto)
        * NOPULL     -> 0  (para evitar "ruido"; podríamos randomizar, pero no ayuda a aprender)
*/

int gpio_read(int pin){
    //Validamos que el pin sea válido
    if(!pin_is_valid(pin)){
        fprintf(stderr, "gpio_read: Pin %d no es válido.\n", pin);
        return 0; //retornamos 0 por defecto
    }

    if(g[pin].mode == GPIO_OUTPUT){
        //Si es salida, retornamos el valor cacheado
        return g[pin].value;
    } 
    else if (g[pin].mode == GPIO_INPUT) {
        //Si es entrada, combinamos input_raw con pull
        if (g[pin].input_raw) {
            return 1; //input_raw es 1, retornamos 1
        } else {
            //input_raw es 0, depende del pull
            switch (g[pin].pull) {
                case GPIO_PULLUP:   return 1; //pull-up: tira a 1
                case GPIO_PULLDOWN: return 0; //pull-down: tira a 0
                case GPIO_NOPULL:   return 0; //sin pull: retornamos 0 por defecto
            }
        }
    }

    fprintf(stderr, "gpio_read: Pin %d no está configurado como entrada o salida.\n", pin);
    return 0; //retornamos 0 por defecto si no es ni entrada ni salida
}



/*
   gpio_sim_set_input(pin, value)
   ------------------------------
   *** SOLO PARA SIMULACIÓN EN PC ***

   "Inyecta" una señal externa cruda al pin (como si fuera el mundo físico).
   - value != 0 -> fuerza "1 crudo"
   - value == 0 -> "0 crudo"

   Esto lo usamos desde los "mains" para mapear el teclado:
     '1' -> gpio_sim_set_input(PIN_BUTTON, 1)
     '0' -> gpio_sim_set_input(PIN_BUTTON, 0)

   En HW REAL:
   - NO EXISTE. El "crudo" viene del propio pin leyendo el registro IDR/PINx.
*/
void gpio_simulate_input(int pin, int value) {
    if (!pin_is_valid(pin)) {
        // printf("[GPIO] gpio_sim_set_input: pin inválido %d\n", pin);
        return;
    }
    g[pin].input_raw = (value != 0) ? 1 : 0;
}

/*==========================================================
=                  NOTAS Y CONSEJOS PRÁCTICOS              =
==========================================================

1) ¿Por qué devolver 0 en NOPULL sin input_raw?
   - En la realidad, un pin sin pull puede "flotar" y leer valores aleatorios.
   - Para aprender es mejor un comportamiento determinista (0) que no confunda.

2) ¿Por qué no imprimir errores todo el tiempo?
   - Puedes descomentar los printf de error si te ayuda a depurar.
   - En firmware real quizá usarías asserts, logs por UART, o códigos de retorno.

3) Portar a HW real (cuando tengas MCU):
   - Crea gpio_hw.c y copia las firmas (mismos nombres/prototipos).
   - Implementa:
        * gpio_init: habilitar clock de puertos, estados seguros.
        * gpio_mode: configurar registro de dirección/modo.
        * gpio_set_pull: configurar PUPDR o equivalente.
        * gpio_write: escribir en ODR/PORTx.
        * gpio_read: leer de IDR/PINx.
   - Borra/omite gpio_sim_set_input (no aplica en HW).
   - Ajusta pins.h para mapear PIN_LED/PIN_BUTTON a pines reales.

4) ¿Por qué tener esta capa?
   - Te permite escribir tu "lógica de aplicación" una vez (debounce, toggle, etc.)
     y luego correrla:
        * Hoy: en PC (sim).
        * Mañana: en tu micro, sin reescribir todo.

*/