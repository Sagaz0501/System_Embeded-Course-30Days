#define _POSIX_C_SOURCE 200809L  // Habilita funciones POSIX como clock_gettime, nanosleep, cfmakeraw
#define _GNU_SOURCE  // Habilita funciones GNU como cfmakeraw()


/*
Simulador de GPIO (led y botón) con polling + debounce en PC
Sin necesidad de un microcontrolador (por ahora)
1 = presionado, 0 = no presionado
Cada presión del botón (después del debounce) cambia el estado del LED
*/

#include <stdio.h>      // printf, getchar
#include <stdbool.h>    // tipo bool: true/false
#include <time.h>       // clock_gettime(), struct timespec
#include <unistd.h>     // STDIN_FILENO
#include <termios.h>    // termios, tcsetattr(), cfmakeraw()
#include <fcntl.h>      // fcntl()
#include <stdint.h>     // uint32_t, int32_t
#include <stdlib.h>     // atexit()

// funcion para dormir en ms
static void sleep_ms(long ms) {
    struct timespec req;
    req.tv_sec  = ms / 1000;
    req.tv_nsec = (ms % 1000) * 1000000L;
    nanosleep(&req, NULL);
}

/* --------------------- "GPIO" SIMULADOS ----------------------------------
 * Usamos variables int para representar pines:
 * - GPIO_BUTTON: lectura (0 = suelto, 1 = presionado)
 * - GPIO_LED:    salida (0 = apagado, 1 = encendido)
 * En un MCU real serían registros mapeados en memoria; aquí basta con ints.
 */
static int GPIO_BUTTON = 0; // Simula el estado del botón (0 = suelto, 1 = presionado)
static int GPIO_LED    = 0; // Simula el estado del LED (0 = apagado, 1 = encendido)

// imprimir el esatdo del led solo si cambia
static void print_led_state(void){
    printf("LED estado: %s\n", (GPIO_LED ? "ENCENDIDO" : "APAGADO"));
}

// escribir el estado del LED normalizado a 0/1
static void led_write(int value){
    int newv = (value != 0) ? 1 : 0; // todo numero que no sea 0 o 1 se considera 1
    if (newv != GPIO_LED) {          // solo actualiza si hay cambio
        GPIO_LED = newv; // Escribe el estado del LED (0 o 1)
        print_led_state(); // Imprime el estado del LED
    }
}

/* --------------------- UTILIDADES DE TIEMPO ------------------------------
 * now_ms(): devuelve el tiempo monotónico del sistema en milisegundos.
 * - CLOCK_MONOTONIC: reloj que NO retrocede si el usuario cambia la hora del sistema.
 * - struct timespec:
 *    - ts.tv_sec  -> segundos (entero)
 *    - ts.tv_nsec -> nanosegundos (0..999,999,999)
 *   Convertimos a ms: ms = sec*1000 + ns/1e6
 */
static long long now_ms(void){
    struct timespec ts; // estructura para almacenar tiempo
    clock_gettime(CLOCK_MONOTONIC, &ts); // obtiene el tiempo actual
    return (long long)ts.tv_sec * 1000LL + ts.tv_nsec / 1000000LL;
}

/* --------------------- TECLADO NO BLOQUEANTE -----------------------------
 * Queremos que el loop de polling siga corriendo sin esperar a que el usuario
 * apriete Enter. Para eso:
 * 1) Pasamos la terminal a modo "raw" (sin buffering, sin eco, etc.) con termios.
 * 2) Ponemos stdin como "no bloqueante" usando fcntl(O_NONBLOCK).
 * Así, getchar() devuelve de inmediato: un char si hay, o EOF si no hay nada.
 */
static struct termios g_orig_termios; // Guardamos la configuración original de la terminal

// Configurar la terminal en modo "raw" para entrada de teclado + no bloqueante
static void tty_raw_enable(void){
    struct termios raw;
    tcgetattr(STDIN_FILENO, &g_orig_termios); // Obtener la configuración original
    raw = g_orig_termios; // Copiar configuración
    cfmakeraw(&raw); // Poner en modo raw
    tcsetattr(STDIN_FILENO, TCSANOW, &raw); // Aplicar modo raw

    // poner stdin en NO BLOQUEANTE
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
}

// Restaurar la configuración original de la terminal al salir
static void tty_raw_disable(void){
    tcsetattr(STDIN_FILENO, TCSANOW, &g_orig_termios);
}

/* --------------------- DEBOUNCE (MODO ESTADO) ----------------------------
 * debounce_state():
 * - raw: lectura instantánea (0/1) del "pin" del botón.
 * - stable_ms: tiempo mínimo que el nuevo estado debe mantenerse para aceptarlo.
 * Aquí no buscamos detectar flancos, sino el estado estable del botón.
 */
static int debounce_state(int raw, long long stable_ms){
    static int last_stable = 0; // último estado aceptado como real
    static int candidate   = 0; // posible nuevo estado (aún no aceptado)
    static long long t0    = 0; // momento en que empezó candidate

    if(raw != last_stable){
        if(raw != candidate){
            candidate = raw;
            t0 = now_ms();
        } else {
            if(now_ms() - t0 >= stable_ms){
                last_stable = candidate;
            }
        }
    } else {
        candidate = last_stable;
        t0 = now_ms();
    }
    return last_stable;
}

/* --------------------- PROGRAMA PRINCIPAL --------------------------------
 * POLL_MS:     cada cuántos ms muestreamos el botón (periodo de polling).
 * DEBOUNCE_MS: cuántos ms debe mantenerse el nuevo estado para aceptarlo.
 *
 * Bucle principal:
 * - Lee teclado (no bloqueante) para actualizar GPIO_BUTTON.
 * - Cada POLL_MS: llama a debounce_state(raw) y pone LED igual a ese estado.
 * - sleep_ms(1) para no consumir 100% CPU si vamos adelantados al próximo tick.
 */
int main(void){
    const int POLL_MS = 5;      // cada 5 ms muestreamos el botón
    const int DEBOUNCE_MS = 50; // el nuevo estado debe mantenerse 50ms para ser aceptado

    printf("Simulador de polling de botón + debounce.\n");
    printf("Teclas: '1' = presiona, '0' = suelta, 'q' = salir.\n\n");

    tty_raw_enable();            // teclado en modo raw
    atexit(tty_raw_disable);     // restaurar terminal al salir

    led_write(0); // LED inicia OFF
    long long next_tick = now_ms(); // programamos el primer tick

    while (1) {
        // 1) Leer teclado (no bloqueante)
        int c = getchar();
        if (c != EOF) {
            if (c == 'q' || c == 'Q') {
                printf("Saliendo...\n");
                break;
            } else if (c == '1') {
                GPIO_BUTTON = 1; // presionado
            } else if (c == '0') {
                GPIO_BUTTON = 0; // suelto
            }
        }

        // 2) Polling cada POLL_MS
        long long now = now_ms();
        if (now >= next_tick) {
            int raw = GPIO_BUTTON;
            int stable = debounce_state(raw, DEBOUNCE_MS);

            // Modo directo: estado del botón = estado del LED
            led_write(stable);

            next_tick += POLL_MS;
        }

        sleep_ms(1);
    }

    return 0;
}
