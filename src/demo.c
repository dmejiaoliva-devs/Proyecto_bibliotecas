#define _GNU_SOURCE
#include <dlfcn.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define COLOR_RESET   "\x1b[0m"
#define COLOR_GREEN   "\x1b[32m"
#define COLOR_YELLOW  "\x1b[33m"
#define COLOR_CYAN    "\x1b[36m"
#define COLOR_RED     "\x1b[31m"

typedef int (*op2i_fn)(int,int);

static void limpiar_buffer(void) {
    int c;
    while ((c = getchar()) != '\n' && c != EOF) { /* descartar */ }
}

static void* abrir_biblioteca(const char* ruta) {
    dlerror(); // limpia
    void* h = dlopen(ruta, RTLD_LAZY);
    if (!h) {
        fprintf(stderr, COLOR_RED "[ERROR] dlopen(%s): %s\n" COLOR_RESET, ruta, dlerror());
        exit(EXIT_FAILURE);
    }
    return h;
}

static op2i_fn resolver(void* h, const char* nombre) {
    dlerror();
    void* sym = dlsym(h, nombre);
    const char* err = dlerror();
    if (err) {
        fprintf(stderr, COLOR_RED "[ERROR] dlsym(%s): %s\n" COLOR_RESET, nombre, err);
        dlclose(h);
        exit(EXIT_FAILURE);
    }
    return (op2i_fn)sym;
}

int main(int argc, char** argv) {
    char ruta_so[256];
    if (argc >= 2) {
        snprintf(ruta_so, sizeof(ruta_so), "%s", argv[1]);
    } else {
        snprintf(ruta_so, sizeof(ruta_so), "./build/libcalc.so");
    }

    void* h = abrir_biblioteca(ruta_so);

    printf(COLOR_CYAN "=== Demo: carga dinámica ===\n" COLOR_RESET);
    printf(COLOR_YELLOW "Biblioteca cargada: %s\n\n" COLOR_RESET, ruta_so);

    for (;;) {
        printf("Elige función:\n"
               "  1) sumar(a,b)\n"
               "  2) restar(a,b)\n"
               "  3) cambiar biblioteca (.so) en caliente\n"
               "  0) salir\n> ");

        int op;
        if (scanf("%d", &op) != 1) { limpiar_buffer(); continue; }

        if (op == 0) break;

        if (op == 3) {
            printf("Ruta nueva de biblioteca (.so): ");
            limpiar_buffer();                       // limpiar \n previo
            if (!fgets(ruta_so, sizeof(ruta_so), stdin)) continue;
            // quitar salto de línea
            size_t n = strlen(ruta_so);
            if (n && ruta_so[n-1] == '\n') ruta_so[n-1] = '\0';

            if (dlclose(h) != 0)
                fprintf(stderr, COLOR_RED "[WARN] dlclose: %s\n" COLOR_RESET, dlerror());

            h = abrir_biblioteca(ruta_so);
            printf(COLOR_YELLOW "Biblioteca cargada ahora: %s\n\n" COLOR_RESET, ruta_so);
            continue;
        }

        const char* nombre = (op == 1) ? "sumar" :
                             (op == 2) ? "restar" : NULL;
        if (!nombre) { puts("Opción no válida.\n"); continue; }

        int a, b;
        printf("Introduce a y b: ");
        if (scanf("%d %d", &a, &b) != 2) { limpiar_buffer(); continue; }

        op2i_fn f = resolver(h, nombre);
        int r = f(a, b);
        printf(COLOR_GREEN "Resultado de %s(%d,%d) = %d\n\n" COLOR_RESET, nombre, a, b, r);
    }

    if (dlclose(h) != 0)
        fprintf(stderr, COLOR_RED "[WARN] dlclose: %s\n" COLOR_RESET, dlerror());
    else
        puts("Biblioteca descargada correctamente.");

    return 0;
}
