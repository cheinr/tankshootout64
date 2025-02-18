# Not really a script; just one off commands I'm keeping here in case I
# need them again


# crop several files of size 74x74 to 64x64 in a way that maximizes tank space
for filename in ./*.png; do convert -crop 64x64+10+10 $filename out/$filename; done

montage *.png -background 'rgba(0,0,0,0)' -tile 9x10 -geometry +0+0 ../../combined-shifted.png
