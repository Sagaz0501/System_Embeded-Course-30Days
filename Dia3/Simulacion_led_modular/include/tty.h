#pragma once

/*
    tty.h -modo raw de terminal, stdin no bloqueante

    esto permite:
    - leer caracteres de la terminal sin necesidad de puldar enter
    - simular un boton o switch en la terminal
    - usar la terminal como entrada de hardware para simular eventos
*/

void tty_raw_enable(void); // Habilita el modo raw de la terminal
void tty_raw_disable(void); // Deshabilita el modo raw de la terminal al salir del programa
int  tty_getch_nonblock(void); // -1 si no hay tecla