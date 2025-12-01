mpiexec -n "$2" ./cmake-build-release/parallel-planets -i "$(pwd)/inputs/gen8_in" -o "$(pwd)/outputs/" -n 6000 -h 0.03 -r "./inputs/mpi_config/core_split2" -f 1 -k 100
