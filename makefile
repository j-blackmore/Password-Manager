CC=gcc
CFLAGS=-g -Wall -std=c99
DEPS = file-hanlder.h login.h

%.o: %.c $(DEPS)
	$(CC) -c -o $@ $^ $(CFLAGS)

all: password-manager.o file-handler.o login.o
	$(CC) -o password-manager password-manager.o file-handler.o login.o