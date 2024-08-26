CC = gcc
CFLAGS = -Wall 

#all: aplicacion esclavo vista
all: esclavo

#aplicacion: aplicacion.c 
#	$(CC) $(CFLAGS) aplicacion.c -o md5

#vista: vista.c lib.c ipc_utils.c
#	$(CC) $(CFLAGS) vista.c -o vista

esclavo: esclavo.c lib.c
	$(CC) $(CFLAGS) esclavo.c -o esclavo

clean:
#	rm -f vista esclavo md5
	rm -f esclavo