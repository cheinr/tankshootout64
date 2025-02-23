# Not really a script; just one off commands I'm keeping here in case I
# need them again


# crop several files of size 74x74 to 64x64 in a way that maximizes tank space
for filename in ./*.png; do convert +repage -crop 64x64+5+5 $filename out/$filename; done

for filename in ./*.png; do convert +repage -crop 64x64+5+5 $filename 64x64/$filename; done

for filename in ./*.png; do \
    echo "filename: $filename"
    convert +repage -crop 32x64+0+0 $filename 32x64/$filename-0.png; \
    convert +repage -crop 32x64+32+0 $filename 32x64/$filename-1.png; \
done

for filename in ./*.png; do convert +repage -crop 32x32+31+31 $filename out/$filename; done

montage *.png -background 'rgba(0,0,0,0)' -tile 9x10 -geometry +0+0 ../../combined-shifted.png

