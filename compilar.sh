#!/bin/bash
echo "Compilando" 

make clean
make -j$(nproc)
