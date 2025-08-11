#pragma once

/*
    debounce.h - filtros de rebote por software

    hay dos enfoques:
    - debounce_state() - devuelve el nivel esatble 1/0 (se usa en main_switch)
    - debounce_press() - devuelve 1 si se detecta un cambio de 0 a 1 (se usa en main_toggle)

*/

#include <stdbool.h>

/*
    Detecta flanco de Presion (0 -> 1) tras mantenerse estable stable_ms
    devuelve true solo caudno se confirma el flanco de presion
*/
bool debounce_press(int pin, long long stable_ms);

/*
    Devuelve el esatdo esatble (0/1) tras mantenerse esatble por stable_ms
    util cuando la salida debe seguir el nivel (switch 1=ON, 0=OFF)
*/
int debounce_state(int pin, long long stable_ms);