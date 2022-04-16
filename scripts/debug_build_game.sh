#! /bin/bash

if [ ! -d "build" ]
then
    mkdir build
fi

gcc -g -c -Wall -Werror -Wuninitialized -fpic src/game.c -o build/game.o -pg

gcc -g -shared -o build/libgame.so build/game.o -pg

