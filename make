#~ /usr/bin/bash

input=main.c
output=game
file=build
LIBS="-lraylib -lm"

if [ ! -d "./$file" ]; then
    mkdir ./$file
fi

clang $input -o ./$file/$output $LIBS
./$file/$output