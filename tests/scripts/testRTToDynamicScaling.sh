core_splits=("10rt2d" "10rt1d" "8rt4d" "8rt2d" "8rt1d" "6rt6d" "6rt4d" "6rt2d" "6rt1d" "4rt8d" "4rt4d" "4rt2d" "4rt1d" "2rt10d" "2rt8d" "2rt6d" "2rt4d" "2rt2d" "1rt1d" "1rt2d" "1rt4d" "1rt6d" "1rt8d" "1rt10d")
#core_splits=("10rt2d")
for c_s in "${core_splits[@]}"
do
    for i in $(seq 1 9);
    do
        for j in $(seq 1 5)
        do
            n=$((2 ** i )) #number of bodies
            k=$((5*(j ** 2))) #rt to dyn scaling
            echo "$n spheres, $c_s core split, $k ratio"
            ./tests/scripts/rtToDynamicRatio.sh $n $c_s $k
        done
    done
done
