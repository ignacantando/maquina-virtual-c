<h1 align="center">🖥️ Máquina Virtual en C</h1>

<p align="center">
  Intérprete de un <b>lenguaje máquina propio</b> escrito en <b>C</b>: carga programas binarios, los ejecuta instrucción por instrucción sobre memoria segmentada y los puede desensamblar o pausar en un <i>breakpoint</i>.
</p>

<p align="center">
  <img src="https://img.shields.io/badge/C-A8B9CC?style=for-the-badge&logo=c&logoColor=black" alt="C" />
  <img src="https://img.shields.io/badge/GCC-compatible-1f6f4a?style=for-the-badge" alt="GCC" />
  <img src="https://img.shields.io/badge/UNMdP-Arquitectura%20de%20Computadoras-1f6f4a?style=for-the-badge" alt="UNMdP Arquitectura de Computadoras" />
</p>

---

## 📌 Sobre el proyecto

Trabajo práctico de **Arquitectura de Computadoras** (Ingeniería Informática, UNMdP, 2024). Implementa una máquina virtual que simula un procesador con **registros, memoria segmentada, pila y llamadas al sistema**. Ejecuta los programas binarios `.vmx` que genera el traductor de la cátedra.

Se hizo en dos entregas: la v1 con un único segmento de código y datos, y la v2 con segmentos configurables, pila, subrutinas, *strings*, imágenes de memoria y depuración.

## ✨ Características

- 🧠 **CPU simulada** con 16 registros: `EAX`–`EFX` de propósito general (con acceso a `AX`, `AL`, `AH`), `IP`, `SP`, `BP`, `CC`, `AC` y registros de segmento
- 🧱 **Memoria segmentada** de hasta 16 KiB, con tabla de descriptores y segmentos **Code, Data, Extra, Stack y Const**
- ⚙️ **Instrucciones:**
  - Aritméticas: `ADD`, `SUB`, `MUL`, `DIV`
  - Lógicas: `AND`, `OR`, `XOR`, `NOT`
  - Desplazamiento: `SHL`, `SHR`
  - Comparación: `CMP`
  - Movimiento: `MOV`, `SWAP`, `LDH`, `LDL`
  - Saltos condicionales: `JMP`, `JZ`, `JP`, `JN`, `JNZ`, `JNP`, `JNN`
  - Pila y subrutinas: `PUSH`, `POP`, `CALL`, `RET`
  - Otras: `RND`, `SYS`, `STOP`
- 🎯 **Operandos** de registro, inmediato y memoria (`b[...]`, `w[...]`, `l[...]` para 1, 2 o 4 bytes)
- 📟 **Llamadas al sistema** (`SYS`) para leer y escribir valores en decimal, hexadecimal, octal o carácter, para leer y escribir *strings*, y para limpiar la pantalla
- 🔍 **Desensamblador** (`-d`): muestra cada instrucción con su dirección, sus bytes y su mnemónico
- 🐞 **Breakpoints** (`SYS F`): guardan una **imagen de memoria `.vmi`** y esperan un comando (`g` continúa, `q` sale, *Enter* avanza un paso)
- 💾 **Imágenes `.vmi`**: permiten retomar la ejecución desde el estado guardado

## 🔍 Ejemplo

`sample1.vmx` lee una palabra, la invierte y la pasa a mayúsculas:

```text
$ ./vmx sample1.vmx
rumpelstiltskin
NIKSTLITSLEPMUR
```

Con `-d`, el desensamblador muestra el programa:

```text
[001D] 00 CB 00 00 CA 00 00 | MOV b[EAX+0],b[EBX+0]
[0024] 49 00 DF CA 00 00    | AND b[EAX+0],DF
[002A] 61 00 01 0A          | ADD EAX,1
[002E] 62 00 01 0B          | SUB EBX,1
[0032] A6 0D 0B             | CMP EBX,EDX
[0035] 77 00 00             | JNN 0
```

## 📁 Estructura

```
├── main.c          # carga de .vmx/.vmi, segmentos, ciclo fetch-decode-execute, breakpoints
├── Funciones.h     # registros, memoria e implementación de cada instrucción y SYS
├── disasembler.h   # desensamblador
└── sample*.vmx     # programas de ejemplo (sample3.vmi es una imagen de memoria)
```

## 🚀 Cómo compilar y ejecutar

```bash
gcc -o vmx main.c
./vmx programa.vmx              # ejecutar
./vmx programa.vmx -d           # ejecutar mostrando el desensamblado
./vmx programa.vmx imagen.vmi   # habilitar breakpoints, guardando la imagen en imagen.vmi
./vmx imagen.vmi                # retomar desde una imagen de memoria
```

> Se desarrolló en Windows: la limpieza de pantalla usa `cls`.

## 👥 Autores

- **Ignacio Cantando Flego** · [GitHub](https://github.com/ignacantando)
- **Gonzalo León** · [GitHub](https://github.com/GonzaloLeon01)
