# Guía Práctica de Laboratorio: Optimización en la Jerarquía de Memoria
**Análisis Empírico de Líneas de Caché (64B), Registros de CPU y Paralelismo ILP en Linux**

---

* **Universidad:** Universidad Católica Boliviana "San Pablo" (UCB)
* **Unidad Académica:** Regional Santa Cruz — Departamento de Ingenierías y Ciencias Exactas
* **Carrera:** Ingeniería de Software (Semestre 1/2026)
* **Asignatura:** Arquitectura de Computadoras (SIS-131)
* **Docente:** Ing. Paulo César Loayza Carrasco
* **Entorno:** GNU/Linux (Ubuntu / Debian / WSL2)

---

## 1. Objetivos del Laboratorio

* **Comprobar empíricamente** la penalización temporal de un fallo de caché (*Cache Miss*) frente al acierto (*Cache Hit*).
* **Comprender la estructura física** de la Línea de Caché de 64 Bytes y su aprovechamiento mediante el orden contiguo en memoria (*Row-Major Order*).
* **Manipular la retención de operandos** en Registros de la CPU (FPU/ALU) para mitigar el cuello de botella del bus de memoria.
* **Analizar el Paralelismo a Nivel de Instrucción (ILP)** mediante la técnica de desenrollado de bucles (*Loop Unrolling*).
* **Obtener mediciones determinísticas**, calcular el factor de aceleración (*Speedup*: $S = T_{\text{base}} / T_{\text{opt}}$) y el rendimiento efectivo en GFLOPS.

---

## 2. Verificación Previa del Hardware en Linux

Antes de compilar y ejecutar el benchmark, se realiza la inspección física de la topología de la CPU y la estructura de las memorias caché del sistema de pruebas.

### 2.1. Consulta de Jerarquía y Tamaños de Caché

Se ejecuta el comando `lscpu` filtrando las líneas de interés:

```bash
lscpu | grep -E "L1|L2|L3|Model name"
```

**Evidencia de Ejecución:**

![Model Name y Caches L1, L2, L3](Imagenes/ConsultaJerarquia.png)

**Análisis de Hardware Identificado:**
* **Procesador:** 11th Gen Intel(R) Core(TM) i9-11900H @ 2.50GHz
* **Caché L1d (Datos):** 192 KiB (4 instancias)
* **Caché L1i (Instrucciones):** 128 KiB (4 instancias)
* **Caché L2:** 5 MiB (4 instancias)
* **Caché L3:** 96 MiB (4 instancias)

---

### 2.2. Determinación del Tamaño de la Línea de Caché L1d

Se verifica el tamaño exacto del bloque de transferencia a la caché L1d mediante `getconf`:

```bash
getconf LEVEL1_DCACHE_LINESIZE
```

**Evidencia de Ejecución:**

![L1 Cache Line Size](Imagenes/TamanoLineaCache.png)

* **Resultado:** **64 Bytes**

---

### 2.3. Confirmación del Bloque de Coherencia vía `sysfs`

Se inspecciona directamente la interfaz del Kernel de Linux (`sysfs`):

```bash
cat /sys/devices/system/cpu/cpu0/cache/index0/coherency_line_size
```

**Evidencia de Ejecución:**

![Coherency Line Size Sysfs](Imagenes/BloqueCoherencia.png)

* **Resultado:** **64 Bytes**

---

### 2.4. Fundamento Físico de las Mediciones

A partir de las mediciones del hardware obtenidas:
1. Una línea de caché física mide **64 Bytes**.
2. Dado que cada número flotante de precisión simple (`float`) en norma IEEE 754 requiere **4 Bytes** (`sizeof(float) = 4`):

$$\text{Elementos por Línea de Caché} = \frac{64 \text{ Bytes}}{4 \text{ Bytes/float}} = 16 \text{ floats}$$

