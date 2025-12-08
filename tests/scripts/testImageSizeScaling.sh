for i in $(seq 3 40);
do
    v=$((i * 60)) #rt to dyn scaling
    echo "8 spheres, 8rt2d core split, 50 ratio, image height $v"
    ./tests/scripts/imageSize.sh 8 8rt2d 50 $v
done
