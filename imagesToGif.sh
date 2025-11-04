ffmpeg -framerate 24 -start_number 1 -i ./outputs/video/image_%04d.png \
-filter_complex "scale=360:-1:flags=lanczos,split[s0][s1];[s0]palettegen[p];[s1][p]paletteuse" \
-loop 0 ./outputs/output.gif
