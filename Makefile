CC = gcc
LIB = -lrt

main: main.o init.o util.o taskmgr.o stat.o scaling.o reliability.o
	$(CC) main.o init.o util.o taskmgr.o stat.o scaling.o reliability.o $(LIB) -o $@

stat.o:
	$(CC) -c stat.c -o $@

taskmgr.o:
	$(CC) -c taskmgr.c -o $@

util.o:
	$(CC) -c util.c -o $@

main.o:
	$(CC) -c main.c -o $@

init.o: 
	$(CC) -c init.c -o $@

scaling.o:
	$(CC) -c scaling.c -o $@

reliability.o:
	$(CC) -c reliability.c -o $@

clean:
	rm main main.o init.o util.o taskmgr.o stat.o scaling.o reliability.o
