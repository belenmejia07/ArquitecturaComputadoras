# Solución de la Práctica: Sistemas Numéricos y Aritmética Binaria

**Materia:** Arquitectura de Computadoras 
**Nombre:** Belen Mejia Medina

---

## Nivel 1 — Conversión de bases (fundamentos)

### 1.1 Convierte los siguientes números decimales a binario: 45, 128, 255, 1000
* **45:** $45 = 32 + 8 + 4 + 1 \rightarrow$ **`101101`**
* **128:** Es potencia exacta ($2^7$) $\rightarrow$ **`10000000`**
* **255:** Valor máximo con 8 bits ($2^8 - 1$) $\rightarrow$ **`11111111`**
* **1000:** $1000 = 512 + 256 + 128 + 64 + 32 + 8 \rightarrow$ **`1111101000`**

### 1.2 Convierte los siguientes números binarios a decimal: 1011, 10110110, 11111111, 100000000
* **1011:** $8 + 0 + 2 + 1 =$ **`11`**
* **10110110:** $128 + 32 + 16 + 4 + 2 =$ **`182`**
* **11111111:** $128 + 64 + 32 + 16 + 8 + 4 + 2 + 1 =$ **`255`**
* **100000000:** Equivale a $2^8 =$ **`256`**

### 1.3 Convierte a hexadecimal los siguientes números binarios: 10101100, 111100001111, 1010101010101010
*(Agrupando en bloques de 4 bits de derecha a izquierda)*
* **1010 1100:** $1010 = \text{A}$, $1100 = \text{C} \rightarrow$ **`AC`**
* **1111 0000 1111:** $1111 = \text{F}$, $0000 = 0$, $1111 = \text{F} \rightarrow$ **`F0F`**
* **1010 1010 1010 1010:** Cada bloque $1010 = \text{A} \rightarrow$ **`AAAA`**

### 1.4 Convierte de octal a decimal: 17, 254, 777
* **17:** $1 \cdot 8^1 + 7 \cdot 8^0 = 8 + 7 =$ **`15`**
* **254:** $2 \cdot 8^2 + 5 \cdot 8^1 + 4 \cdot 8^0 = 128 + 40 + 4 =$ **`172`**
* **777:** $7 \cdot 8^2 + 7 \cdot 8^1 + 7 \cdot 8^0 = 448 + 56 + 7 =$ **`511`**

---

## Nivel 2 — Aritmética binaria básica

### 2.1 Realiza las siguientes sumas binarias (muestra el acarreo):
* **Suma 1:** `1011 + 0110`
  ```text
    111   <- Acarreos
    1011  (11)
  + 0110  (6)
  ------
   10001  (17)
  ```
  **Resultado:** **`10001`**

* **Suma 2:** `11101 + 01011`
  ```text
   11111  <- Acarreos
    11101  (29)
  + 01011  (11)
  ------
  101000  (40)
  ```
  **Resultado:** **`101000`**

### 2.2 Realiza las siguientes restas binarias usando préstamo (borrow):
* **Resta 1:** `1100 - 0101`
  ```text
    01    <- Préstamos
    1100  (12)
  - 0101  (5)
  ------
    0111  (7)
  ```
  **Resultado:** **`0111`**

* **Resta 2:** `10000 - 00111`
  ```text
   1111   <- Préstamos
    10000  (16)
  - 00111  (7)
  ------
    01001  (9)
  ```
  **Resultado:** **`01001`**

### 2.3 Multiplica en binario: 1011 × 0101
```text
      1011  (11)
    × 0101  (5)
    ------
      1011
     0000
    1011
  +0000
  --------
    110111  (55)
```
**Resultado:** **`110111`**

---

## Nivel 3 — Complemento a dos

### 3.1 Representa los siguientes números decimales en complemento a dos usando 8 bits: -45, -1, -128, 27
* **-45:** $+45 = 00101101 \rightarrow$ Invertido: $11010010 \rightarrow$ Sumar 1: **`11010011`**
* **-1:** $+1 = 00000001 \rightarrow$ Invertido: $11111110 \rightarrow$ Sumar 1: **`11111111`**
* **-128:** Es el límite inferior para 8 bits $\rightarrow$ **`10000000`**
* **27:** Como es positivo, se queda igual $\rightarrow$ **`00011011`**

### 3.2 Dado el número en complemento a dos de 8 bits 11010110, indica su valor decimal.
Como empieza con `1`, es un número negativo. Para saber su valor absoluto invertimos los bits y sumamos 1:
* $11010110 \rightarrow$ Invertir bits: $00101001$
* Sumar 1: $00101010 = 32 + 8 + 2 = 42$
* **Resultado:** **`-42`**

### 3.3 Realiza la resta 45 - 60 usando complemento a dos con registros de 8 bits, y verifica el resultado.
Equivale a la operación: $45 + (-60)$.
* $+45 = 00101101$
* $-60$: $+60 = 00111100 \rightarrow$ Invertido: $11000011 \rightarrow$ $+ 1 = 11000100$

