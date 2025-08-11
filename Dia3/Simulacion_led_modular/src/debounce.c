/*
  debounce.c — Implementaciones de debounce (estado y flanco)

  Ideas clave:
  - "raw" = lectura instantánea (cruda) del pin (0/1), puede rebotar.
  - "candidate" = posible nuevo estado que estamos "probando".
  - "last_stable" = último estado ya aceptado como REAL (sin rebotes).
  - Si "raw" se mantiene igual a "candidate" por >= stable_ms, aceptamos el cambio.
  - now_ms() = marca de tiempo en milisegundos (ver timeutil.h / timeutil.c).
*/

#include "debounce.h"
#include "timeutil.h"

/* -------------------- DETECCIÓN DE EVENTO (flanco 0->1) --------------------
   Devuelve true EXACTAMENTE UNA VEZ cuando se confirma un flanco de PRESIÓN
   (cambio estable de 0 -> 1). Si hay rebotes 0/1 rápidos, no dispara hasta que
   el nivel 1 se mantiene durante "stable_ms".

   raw:        0/1 crudo del botón.
   stable_ms:  ventana de tiempo mínima que debe sostener "candidate".
*/
bool debounce_press(int raw, long long stable_ms){
    static int last_stable = 0;   // Último estado real confirmado (0/1)
    static int candidate   = 0;   // Posible nuevo estado (todavía no confirmado)
    static long long t0    = 0;   // Marca temporal cuando vimos el "candidate"

    if (raw != last_stable) {
        // Vemos una diferencia respecto al estado REAL actual
        if (raw != candidate) {
            // Cambió el candidato: empezar a medir estabilidad desde cero
            candidate = raw;
            t0 = now_ms();
        } else {
            // El candidato se mantiene; comprobar si ya cumplió la ventana de tiempo
            if (now_ms() - t0 >= stable_ms) {
                // ¡Cambio confirmado!
                last_stable = candidate;

                // ¿Fue un flanco 0->1? Entonces es un EVENTO "press"
                if (last_stable == 1) {
                    return true;  // disparamos una sola vez
                }
            }
        }
    } else {
        // raw == last_stable -> nada cambió realmente; mantener sincronía
        candidate = last_stable;
        t0 = now_ms(); // opcional: re-referenciamos el reloj
    }

    return false; // No hubo flanco 0->1 confirmado en esta llamada
}

/* --------------------- ESTADO ESTABLE (nivel 0/1) -------------------------
   Devuelve SIEMPRE el último nivel estable (0/1). No es "evento" puntual;
   es el valor con rebotes filtrados, aceptado sólo si se sostiene "stable_ms".

   raw:        0/1 crudo del botón.
   stable_ms:  ventana mínima antes de aceptar el nuevo nivel.
*/
int debounce_state(int raw, long long stable_ms){
    static int last_stable = 0;   // Nivel aceptado (0/1)
    static int candidate   = 0;   // Posible nuevo nivel
    static long long t0    = 0;   // Marca temporal para estabilidad

    if (raw != last_stable) {
        // Hay un intento de cambio de nivel
        if (raw != candidate) {
            // Nuevo candidato: reiniciar conteo de estabilidad
            candidate = raw;
            t0 = now_ms();
        } else {
            // El candidato se mantiene; verificar tiempo
            if (now_ms() - t0 >= stable_ms) {
                // Aceptar nuevo nivel estable
                last_stable = candidate;
            }
        }
    } else {
        // No hay cambios reales; mantener sincronía
        candidate = last_stable;
        t0 = now_ms(); // opcional
    }
    return last_stable; // nivel estable actual (0/1)
}
