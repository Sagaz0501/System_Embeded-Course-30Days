#define _POSIX_C_SOURCE 200809L   // clock_gettime, nanosleep (cfmakeraw: define -D_GNU_SOURCE al compilar)
#define _GNU_SOURCE // cfmakeraw() requiere este define


#include <stdio.h>      // printf, getchar
#include <stdbool.h>    // bool
#include <time.h>       // clock_gettime(), nanosleep(), struct timespec
#include <unistd.h>     // STDIN_FILENO
#include <termios.h>    // termios, tcgetattr(), tcsetattr(), cfmakeraw()
#include <fcntl.h>      // fcntl()
#include <stdint.h>     // uint32_t, int32_t
#include <stdlib.h>     // atexit()

/* ----------------- Parámetros de timing del “firmware” ------------------ */
#define POLL_MS          5     // cada cuántos ms muestreamos el “pin” (periodo de polling)
#define DEBOUNCE_MS     50     // ms que debe mantenerse el cambio para aceptarlo
#define PULSE_MARGIN_MS  5     // MARGEN extra al mantener “1” (garantiza que el polling lo vea)

/* ---------------------------------------------------------------------------
   TIPOS DE DATOS USADOS:
   - int: 0/1 para GPIO simulado.
   - bool: true/false.
   - long long: milisegundos acumulados (64 bits).
   - struct timespec: { tv_sec, tv_nsec } para temporización fina.
--------------------------------------------------------------------------- */

/* --------------------- "GPIO" SIMULADOS ---------------------------------- */
static int GPIO_BUTTON = 0; // 0 = suelto, 1 = presionado (crudo)
static int GPIO_LED    = 0; // 0 = apagado, 1 = encendido

// imprimir el estado del LED (solo si cambia)
static void print_led_state(void){
    printf("LED estado: %s\n", (GPIO_LED ? "ENCENDIDO" : "APAGADO"));
}

// escribir el estado del LED normalizado a 0/1, solo si hay cambio
static void led_write(int value){
    int newv = (value != 0) ? 1 : 0; // todo número !=0 se considera 1
    if (newv != GPIO_LED) {
        GPIO_LED = newv;
        print_led_state();
    }
}

/* --------------------- UTILIDADES DE TIEMPO ------------------------------ */
static long long now_ms(void){
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts); // reloj monotónico (no retrocede)
    return (long long)ts.tv_sec * 1000LL + ts.tv_nsec / 1000000LL;
}

static void sleep_ms(long ms){
    struct timespec req;
    req.tv_sec  = ms / 1000;
    req.tv_nsec = (ms % 1000) * 1000000L;
    nanosleep(&req, NULL);
}

/* --------------------- TECLADO NO BLOQUEANTE ----------------------------- */
static struct termios g_orig_termios; // guardamos configuración original

// Poner terminal en modo raw + no bloqueante (getchar() no espera Enter)
static void tty_raw_enable(void){
    struct termios raw;
    tcgetattr(STDIN_FILENO, &g_orig_termios);
    raw = g_orig_termios;
    cfmakeraw(&raw);                          // <-- puede requerir -D_GNU_SOURCE al compilar
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);

    // stdin no bloqueante
    int flags = fcntl(STDIN_FILENO, F_GETFL, 0);
    fcntl(STDIN_FILENO, F_SETFL, flags | O_NONBLOCK);
}

// Restaurar terminal
static void tty_raw_disable(void){
    tcsetattr(STDIN_FILENO, TCSANOW, &g_orig_termios);
}

/* ------------------- DEBOUNCE: flanco de PRESIÓN (0->1) ------------------ */
static bool debounce_press(int raw, long long stable_ms){
    static int last_stable = 0; // último estado aceptado (0/1)
    static int candidate   = 0; // posible nuevo estado
    static long long t0    = 0; // marca de inicio de estabilidad

    if (raw != last_stable){
        if (raw != candidate){
            candidate = raw;      // nuevo candidato
            t0 = now_ms();        // reinicia ventana de estabilidad
        } else {
            if (now_ms() - t0 >= stable_ms){
                last_stable = candidate;     // aceptamos cambio
                if (last_stable == 1) {      // flanco 0->1 (presión)
                    return true;
                }
            }
        }
    } else {
        // seguimos estables (mantener sincronía, opcional)
        candidate = last_stable;
        t0 = now_ms();
    }
    return false;
}

/* ------------------ PULSO VIRTUAL controlado por tecla '1' --------------- 
 * Queremos que CADA '1' sea un press real (0->1->0) SIN que el usuario mande '0'.
 * - virt_pressed: indica que hay un pulso “alto” en curso.
 * - virt_release_at: cuándo bajamos a 0 automáticamente.
 * NOTA CRÍTICA: mantenemos el “1” un poco MÁS que DEBOUNCE_MS (margen),
 * y además hacemos el POLLING ANTES de liberar, para NO perder el flanco.
 */
static int        virt_pressed    = 0;
static long long  virt_release_at = 0;

/* --------------------------- PROGRAMA PRINCIPAL -------------------------- */
int main(void){
    printf("TOGGLE con pulso virtual: presiona '1' para alternar LED, 'q' para salir.\n");
    printf("(POLL=%d ms, DEBOUNCE=%d ms, MARGEN=%d ms)\n\n", POLL_MS, DEBOUNCE_MS, PULSE_MARGIN_MS);

    tty_raw_enable();
    atexit(tty_raw_disable);

    led_write(0);                        // LED inicia OFF
    long long next_tick = now_ms();      // primer tick de polling

    while (1){
        // 1) Leer teclado (no bloqueante)
        int c = getchar();
        if (c != EOF){
            if (c == 'q' || c == 'Q') {
                printf("Saliendo...\n");
                break;
            } else if (c == '1') {
                // Si no hay pulso en curso, generamos uno nuevo: 1 mantenido el tiempo suficiente
                if (!virt_pressed){
                    GPIO_BUTTON   = 1;   // “presionado”
                    virt_pressed  = 1;
                    // Mantén “1” por DEBOUNCE_MS + MARGEN (para garantizar que el polling lo vea estable)
                    virt_release_at = now_ms() + DEBOUNCE_MS + PULSE_MARGIN_MS;
                }
            }
            // ignoramos otras teclas: NO hace falta '0'
        }

        // 2) Polling periódico (ANTES de liberar el pulso virtual)
        long long now = now_ms();
        if (now >= next_tick){
            int raw = GPIO_BUTTON;  // estado crudo del “pin”
            if (debounce_press(raw, DEBOUNCE_MS)){
                led_write(!GPIO_LED); // toggle al flanco 0->1 confirmado
            }
            next_tick += POLL_MS;   // programar próximo tick
        }

        // 3) AHORA sí: auto-liberación del pulso (0) DESPUÉS del polling
        if (virt_pressed && now_ms() >= virt_release_at){
            GPIO_BUTTON  = 0;       // “soltado” automático
            virt_pressed = 0;
        }

        // 4) Pequeño descanso
        sleep_ms(1);
    }
    return 0;
}