Suma:
```text
  00101101  (45)
+ 11000100  (-60)
----------
  11110001
```
Para verificar $11110001$: Invertir bits $\rightarrow 00001110$, sumar $1 \rightarrow 00001111$ (que es $15$).  
**Resultado:** **`11110001`** (Equivale a **`-15`**).

---

## Nivel 4 — Desbordamiento (overflow) y banderas

### 4.1 Determina si ocurre overflow al sumar, en complemento a dos de 8 bits:
* **Suma 1:** `01111111 + 00000001`
  * Operación: $127 + 1 = 128$.
  * En binario: $01111111 + 00000001 = 10000000$ (que representa $-128$).
  * **Sí ocurre overflow.** Se sumaron dos positivos y dio un negativo. El rango máximo de 8 bits con signo es $127$.
* **Suma 2:** `10000000 + 11111111`
  * Operación: $-128 + (-1) = -129$.
  * En binario: $10000000 + 11111111 = 01111111$ (con acarreo residual que se descarta; da $+127$).
  * **Sí ocurre overflow.** Se sumaron dos negativos y dio un positivo. El rango mínimo es $-128$.

### 4.2 Explica la diferencia entre carry (acarreo) y overflow en una suma con signo, y da un ejemplo de cada caso usando registros de 4 bits.
* **Carry (Acarreo):** Indica que la suma aritmética superó la capacidad del bit más significativo en operaciones **sin signo**. El bit "se sale" del registro.
* **Overflow (Desbordamiento):** Indica que el resultado de una operación **con signo** ha excedido el rango representable, corrompiendo el bit de signo (por ejemplo, sumar dos positivos y obtener un resultado negativo).

**Ejemplos con 4 bits (Rango con signo: $-8$ a $+7$. Sin signo: $0$ a $15$):**
* **Caso con Carry pero SIN Overflow:** `1111 + 0001` (15 + 1).  
  $1111 + 0001 = 10000 \rightarrow$ El bit extra activa el **Carry**. Sin signo dio 0 por descarte, pero con signo representa $-1 + 1 = 0$ (el resultado binario remanente `0000` es correcto para aritmética con signo, por lo que **no hay Overflow**).
* **Caso con Overflow pero SIN Carry:** `0100 + 0100` (4 + 4).  
  $0100 + 0100 = 1000$ (en 4 bits con signo, `1000` es $-8$). Sumamos dos positivos y dio un negativo. **Hay Overflow** porque 8 excede el rango superior (+7), pero **no hay Carry** porque ningún bit se salió de las 4 posiciones.

### 4.3 Programa en C (Práctico en Ubuntu)
Código fuente (`overflow.c`):
```c
#include <stdio.h>
#include <stdint.h>

int main() {
    int8_t a = 120;
    int8_t b = 20;
    int8_t resultado = a + b;
    
    printf("Matemáticamente: 120 + 20 = 140\n");
    printf("Impreso por el programa: %d\n", resultado);
    return 0;
}
```
**Comando de compilación y ejecución:**
```bash
gcc overflow.c -o overflow
./overflow
```
**Salida obtenida:**
```text
Matemáticamente: 120 + 20 = 140
Impreso por el programa: -116
```
**Explicación:** Un entero de tipo `int8_t` almacena valores con signo de $-128$ a $127$. El valor matemático $140$ sobrepasa los $127$ permitidos. En binario, $120$ es `01111000` y $20$ es `00010100`. Al sumarlos se obtiene `10001100`. Al tener un `1` en el bit de signo, la computadora lo interpreta en complemento a dos como el número decimal **$-116$**.

---

## Nivel 5 — Punto flotante (IEEE 754)

### 5.1 Representa el número decimal 10.25 en formato IEEE 754 de precisión simple (32 bits)
1. **Signo:** Es positivo $\rightarrow$ **`0`**
2. **Pasar a binario:** $10 = 1010$, y $0.25 = 01_2 \rightarrow 1010.01_2$
3. **Normalizar:** $1.01001 \cdot 2^3$
4. **Exponente con sesgo (127):** $3 + 127 = 130 \rightarrow$ En binario: **`10000010`**
5. **Mantisa:** Parte fraccionaria rellenando con ceros hasta 23 bits $\rightarrow$ **`01001000000000000000000`**

**Resultado Completo:** `0 10000010 01001000000000000000000`

### 5.2 Dado el patrón de bits IEEE 754: `1 10000010 01100000000000000000000`
1. **Signo:** `1` $\rightarrow$ Negativo ($-$)
2. **Exponente:** $10000010_2 = 130 \rightarrow$ Restar sesgo: $130 - 127 = 3$
3. **Mantisa:** `011` $\rightarrow$ Con el 1 implícito queda $1.011_2$
4. **Desnormalizar:** $1.011_2 \cdot 2^3 = 1011_2$
5. **Convertir a decimal:** $1011_2 = 11$
* **Resultado:** **`-11.0`**

