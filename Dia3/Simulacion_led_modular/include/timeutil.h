#pragma once

/*
    timeutil.h - Utilidades de tiempo para temporizadores no bloquiantes
*/

long long now_ms(void); // Devuelve el tiempo actual en milisegundos desde el inicio del programa

void sleep_ms(long long ms); // Suspende la ejecución durante el número de milisegundos especificado
