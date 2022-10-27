#!/bin/sh
set -x

FONT_PREFIX='lib/MAKERphone/src/TFT_eSPI/Fonts/GFXFF'
RANGES='12353:12435'

./fontconvert/fontconvert fontconvert/jp_font.otf 9 "$RANGES" > ${FONT_PREFIX}/hiragana9.h
./fontconvert/fontconvert fontconvert/jp_font.otf 12 "$RANGES" > ${FONT_PREFIX}/hiragana12.h
