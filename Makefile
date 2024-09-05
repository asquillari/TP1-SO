CC = gcc
CFLAGS = -Wall 
SOURCES_ADT=$(wildcard ADTs/*.c)
OBJECTS_ADT=$(SOURCES_ADT:.c=.o)
all: $(BINARIES) $(OBJECTS_ADT)

$(OBJECTS_ADT): $(SOURCES_ADT)
	cd ADTs; make all

#all: app slave view
all: slave app

app: app.c app_lib.c $(OBJECTS_ADT)
	$(CC) $(CFLAGS) app.c app_lib.c $(OBJECTS_ADT) -o md5

#view: view.c
#	$(CC) $(CFLAGS) view.c -o view

slave: slave.c 
	$(CC) $(CFLAGS) slave.c -o slave



clean:
#	rm -f view slave md5
	rm -f slave md5 result.txt