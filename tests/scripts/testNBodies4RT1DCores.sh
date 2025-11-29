sphere_count=$1 \
generated_input_path="$(pwd)/tests/inputs/gen${sphere_count}_in"
{
    ./cmake-build-release/input-generator $sphere_count $generated_input_path && \
    mpiexec \
        -n 5 ./cmake-build-release/parallel-planets \
        -i $generated_input_path \
        -o "$(pwd)/tests/outputs/" \
        -n 60000 \
        -h 0.01 \
        -r "$(pwd)/tests/inputs/core_splits/4rt1d"
} 2>&1 | tee "$(pwd)/tests/outputs/${sphere_count}b4rt1d.txt"
