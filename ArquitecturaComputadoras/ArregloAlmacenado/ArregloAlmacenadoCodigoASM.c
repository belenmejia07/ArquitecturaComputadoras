#include <stdio.h>

int main() {
    int arr[] = {5, 12, 8, 20, 3};
    int n = sizeof(arr) / sizeof(arr[0]);
    int suma = 0;

    __asm__ __volatile__ (
        "xor %%eax, %%eax\n\t"         // Acumulador (suma) inicializado en 0
        "xor %%rcx, %%rcx\n\t"         // Índice del bucle (i) en 64 bits
        "movslq %[len], %%rsi\n\t"     // Extiende 'n' a 64 bits en %%rsi para compararlo con %%rcx
        "1:\n\t"
        "cmp %%rsi, %%rcx\n\t"         // Compara el índice (%%rcx) con el tamaño (%%rsi)
        "jge 2f\n\t"                   // Si i >= n, salta fuera del bucle
        "mov (%[array], %%rcx, 4), %%edx\n\t" // Carga arr[i] usando base de 64 bits e índice de 64 bits
        "add %%edx, %%eax\n\t"         // Suma el valor al acumulador
        "inc %%rcx\n\t"                // Incrementa el índice (i++)
        "jmp 1b\n\t"                   // Salta de regreso al inicio del bucle
        "2:\n\t"
        "mov %%eax, %[result]"         // Copia el resultado final a la variable 'suma'
        : [result] "=r" (suma)         // Salida
        : [array] "r" (arr), [len] "r" (n) // Entradas
        : "%eax", "%rcx", "%rdx", "%rsi", "cc", "memory" // Registros modificados (agregado %rsi)
    );

    printf("La suma es: %d\n", suma);
    return 0;
}