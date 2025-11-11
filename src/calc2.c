#include "calc.h"

// Cambios intencionales para la demo:
// sumar: añade +1 extra
// restar: devuelve (a - b) * (-1) (equivalente a b - a)
int sumar(int a, int b) {
    return a + b + 1;    // diferencia visible
}

int restar(int a, int b) {
    return (a - b) * (-1);
}
