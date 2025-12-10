sphere_count=$1
core_split=$2
output_root="$(pwd)/tests/outputs/"
script_output_path="${output_root}/${sphere_count}b_100k"
mkdir -p "$(pwd)/tests/inputs/bodies/"
generated_input_path="$(pwd)/tests/inputs/bodies/gen${sphere_count}_in"
mkdir -p "${output_root}"
{
    ./cmake-build-release/input-generator $sphere_count $generated_input_path && \
    mpiexec \
        -n 12 ./cmake-build-release/parallel-planets \
        -i $generated_input_path \
        -o $output_root \
        -n 60000 \
        -h 0.01 \
        -r "$(pwd)/tests/inputs/core_splits/${core_split}" \
        -f 1 \
        -k 100 \
        -v 360
} 2>&1 | tee "${script_output_path}${core_split}.txt"
