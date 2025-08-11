#pragma once

/*
    GPIO.h - API minima de GPIO con interfax tipo FW

    objetvo:
    - El resto del copdigo(main_switch, main_toggle, debounce) usa solo esats funciones
    - para pasar el HW real: implementar lkas mismas fucniones en gpio_hw.c en lugar de gpio_sim.c
    - para simular el HW: implementar las funciones en gpio_sim.c
*/

#include <stdint.h>

typedef enum{
    GPIO_INPUT = 0, //configurar pin como entrada
    GPIO_OUTPUT = 1 //configurar pin como salida
} gpio_mode_t; //modo de un pin

typedef enum{
    GPIO_NOPULL = 0, //sin pull-up ni pull-down entrada puede ser flotante
    GPIO_PULLUP = 1, //resistencia interna a VCC (1 por defecto)
    GPIO_PULLDOWN = 2 //resistencia interna a GND (0 por defecto)
} gpio_pull_t; //configuracion de pull-up/pull-down

/*Inicializa la "Capa GPIO"
    En HW real: habilitar los clocks en los puertos, yponer los pines en un estado seguro
    En simulacion: inicializar las variables internas

*/
void gpio_init(void);

//Configura el modo del pin (entrada/salida)
void gpio_mode(int pin, gpio_mode_t mode);

//Configura resistencias internas(solo input)
void gpio_set_pull(int pin, gpio_pull_t pull);

//Escribe un valor en el pin (solo output)
void gpio_write(int pin, int value); //value: 0 o 1

//Lee el valor del pin (solo input)
int gpio_read(int pin); //retorna 0 o 1

/* ==========SOLO en simulacion==============
 *alimanta la "entrada cruda" (como si viniera del mundo fisico/teclado)
 *En HW real no se usa
*/

void gpio_simulate_input(int pin, int value); //value: 0 o 1

/* ========================================= */