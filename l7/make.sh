#!/bin/sh
gcc master.c -o master -lpvm3 -lm -std=gnu99
gcc slave.c -o slave -lpvm3 -lm -std=gnu99
gcc slave2.c -o slave2 -lpvm3 -lm -std=gnu99
gcc hello.c -o hello -lpvm3 -lm -std=gnu99

cp master slave slave2 hello $HOME/pvm3/bin/LINUX64/

