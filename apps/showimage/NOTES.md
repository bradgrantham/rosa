for i in *.jpg ; do echo $i ; djpeg $i | ppmquant 256 | ppmtobmp -bpp 8 > ~/trees/rosa/emu-root/slideshow/${i%.*}.bmp ; done
