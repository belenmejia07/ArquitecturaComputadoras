#include <stdio.h>

int main() {
    int arr[] = {5, 12, 8, 20, 3};
    int n = sizeof(arr) / sizeof(arr[0]); // cantidad de elementos
    int suma = 0;

    for (int i = 0; i < n; i++) {
        suma += arr[i];
    }

    printf("La suma es: %d\n", suma);
    return 0;
}