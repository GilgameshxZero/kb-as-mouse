# Conversion using imagemagick,
convert -background none -density 8192 $1.svg \( -clone 0 -resize 16x16 \) \( -clone 0 -resize 24x24 \) \( -clone 0 -resize 32x32 \) \( -clone 0 -resize 48x48 \) \( -clone 0 -resize 64x64 \) \( -clone 0 -resize 96x96 \) \( -clone 0 -resize 128x128 \) \( -clone 0 -resize 192x192 \) \( -clone 0 -resize 256x256 \) -delete 0 $1.ico
