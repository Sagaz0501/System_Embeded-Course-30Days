/*
  main_toggle.c — MODO “TOGGLE” (solo tecla '1')
  Objetivo:
    - Cada vez que presionas '1', el LED alterna (ON/OFF) UNA vez.
    - No necesitas presionar '0'. Simulamos un “pulso virtual” 0->1->0 suficientemente largo
      para que pase el debounce y el polling lo vea.

  Teclado:
    '1' = genera un press virtual (mantiene 1 ms suficiente y luego suelta)
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
    const int POLL_MS         = 5;   // Periodo de muestreo
    const int DEBOUNCE_MS     = 50;  // Ventana de estabilidad requerida
    const int PULSE_MARGIN_MS = 5;   // Margen extra para asegurar detección

    tty_raw_enable();
    atexit(tty_raw_disable);

    gpio_init();
    gpio_mode(PIN_LED, GPIO_OUTPUT);
    gpio_mode(PIN_BUTTON, GPIO_INPUT);
    gpio_set_pull(PIN_BUTTON, GPIO_PULLDOWN); // por defecto 0

    // Estado “visual” para no spamear prints
    int last_led = -1;
    long long next_tick = now_ms();

    // Pulso virtual (para no necesitar tecla '0')
    int        virt_pressed    = 0;
    long long  virt_release_at = 0;

    puts("TOGGLE: '1' = alterna LED (pulso virtual). 'q' = salir.");

    while (1){
        // 1) Teclado no bloqueante
        int ch = tty_getch_nonblock();
        if (ch != EOF){
            if (ch=='q' || ch=='Q') { puts("Saliendo..."); break; }
            if (ch=='1' && !virt_pressed){
                // Generamos un press: poner 1 “suficiente” para pasar el debounce
                gpio_simulate_input(PIN_BUTTON, 1);
                virt_pressed    = 1;
                virt_release_at = now_ms() + DEBOUNCE_MS + PULSE_MARGIN_MS;
            }
        }

        long long now = now_ms();

        // 2) Polling ANTES de liberar el “pulso”
        if (now >= next_tick){
            int raw = gpio_read(PIN_BUTTON);

            // Si hay flanco 0->1 estable, alternar LED
            if (debounce_press(raw, DEBOUNCE_MS)){
                int led = gpio_read(PIN_LED);
                gpio_write(PIN_LED, !led);
            }

            // Imprimir solo al cambiar
            int led_now = gpio_read(PIN_LED);
            if (led_now != last_led){
                printf("LED: %s\n", led_now ? "ENCENDIDO" : "APAGADO");
                last_led = led_now;
            }

            next_tick += POLL_MS;
        }

        // 3) AHORA sí: liberar el pulso (poner 0) después de haber muestreado
        if (virt_pressed && now_ms() >= virt_release_at){
            gpio_simulate_input(PIN_BUTTON, 0);
            virt_pressed = 0;
        }

        sleep_ms(1);
    }
    return 0;
}
