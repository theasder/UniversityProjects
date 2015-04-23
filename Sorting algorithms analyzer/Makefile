all:
	nasm -f elf32 -o module.o module.asm
	gcc -o main.o -c main.c
	gcc -o prog module.o main.o

