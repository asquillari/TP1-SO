CC = gcc
CFLAGS = -Wall 

#all: app slave view
all: slave app

app: app.c app_lib.c
	$(CC) $(CFLAGS) app.c app_lib.c -o md5

#view: view.c
#	$(CC) $(CFLAGS) view.c -o view

slave: slave.c 
	$(CC) $(CFLAGS) slave.c -o slave


clean:
#	rm -f view slave md5
	rm -f slave md5