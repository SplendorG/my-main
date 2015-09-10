#!/bin/sh

gcc -I -Wall ./server.h ./ser_main.c ./server.c -lib pthread -o ser.out