**Principio de Funcionamiento:**
Cuando la CPU necesita acceder a una dirección de memoria no presente en caché, se genera un *Cache Miss*. El controlador de memoria transfiere la **línea física completa de 64 Bytes** (16 elementos contiguos) desde la memoria principal a la caché L1. Si la estructura del bucle accede a los datos de forma contigua en memoria (*Row-Major Order*), el primer acceso provocará 1 fallo (*Miss*), pero los siguientes 15 accesos se resolverán inmediatamente en la caché L1 (*Hits*).

---

## 3. Arquitectura del Algoritmo y Variantes Experimentales

El benchmark evalúa la multiplicación de matrices de punto flotante $C = A \times B$ de tamaño $N \times N$ ($1024 \times 1024$). La huella total de memoria ocupada por las matrices es de aproximadamente **24 MB** ($6 \text{ matrices} \times 4 \text{ MB}$), superando con frecuencia la capacidad de las cachés más rápidas.

1. **Fase 1 — Naive ($i$-$j$-$k$):**  
   Algoritmo directo con orden tradicional de bucles. En la matriz $B[k][j]$, el índice interno que varía es $k$. Como las matrices en C se almacenan por filas (*Row-Major Order*), cada incremento en $k$ realiza un salto de $N \times 4 \text{ bytes} = 4096 \text{ bytes}$ en la memoria. Esto evapora la localidad espacial, desaprovecha el 93.75% de la línea de caché cargada y desencadena *Cache Misses* continuos.

2. **Fase 2 — Localidad Espacial ($i$-$k$-$j$):**  
   Se reordenan los bucles intercambiando $j$ y $k$. Al iterar sobre el bucle interno con $j$, los accesos a $B[k][j]$ y $C[i][j]$ son estrictamente contiguos en memoria (salto de 4 bytes). Se aprovecha la línea completa de 64 bytes alcanzando un *Hit Rate* teórico del **93.75%**.

3. **Fase 3 — Localidad Temporal + Registros de CPU:**  
   Dado que el operando $A[i][k]$ permanece inalterado a lo largo de toda la iteración del bucle interno $j$, se almacena explícitamente en un registro vectorial/flotante de la CPU (`register const float reg_a`). Se elimina la necesidad de leer repetidamente dicho escalar de la memoria RAM o de la caché durante $N$ iteraciones.

4. **Fase 4 — Loop Unrolling 4x + ILP:**  
   Se aplica desenrollado de bucles procesando 4 iteraciones del bucle interno de forma paralela en una sola pasada ($j, j+1, j+2, j+3$). Esto reduce sustancialmente el coste de instrucciones de control de flujo (saltos y comparaciones) y permite que los pipelines superescalares del microprocesador ejecuten múltiples operaciones en unidades FMA (*Fused Multiply-Add*) de forma simultánea.

---

## 4. Código Fuente Oficial del Benchmark (`benchmark_arquitectura.c`)

