// Control de Afinidad de CPU en C/C++
// Windows (MSVC):  cl /EHsc /O2 afinidad_cpu.cpp
// Windows (MinGW): g++ -O2 -std=c++17 afinidad_cpu.cpp -o afinidad_cpu
// Linux:           g++ -O2 -std=c++17 -pthread afinidad_cpu.cpp -o afinidad_cpu

#include <atomic>
#include <cmath>
#include <iostream>
#include <set>
#include <thread>
#include <vector>

#ifdef _WIN32
  #include <windows.h>
#else
  #ifndef _GNU_SOURCE
    #define _GNU_SOURCE
  #endif
  #include <pthread.h>
  #include <sched.h>
#endif

std::atomic<bool> detener{false};

// Fija la afinidad del hilo que la llama (el propio hilo) a un solo nucleo.
bool fijarAfinidad(int nucleo) {
#ifdef _WIN32
    DWORD_PTR mascara = static_cast<DWORD_PTR>(1) << nucleo;
    return SetThreadAffinityMask(GetCurrentThread(), mascara) != 0;
#else
    cpu_set_t cpuset;
    CPU_ZERO(&cpuset);
    CPU_SET(nucleo, &cpuset);
    return pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset) == 0;
#endif
}

// Rutina de estres: calculo intensivo hasta que se pida detener.
void trabajador(int nucleo) {
    if (!fijarAfinidad(nucleo)) {
        std::cerr << "[!] No se pudo fijar la afinidad al nucleo " << nucleo << "\n";
    } else {
        std::cout << "[+] Hilo fijado al nucleo " << nucleo << "\n";
    }

    volatile double sumidero = 0.0;  // evita que el compilador elimine el bucle
    double x = 1.0001;
    while (!detener.load(std::memory_order_relaxed)) {
        for (int i = 0; i < 100000; ++i) {
            x = std::sin(x) * std::cos(x) + std::sqrt(x + 1.0);
        }
        sumidero = x;
    }
}

int main() {
    // 1. Deteccion
    const int N = static_cast<int>(std::thread::hardware_concurrency());
    if (N <= 0) {
        std::cerr << "No se pudo detectar el numero de nucleos.\n";
        return 1;
    }
    std::cout << "Nucleos logicos detectados: " << N << "\n";
#ifdef _WIN32
    if (N > 64) std::cout << "Aviso: en Windows solo se usan los primeros 64 nucleos (grupo de procesadores).\n";
#endif

    // 2. Configuracion
    int cantidad = 0;
    std::cout << "Cuantos nucleos desea utilizar (1-" << N << ")? ";
    while (!(std::cin >> cantidad) || cantidad < 1 || cantidad > N) {
        std::cin.clear();
        std::cin.ignore(10000, '\n');
        std::cout << "Valor invalido. Ingrese un numero entre 1 y " << N << ": ";
    }

    std::set<int> seleccion;
    std::cout << "Ingrese los indices de los nucleos (0-" << N - 1 << "):\n";
    while (static_cast<int>(seleccion.size()) < cantidad) {
        int n;
        std::cout << "  Nucleo " << seleccion.size() + 1 << "/" << cantidad << ": ";
        if (!(std::cin >> n) || n < 0 || n >= N || seleccion.count(n)) {
            std::cin.clear();
            std::cin.ignore(10000, '\n');
            std::cout << "  Indice invalido o repetido.\n";
            continue;
        }
        seleccion.insert(n);
    }
    std::cin.ignore(10000, '\n');

    // 3 y 4. Ejecucion + Estres: un hilo por nucleo seleccionado
    std::vector<std::thread> hilos;
    for (int nucleo : seleccion) {
        hilos.emplace_back(trabajador, nucleo);
    }

    // 5. Control: terminar limpiamente con ENTER
    std::cout << "\nEstresando " << cantidad << " nucleo(s). Abra el Administrador de Tareas / Monitor de Recursos.\n";
    std::cout << "Presione ENTER para finalizar...\n";
    std::cin.get();

    detener = true;
    for (auto& h : hilos) h.join();

    std::cout << "Todos los hilos finalizados. Fin del programa.\n";
    return 0;
}