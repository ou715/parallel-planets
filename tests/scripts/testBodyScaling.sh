for i in $(seq 1 80);
do
    n=$((4*i))
    ./tests/scripts/bodyScaling.sh $n 4rt4d
done
