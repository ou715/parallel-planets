mpiexec \
    -n "$2" ./cmake-build-release/parallel-planets \
    -sphere_input_file "$(pwd)/inputs/gen8_in" \
    -output_root_path "$(pwd)/outputs/" \
    -n_steps 6000 \
    -step_size 0.03 \
    -ranks_file "./inputs/mpi_config/core_split2" \
    -file_output_flag 1 \
    -render_step_ratio 100 \
    -image_height 360
