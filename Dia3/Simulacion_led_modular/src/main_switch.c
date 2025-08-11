/*
  main_switch.c — MODO “SWITCH”
  Comportamiento:
    - Botón 1 => LED ON (1)
    - Botón 0 => LED OFF (0)
  Con debounce por nivel: el LED solo cambia cuando el nivel es estable.
  Teclado:
    '1' = presiona (pone 1 crudo)
    '0' = suelta  (pone 0 crudo)
    'q' = salir
*/

#include <stdio.h>
#include <stdlib.h>
#include "pins.h"
#include "gpio.h"
#include "debounce.h"
#include "timeutil.h"
#include "tty.h"

int main(void){
    const int POLL_MS = 40; // 40ms para el polling
    const long long DEBOUNCE_MS = 50; // 50ms para el debounce
    
    //1. Hanilitamos el esatdo raw de la terminal
    tty_raw_enable();
    //2. y volvemos a dejarlo en modo noraml al salir
    atexit(tty_raw_disable);

    //3. Inicializamos la capa GPIO
    gpio_init();
    //4. Configuramos el LED como salida
    gpio_mode(PIN_LED, GPIO_OUTPUT);
    //5. Configuramos el botón como entrada con pull-up
    gpio_mode(PIN_BUTTON, GPIO_INPUT);
    //6. Configuramos la resistencia pull-up del botón
    gpio_set_pull(PIN_BUTTON, GPIO_PULLDOWN); // Resistencia interna a GND (0 por defecto)

    int last_led =0; //cache para imprimir solo cuando cambie
    long long next_tick = now_ms(); // Programamos el primer tick

    puts("SWITCH MODE");
    puts("Presiona '1' para encender el LED, '0' para apagarlo.");
    puts("Presiona 'q' para salir.");

    //Bucle primcipal
    while(1){
        //t. teclado no bloqueante
        int c = tty_getch_nonblock();
        if(c != EOF){
            if(c == 'q'){
                puts("Saliendo...");
                break; // salir del bucle
            } else if(c == '1'){
                gpio_simulate_input(PIN_BUTTON, 1); // Simula botón presionado
            } else if(c == '0'){
                gpio_simulate_input(PIN_BUTTON, 0); // Simula botón soltado
            }
            
        }

        //7. Polling del botón cada POLL_MS ms
        if(now_ms() >= next_tick){
            //leer el esstado curdo dedl boton|
            int raw = gpio_read(PIN_BUTTON);
            
            //8. Aplicar debounce al estado crudo
            int stable = debounce_state(raw, DEBOUNCE_MS); // Aplicar debounce

            //9. LED sigue el esatdo esatble del botón
            gpio_write(PIN_LED, stable); // Escribir el estado estable al LED

            //10. Imprimir el estado del LED solo si ha cambiado
            if(stable != last_led){
                last_led = stable; // Actualizar el cache
                printf("LED: %s\n", stable ? "ON" : "OFF"); // Imprimir estado del LED
            }

            //11. Programar el siguiente tick
            next_tick += POLL_MS; // Incrementar el tiempo del próximo tick

        }

        //12. Dormir hasta el próximo tick
        sleep_ms(1); // Dormir para no consumir 100% CPU
    }
    return 0; // Salir del programa
}