#!/bin/bash
echo "=========================================="
echo "    INICIANDO NAKA CORE ($NKC) - MINERO"
echo "=========================================="
mkdir -p build
cd build
cmake ..
make -j$(nproc)
echo "[+] Compilación exitosa. Arrancando nodo minero..."
./nakacored
