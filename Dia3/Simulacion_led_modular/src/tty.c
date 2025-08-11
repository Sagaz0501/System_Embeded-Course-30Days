#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif
#include <termios.h>  // termios: config de TTY (modo raw, flags, etc.)
#include <unistd.h>   // STDIN_FILENO, read(), tcgetattr/tcsetattr
#include <fcntl.h>    // fcntl(), F_GETFL, F_SETFL, O_NONBLOCK
#include <stdio.h>    // setvbuf(), printf (por si quieres logs)

/// Guardamos el estado ORIGINAL del TTY para restaurarlo al salir.
/// Si no restauras, te quedas con la terminal rara (sin eco).
static struct termios orig;

/// Guardamos los flags ORIGINALES del descriptor (stdin)
/// para restaurar O_NONBLOCK y otros que existieran.
static int orig_fl = -1;

/// Habilita modo RAW + no-bloqueante en stdin.
/// - RAW: sin echo, sin edición de línea, sin señales (Ctrl-C/Ctrl-Z), etc.
/// - No-bloqueante: read() no se queda esperando por teclas.
/// - stdout sin buffer: prints inmediatos (útil para UIs de texto).
void tty_raw_enable(void){
    // 1) Leer y guardar el estado actual del TTY.
    if (tcgetattr(STDIN_FILENO, &orig) == -1) {
        // Si esto falla, no podemos continuar. Mejor devolverse "suave".
        return;
    }

    // 2) Copiamos el estado original, y sobre esa copia armamos el "raw".
    struct termios raw = orig;

    // cfmakeraw() ajusta la estructura para "raw mode":
    //  - Desactiva ICANON (modo canónico), ECHO, ISIG (señales), IEXTEN...
    //  - Configura los flags para lectura byte a byte sin transformaciones.
    cfmakeraw(&raw);

    // 3) Ajustamos control chars para que read() NO bloquee:
    //    VMIN = 0 -> read puede retornar con 0 bytes.
    //    VTIME = 0 -> sin timeout (retorno inmediato).
    raw.c_cc[VMIN]  = 0;
    raw.c_cc[VTIME] = 0;

    // 4) Aplicamos la nueva config YA (TCSANOW).
    //    Si quisieras sincronizar con vaciado de salida, usarías TCSADRAIN.
    tcsetattr(STDIN_FILENO, TCSANOW, &raw);

    // 5) Activar O_NONBLOCK en el descriptor de stdin:
    //    - Primero leemos sus flags actuales para no perder otros.
    orig_fl = fcntl(STDIN_FILENO, F_GETFL, 0);
    if (orig_fl != -1) {
        // Seteamos O_NONBLOCK sin borrar otros flags (por eso OR).
        fcntl(STDIN_FILENO, F_SETFL, orig_fl | O_NONBLOCK);
    }

    // 6) Desactivar el buffering de stdout:
    //    - _IONBF = "no buffered": cada printf sale al instante.
    //    - Útil en programas "interactivos" de consola (UIs, juegos, etc.).
    setvbuf(stdout, NULL, _IONBF, 0);
}

/// Restaura el estado ORIGINAL del TTY y flags del descriptor.
/// Llamar SIEMPRE al salir (atexit(tty_raw_disable)) para no dejar la consola “dañada”.
void tty_raw_disable(void){
    // Restaurar atributos del terminal
    tcsetattr(STDIN_FILENO, TCSANOW, &orig);

    // Restaurar flags del descriptor (quitar O_NONBLOCK si no estaba)
    if (orig_fl != -1) {
        fcntl(STDIN_FILENO, F_SETFL, orig_fl);
    }
}

/// Lee UNA tecla en modo no bloqueante.
/// Devuelve: 0..255 (unsigned char) si hay tecla, o -1 si no hay nada.
/// Nota: NO imprime, NO hace echo (estás en raw), el UI lo decides tú.
int tty_getch_nonblock(void){
    unsigned char ch;
    // read() intenta leer 1 byte de stdin.
    // Con O_NONBLOCK + VMIN/VTIME=0, si no hay datos devuelve -1 (errno=EAGAIN/EWOULDBLOCK).
    ssize_t n = read(STDIN_FILENO, &ch, 1);
    if (n == 1) {
        // ¡Hay tecla! devolvemos su código (char -> int)
        return ch;
    }
    // No hay tecla pendiente.
    return -1;
}
