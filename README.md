# Assembler

A custom two-pass assembler written in standard C (ANSI-C). This project simulates an assembler for a fictional 12-bit machine language. It reads assembly source files (.as), parses the instructions, expands macros, and translates the code into machine code output files.

This project was developed as part of the course "Systems Programming Lab".

## 🚀 Features

* **Macro Expansion (Pre-Assembler):** Automatically identifies and expands macros, outputting a clean .am file.
* **Two-Pass Architecture:**
    * **First Pass:** Builds a robust Symbol Table, counts Instruction Counter (IC) and Data Counter (DC), and encodes primary instruction words.
    * **Second Pass:** Resolves forward references and completes the binary encoding for labels.
* **12-bit Memory Words:** Implements bitwise operations to correctly construct 12-bit machine words, including A.R.E (Absolute, Relocatable, External) encoding.
* **Addressing Modes:** Supports Immediate (#), Direct (Labels), Relative (%), and Register (r0-r7) addressing modes.
* **Error Handling:** Extensive validation for syntax errors, undefined symbols, illegal addressing modes, and memory overflow (MAX_RAM = 4096).
* **Dynamic Memory Management:** Uses efficient Linked Lists for the Symbol Table, Externals List, and Macro caching.

## 📁 Project Structure

The project uses a flat directory structure, dividing logic into modular components:

| File | Description |
|------|-------------|
| `assembler.c` | Main application entry point. Orchestrates the 3 phases. |
| `preAssembler.c/h` | Handles the macro expansion phase. |
| `firstPass.c/h` | First pass logic: symbol table construction & initial encoding. |
| `secondPass.c/h` | Second pass logic: resolving label addresses. |
| `table.c/h` | Linked-list implementations for Symbols and Externals. |
| `utils.c/h` | String manipulation, parsing, and validation helpers. |
| `outputFiles.c/h` | Generates the `.ob`, `.ent`, and `.ext` files. |
| `globals.h` | System-wide constants, bit-masks, and structs. |

## 🛠️ Build Instructions

The project includes a `makefile` for easy compilation using `gcc`. To build the project, simply run:

```bash
make
