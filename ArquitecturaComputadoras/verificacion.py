def imprimir_binario(numero):
    print(f"Binario de {numero} = {bin(numero)}")

def imprimir_octal(numero):
    print(f"Octal de {numero} = {oct(numero)}")

def imprimir_hexadecimal(numero):
    print(f"Hexadecimal de {numero} = {hex(numero)}")


def commplemento_dos(numero, ancho):
    if numero < 0:
        valor_c2 = numero + (2 ** ancho)
    else:
        valor_c2 = numero
    return valor_c2



if __name__ == "__main__":
    entero = int(input("Ingrese un numero entero: "))
    imprimir_binario(entero)
    imprimir_octal(entero)
    imprimir_hexadecimal(entero)
    print("Complemento a 2: ")
    entero_c2 = commplemento_dos(entero, 8)
    imprimir_binario(entero_c2)
    entero_c2 = commplemento_dos(entero, 16)
    imprimir_binario(entero_c2)
    entero_c2 = commplemento_dos(entero, 32)
    imprimir_binario(entero_c2)