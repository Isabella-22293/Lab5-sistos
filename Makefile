CC = gcc
CFLAGS = -Wall -pthread -I src
SRC = src/semaforos.c src/monitor.c
BIN = bin/semaforos bin/monitor

all: bin prog_sem prog_mon

bin:
	mkdir -p bin

prog_sem: src/compartido.h src/semaforos.c
	$(CC) $(CFLAGS) src/semaforos.c -o bin/semaforos

prog_mon: src/compartido.h src/monitor.c
	$(CC) $(CFLAGS) src/monitor.c -o bin/monitor

clean:
	rm -rf bin logs

.PHONY: all clean
