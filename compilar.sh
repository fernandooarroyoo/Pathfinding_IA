#!/bin/bash
#SI pones argumento se lanza tambien el programa, si no le pones nada solo limpia y recompila
echo "Compilando" 

make clean
make -j$(nproc)

if [ $# -ne 0 ]; then
  echo "Lanzando programa"
  ./practica2 -m mapas/mapa30.map -n 0 -i 17 5 0 -t 17 17 0
fi