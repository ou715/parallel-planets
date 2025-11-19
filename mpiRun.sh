mpiexec -n "$2" ./cmake-build-release/parallel-planets -i "$(pwd)/inputs/orig_in" -o "$(pwd)/outputs/" -n 30000 -h 0.01 -r "./inputs/mpi_config/core_split"
