#define _POSIX_C_SOURCE 200809L
/*
  timeutil.c — Tiempo monotónico + sleep cooperativo

  now_ms():
    - Usa CLOCK_MONOTONIC (no retrocede si cambia la hora del SO).
    - Retorna milisegundos desde que arrancó el reloj.

  sleep_ms():
    - Envuelve nanosleep para ms.
    - Útil para que el loop no consuma 100% CPU en la simulación.
*/

#include <time.h>

long long now_ms(void){
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (long long)ts.tv_sec * 1000LL + ts.tv_nsec / 1000000LL;
}

void sleep_ms(long ms){
    struct timespec rq = { .tv_sec = ms/1000, .tv_nsec = (ms%1000)*1000000L };
    nanosleep(&rq, NULL);
}