```c
#define _POSIX_C_SOURCE 199309L
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>

#define N 1024

static double medir_tiempo_segundos(void) {
    struct timespec ts;
    clock_gettime(CLOCK_MONOTONIC, &ts);
    return (double)ts.tv_sec + (double)ts.tv_nsec / 1e9;
}

/* 1. NAIVE (i-j-k): Ineficiente, Cache Miss masivo */
void algoritmo_naive(const float *A, const float *B, float *C) {
    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            float suma = 0.0f;
            for (int k = 0; k < N; k++) {
                suma += A[i * N + k] * B[k * N + j]; // Salto de fila en B: 4096 bytes
            }
            C[i * N + j] = suma;
        }
    }
}

/* 2. LOCALIDAD ESPACIAL (i-k-j): Aprovecha linea de cache de 64 bytes */
void algoritmo_localidad_espacial(const float *A, const float *B, float *C) {
    for (int idx = 0; idx < N * N; idx++) C[idx] = 0.0f;
    for (int i = 0; i < N; i++) {
        for (int k = 0; k < N; k++) {
            float r = A[i * N + k];
            int fila_b = k * N, fila_c = i * N;
            for (int j = 0; j < N; j++) {
                C[fila_c + j] += r * B[fila_b + j]; // Acceso strictly contiguo
            }
        }
    }
}

/* 3. LOCALIDAD TEMPORAL + REGISTROS: Mantiene A[i][k] en registro del CPU */
void algoritmo_registros_cpu(const float *A, const float *B, float *C) {
    for (int idx = 0; idx < N * N; idx++) C[idx] = 0.0f;
    for (int i = 0; i < N; i++) {
        float *ptr_c = &C[i * N];
        for (int k = 0; k < N; k++) {
            register const float reg_a = A[i * N + k]; // En registro de la FPU
            const float *ptr_b = &B[k * N];
            for (int j = 0; j < N; j++) {
                ptr_c[j] += reg_a * ptr_b[j];
            }
        }
    }
}

/* 4. LOOP UNROLLING 4X + ILP: Paralelismo a nivel de instruccion */
void algoritmo_loop_unrolling(const float *A, const float *B, float *C) {
    for (int idx = 0; idx < N * N; idx++) C[idx] = 0.0f;
    for (int i = 0; i < N; i++) {
        float *ptr_c = &C[i * N];
        for (int k = 0; k < N; k++) {
            register const float reg_a = A[i * N + k];
            const float *ptr_b = &B[k * N];
            for (int j = 0; j < N; j += 4) {
                ptr_c[j]     += reg_a * ptr_b[j];
                ptr_c[j + 1] += reg_a * ptr_b[j + 1];
                ptr_c[j + 2] += reg_a * ptr_b[j + 2];
                ptr_c[j + 3] += reg_a * ptr_b[j + 3];
            }
        }
    }
}

static double calcular_checksum(const float *mat) {
    double sum = 0.0;
    for (int i = 0; i < N * N; i++) sum += (double)mat[i];
    return sum;
}

int main(void) {
    size_t total = (size_t)N * N, bytes = total * sizeof(float);
    double gflops = (2.0 * (double)N * N * N) / 1e9;
    float *A = (float*)malloc(bytes), *B = (float*)malloc(bytes);
    float *C1 = (float*)malloc(bytes), *C2 = (float*)malloc(bytes);
    float *C3 = (float*)malloc(bytes), *C4 = (float*)malloc(bytes);

    for (int i = 0; i < N; i++) {
        for (int j = 0; j < N; j++) {
            A[i * N + j] = (float)((i + j) % 50) * 0.02f + 1.0f;
            B[i * N + j] = (float)((i * 2 + j) % 50) * 0.02f + 0.5f;
        }
    }

    double t0, t_naive, t_cache, t_reg, t_unroll;
    t0 = medir_tiempo_segundos(); algoritmo_naive(A, B, C1); t_naive = medir_tiempo_segundos() - t0;
    t0 = medir_tiempo_segundos(); algoritmo_localidad_espacial(A, B, C2); t_cache = medir_tiempo_segundos() - t0;
    t0 = medir_tiempo_segundos(); algoritmo_registros_cpu(A, B, C3); t_reg = medir_tiempo_segundos() - t0;
    t0 = medir_tiempo_segundos(); algoritmo_loop_unrolling(A, B, C4); t_unroll = medir_tiempo_segundos() - t0;

    printf("\n=== RESULTADOS DETERMINISTICOS (N=%d) ===\n", N);
    printf("1. Naive (i-j-k)          : %7.4f s | %6.2f GFLOPS | Speedup: 1.00x\n", t_naive, gflops/t_naive);
    printf("2. Localidad Espacial (Cache) : %7.4f s | %6.2f GFLOPS | Speedup: %.2fx\n", t_cache, gflops/t_cache, t_naive / t_cache);
    printf("3. Registros de CPU       : %7.4f s | %6.2f GFLOPS | Speedup: %.2fx\n", t_reg, gflops/t_reg, t_naive / t_reg);
    printf("4. Loop Unrolling 4x (ILP) : %7.4f s | %6.2f GFLOPS | Speedup: %.2fx\n", t_unroll, gflops/t_unroll, t_naive / t_unroll);

    double chk1 = calcular_checksum(C1), chk4 = calcular_checksum(C4);
    printf("[OK] Validacion de Checksum: %.4e (Error = %.4e)\n", chk1, fabs(chk1 - chk4));

    free(A); free(B); free(C1); free(C2); free(C3); free(C4);
    return 0;
}
```