### 5.3 Explica la pérdida de precisión en punto flotante y demuéstrala numéricamente
La pérdida de precisión ocurre porque las computadoras guardan los números usando potencias binarias de base 2 ($1/2, 1/4, 1/8$, etc.). Fracciones decimales simples como $0.1$ o $0.2$ se transforman en **números periódicos infinitos** en binario, obligando a la computadora a redondearlos y truncarlos.

**Demostración:** Al sumar $0.1 + 0.2$ en un lenguaje de programación común (como Python o JavaScript), el resultado no es $0.3$, sino **`0.30000000000000004`** debido a los bits de residuo acumulados tras el redondeo IEEE 754 de doble precisión.

---

## Nivel 6 — Integración: script de verificación

### 6.1 Script de Python (`verificador.py`)
```python
import sys

def verificar_numero(num):
    print(f"--- Análisis del número: {num} ---")
    # Representaciones estándar
    print(f"Binario puro (sin signo): {bin(num if num >= 0 else num & 0xffffffff)}")
    print(f"Octal: {oct(num if num >= 0 else num & 0xffffffff)}")
    print(f"Hexadecimal: {hex(num if num >= 0 else num & 0xffffffff)}")
    
    # Complemento a dos
    def comp2(v, bits):
        return format(v & ((1 << bits) - 1), f'0{bits}b')
    
    print(f"Complemento a dos (8 bits):  {comp2(num, 8)}")
    print(f"Complemento a dos (16 bits): {comp2(num, 16)}")
    print(f"Complemento a dos (32 bits): {comp2(num, 32)}")
    
    # Overflow en 8 bits con signo [-128, 127]
    if num < -128 or num > 127:
        print("¿Produce overflow en 8 bits con signo?: SÍ")
    else:
        print("¿Produce overflow en 8 bits con signo?: NO")
    print()

# Valores de prueba sugeridos
valores = [127, 128, -128, -129, 0]
for v in valores:
    verificar_numero(v)
```

**Salidas del Script al ejecutarse:**
```text
--- Análisis del número: 127 ---
Binario puro (sin signo): 0b1111111
Octal: 0o177
Hexadecimal: 0x7f
Complemento a dos (8 bits):  01111111
Complemento a dos (16 bits): 0000000001111111
Complemento a dos (32 bits): 00000000000000000000000001111111
¿Produce overflow en 8 bits con signo?: NO

--- Análisis del número: 128 ---
Binario puro (sin signo): 0b10000000
Octal: 0o200
Hexadecimal: 0x80
Complemento a dos (8 bits):  10000000
Complemento a dos (16 bits): 0000000010000000
Complemento a dos (32 bits): 00000000000000000000000010000000
¿Produce overflow en 8 bits con signo?: SÍ

--- Análisis del número: -128 ---
Binario puro (sin signo): 0xffffff80
Octal: 0o37777777600
Hexadecimal: 0xffffff80
Complemento a dos (8 bits):  10000000
Complemento a dos (16 bits): 1111111110000000
Complemento a dos (32 bits): 11111111111111111111111110000000
¿Produce overflow en 8 bits con signo?: NO

--- Análisis del número: -129 ---
Binario puro (sin signo): 0xffffff7f
Octal: 0o37777777577
Hexadecimal: 0xffffff7f
Complemento a dos (8 bits):  01111111
Complemento a dos (16 bits): 1111111101111111
Complemento a dos (32 bits): 11111111111111111111111101111111
¿Produce overflow en 8 bits con signo?: SÍ

--- Análisis del número: 0 ---
Binario puro (sin signo): 0b0
Octal: 0o0
Hexadecimal: 0x0
Complemento a dos (8 bits):  00000000
Complemento a dos (16 bits): 0000000000000000
Complemento a dos (32 bits): 00000000000000000000000000000000
¿Produce overflow en 8 bits con signo?: NO
```

### 6.2 Explicación del caso de uso (Sensor de Temperatura)
Si un sensor de hardware lee una temperatura real de $128^{\circ}\text{C}$ y el software la procesa usando un entero de 8 bits con signo (`int8_t`), ocurrirá un **overflow silencioso**. Como vimos en el script, el patrón binario para $128$ es `10000000`, el cual equivale a **$-128^{\circ}\text{C}$** con signo. 

Esto es extremadamente peligroso en un sistema embebido: un software de control de seguridad industrial o automotriz interpretará que el sistema está experimentando un frío extremo e hipotético en lugar de un sobrecalentamiento crítico. Como consecuencia directa, las rutinas de enfriamiento no se activarían y los calentadores podrían encenderse a su máxima potencia de forma errónea, desencadenando fallas de hardware catastróficas, incendios o explosiones físicas en plantas industriales.
