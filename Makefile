CC = gcc
CFLAGS = -Wall 

#all: aplicacion esclavo vista
all: esclavo

#aplicacion: aplicacion.c 
#	$(CC) $(CFLAGS) aplicacion.c -o md5

#vista: vista.c
#	$(CC) $(CFLAGS) vista.c -o vista

esclavo: esclavo.c 
	$(CC) $(CFLAGS) esclavo.c -o esclavo

clean:
#	rm -f vista esclavo md5
	rm -f esclavo