---

## 5. Compilación y Ejecución Paso a Paso en Linux

### Paso 1: Compilación Determinística Oficial
Compilar el programa fijando el nivel de optimización `-O1`. Esto deshabilita auto-vectorizaciones agresivas o reordenamiento automático de bucles por parte del compilador, garantizando que los resultados reflejen la optimización realizada a nivel de código.

```bash
gcc -Wall -Wextra -O1 benchmark_arquitectura.c -o benchmark_arquitectura -lm
```

> **Nota:** Se utiliza el flag `-O1` porque aplica optimizaciones estándar sin alterar la topología de los bucles programados, permitiendo medir el impacto mecánico directo del diseño del algoritmo.

### Paso 2: Ejecución Directa
Ejecutar el binario generado en la terminal:

```bash
./benchmark_arquitectura
```

### Paso 3: Análisis de Rendimiento con `perf`
Para monitorear los contadores de hardware del procesador (PMU), instalar y ejecutar la herramienta `perf`:

```bash
# Instalación previa si no está presente:
sudo apt-get update && sudo apt-get install -y linux-tools-generic linux-tools-common

# Perfilado de contadores de hardware:
perf stat -e L1-dcache-loads,L1-dcache-load-misses,cycles,instructions ./benchmark_arquitectura
```

---

## 6. Plantilla de Registro de Métricas (Para el Estudiante)

Llene los datos registrados durante la ejecución en su equipo:

| Fase / Configuración | Tiempo Medido (s) | Rendimiento (GFLOPS) | Aceleración (Speedup) | Tasa Acierto Caché |
| :--- | :---: | :---: | :---: | :---: |
| **1. Naive (i-j-k)** | ______ s | ______ GFLOPS | 1.00x (Base) | Baja (< 15%) |
| **2. Localidad Espacial (i-k-j)** | ______ s | ______ GFLOPS | ______ x | Alta (> 90%) |
| **3. Uso de Registros CPU** | ______ s | ______ GFLOPS | ______ x | Óptima |
| **4. Loop Unrolling 4x (ILP)** | ______ s | ______ GFLOPS | ______ x | Máxima |

---

## 7. Cuestionario de Análisis Crítico

### Pregunta 1 (Líneas de Caché)
**Si una línea de caché mide 64 bytes y cada float ocupa 4 bytes, ¿cuántos accesos a memoria consecutivos aprovechan una sola carga a la caché L1 en la Fase 2? Demuestre la fórmula teórica del Hit Rate.**

**Respuesta:**
En un esquema de almacenamiento contiguo (*Row-Major Order*), al iterar sobre la variable $j$ (bucle interno de la Fase 2), los elementos $B[k][j]$ se encuentran alineados secuencialmente en memoria.

1. **Cantidad de accesos aprovechados:**
   $$\text{Elementos por línea} = \frac{64 \text{ Bytes}}{4 \text{ Bytes/float}} = 16 \text{ accesos consecutivos}$$

2. **Demostración de la Fórmula del Hit Rate ($HR$):**
   Para cada bloque de 16 elementos consecutivos, el primer acceso provoca un fallo de caché (*Cache Miss*) al requerir la lectura de la línea desde la memoria principal o L2. Los siguientes 15 accesos leen los datos directamente de la caché L1d (*Cache Hits*).

   $$\text{Hit Rate } (HR) = \frac{\text{Aciertos}}{\text{Accesos Totales}} = \frac{15}{16} = 0.9375 \Rightarrow \mathbf{93.75\%}$$

