#pragma once
/*
    pins.h - mapa de pines logicos (independiente del hardware)

    por que?
    - Pra que el codigo sea mas portable entre diferentes placas
    - Para que el codigo sea mas legible y mantenible
    - Para que el codigo sea mas facil de entender para alguien que no conoce el hardware

    -Aqui definimos identificadores logicos para que el resto del codigo no dependa
    de variables con numeros magicos.

    -Este archivo se cambiara cuando llegue el hardware, por ahora solo se simulan los pines
*/

enum{
    PIN_LED = 0, //LEd del sistema
    PIN_BUTTON = 1, //Boton del usuario
    PIN_COUNT //Numero total de pines definidos
};