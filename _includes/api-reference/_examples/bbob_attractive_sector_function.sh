#!/bin/sh
if [[ "$(uname -o)" = "Msys" ]]; then
  c++ -std=c++11 bbob_attractive_sector_function.cpp -LC:/Programme/OpenBLAS/lib -lopenblas -IC:/Programme/Armadillo/include -IC:/Programme/Mantella/include -o bbob_attractive_sector_function
  ./bbob_attractive_sector_function.exe
  rm bbob_attractive_sector_function.exe
else
  ./bbob_attractive_sector_function.core.sh
  rm bbob_attractive_sector_function
fi

rm bbob_attractive_sector_function_surface.png bbob_attractive_sector_function_contour.png 2> /dev/null 
matlab -nodisplay -nosplash -nodesktop -r "run('./bbob_attractive_sector_function.m');exit;"

while [[ ! -s bbob_attractive_sector_function_surface.png || ! -s bbob_attractive_sector_function_contour.png ]]; do
  sleep 2
done
sleep 5

rm X.mat Y.mat Z.mat
mv bbob_attractive_sector_function_surface.png bbob_attractive_sector_function_contour.png ../../../assets/images/api-reference/