---

### Pregunta 2 (Localidad Temporal y Registros)
**En la Fase 3, ¿qué ventaja física representa almacenar `reg_a` en un registro de la FPU/ALU en lugar de releerlo desde la memoria en cada paso del bucle interior?**

**Respuesta:**
1. **Diferencia de Latencia Física:**  
   Acceder a los registros del CPU (`XMM`/`YMM` de la FPU) requiere aproximadamente **1 ciclo de reloj** ($< 0.5 \text{ ns}$). Leer desde la caché L1 toma entre **4 y 5 ciclos**, mientras que un acceso a la memoria RAM principal requiere entre **150 y 200 ciclos de reloj** (~50–70 ns).
2. **Mitigación del Cuello de Botella del Bus:**  
   Al almacenar el operando $A[i][k]$ en la variable cualificada como `register`, se retiene en un registro escalar del microprocesador durante las $N$ iteraciones del bucle interno $j$. Esto evita realizar $N$ lecturas repetitivas por el bus de datos hacia la caché o memoria, liberando ancho de banda del bus para la transmisión contigua de los arreglos $B$ y $C$.

---

### Pregunta 3 (Loop Unrolling e ILP)
**¿Por qué el desenrollado de bucles en la Fase 4 reduce el tiempo de ejecución incluso cuando el número total de sumas y multiplicaciones matemáticas es exactamente el mismo?**

**Respuesta:**
Aunque el trabajo aritmético (FLOPs) es idéntico, la Fase 4 optimiza el rendimiento por las siguientes razones de arquitectura:
1. **Reducción de Sobrecarga de Bucles (*Loop Overhead*):**  
   Al avanzar de a 4 elementos (`j += 4`), las instrucciones de control (incremento de $j$, comparación de límites y saltos condicionales `jmp`/`jne`) se reducen en un **75%**.
2. **Explotación del Paralelismo a Nivel de Instrucción (ILP):**  
   Los procesadores modernos disponen de múltiples unidades de ejecución (pipelines superescalares y unidades FMA). Al desenrollar las instrucciones:
   ```c
   ptr_c[j]     += reg_a * ptr_b[j];
   ptr_c[j + 1] += reg_a * ptr_b[j + 1];
   ptr_c[j + 2] += reg_a * ptr_b[j + 2];
   ptr_c[j + 3] += reg_a * ptr_b[j + 3];
   ```
   Se presentan múltiples operaciones independientes que pueden ser despachadas simultáneamente a distintas unidades funcionales, reduciendo los puestos de espera (*stalls*) en el pipeline.

---

### Pregunta 4 (Validación de Determinismo)
**¿Por qué el valor del Checksum matemático debe ser idéntico en las cuatro fases? ¿Qué indicaría si el Checksum de la Fase 4 difiere del de la Fase 1?**

**Respuesta:**
1. **Identidad Matemática:**  
   El Checksum representa la suma acumulada de todos los elementos de la matriz resultante $C$. Dado que la multiplicación matricial en aritmética de punto flotante sobre los mismos datos de entrada debe producir exactamente los mismos resultados numéricos bajo estas trasformaciones de bucle, las cuatro matrices finales deben ser idénticas.
2. **Interpretación de una Discrepancia:**  
   Si el Checksum de la Fase 4 difiriera del de la Fase 1, indicaría:
   * **Condición de Carrera o Desbordamiento de Índices:** Un error de indexación al iterar de 4 en 4 (ej. acceder a posiciones de memoria fuera del límite $N$).
   * **Error de Corrupción de Memoria:** Un puntero mal direccionado escribiendo en regiones incorrectas.
   * **Acumulación de Redondeo:** Variaciones en el orden de acumulación de operaciones flotantes (asociatividad no estricta de IEEE 754 cuando hay vectorización de por medio), lo que violaría la validez del benchmark.
## 9. Prueba de la compilacion del codigo BenchMark
![Pruebas](Imagenes/ResultadoBenckMark.png)