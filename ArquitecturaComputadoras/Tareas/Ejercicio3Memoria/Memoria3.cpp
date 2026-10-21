#include <iostream>
#include <fstream>
#include <cstring>  // Para memcpy, memmove, memset
#include <iomanip>

using namespace std;

// Función auxiliar para imprimir un arreglo completo o por partes
void mostrarPrimerosYUltimosCinco(const int arr[], int tamano, const string& nombre) {
    cout << "\n--- " << nombre << " ---" << endl;
    if (tamano <= 10) {
        for (int i = 0; i < tamano; ++i) {
            cout << arr[i] << " ";
        }
        cout << endl;
        return;
    }

    cout << "Primeros 5 elementos: ";
    for (int i = 0; i < 5; ++i) {
        cout << arr[i] << " ";
    }
    cout << "\nÚltimos 5 elementos: ";
    for (int i = tamano - 5; i < tamano; ++i) {
        cout << arr[i] << " ";
    }
    cout << endl;
}

int main() {
    const int TAM_ORIGEN = 1000;
    const int TAM_DESTINO = 1000 - 900 + 0; // 100 elementos (índices 500 a 599 inclusive)
    const int INDICE_INICIO = 500;
    const int CANTIDAD_TRANSFERIR = 100;

    int arregloOriginal[TAM_ORIGEN];
    int arregloDestino[CANTIDAD_TRANSFERIR];

    // 1. Crear un archivo simulado en memoria secundaria si no existe
    ofstream archivoSalida("datos.bin", ios::binary);
    if (archivoSalida.is_open()) {
        for (int i = 0; i < TAM_ORIGEN; ++i) {
            int valor = i + 1; // Llenar con valores correlativos 1, 2, 3... 1000
            archivoSalida.write(reinterpret_cast<char*>(&valor), sizeof(int));
        }
        archivoSalida.close();
    }

    // 2. Leer el arreglo de 1000 elementos almacenado en memoria secundaria (archivo)
    ifstream archivoEntrada("datos.bin", ios::binary);
    if (!archivoEntrada.is_open()) {
        cerr << "Error al abrir el archivo de memoria secundaria." << endl;
        return 1;
    }

    archivoEntrada.read(reinterpret_cast<char*>(arregloOriginal), sizeof(int) * TAM_ORIGEN);
    archivoEntrada.close();

    cout << "=== ESTADO INICIAL DE LA MEMORIA ===" << endl;
    mostrarPrimerosYUltimosCinco(arregloOriginal, TAM_ORIGEN, "Arreglo Original (Leído de Archivo)");

    // 3. Transferir los 100 elementos (índices 500 al 599) al segundo arreglo mediante memcpy / memmove
    memcpy(arregloDestino, &arregloOriginal[INDICE_INICIO], sizeof(int) * CANTIDAD_TRANSFERIR);

    // 4. Limpiar los valores transferidos en el arreglo original estableciéndolos a cero (memset)
    memset(&arregloOriginal[INDICE_INICIO], 0, sizeof(int) * CANTIDAD_TRANSFERIR);

    // 5. Salidas por consola
    cout << "\n=== ESTADO TRAS LA TRANSFERENCIA Y LIMPIEZA ===" << endl;
    
    // Muestra los primeros y últimos 5 del arreglo inicial antes/después del traslado
    mostrarPrimerosYUltimosCinco(arregloOriginal, TAM_ORIGEN, "Arreglo Original (Posterior al Traslado)");

    // Muestra los 100 elementos transferidos al segundo arreglo (primeros y últimos 5)
    mostrarPrimerosYUltimosCinco(arregloDestino, CANTIDAD_TRANSFERIR, "Arreglo Destino (100 elementos)");

    // Validar que los índices 500 al 599 contengan ceros
    bool estaLimpio = true;
    for (int i = INDICE_INICIO; i < INDICE_INICIO + CANTIDAD_TRANSFERIR; ++i) {
        if (arregloOriginal[i] != 0) {
            estaLimpio = false;
            break;
        }
    }

    cout << "\n=== VALIDACIÓN DE MEMORIA ===" << endl;
    if (estaLimpio) {
        cout << "[ÉXITO] Verificación correcta: Los índices 500 a 599 del arreglo original contienen únicamente ceros." << endl;
    } else {
        cout << "[ERROR] Falló la verificación: Algunos elementos entre los índices 500 y 599 no son cero." << endl;
    }

    return 0;
}