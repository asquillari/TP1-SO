CC = gcc
CFLAGS = -Wall 
SOURCES_ADT=$(wildcard ADTs/*.c)
OBJECTS_ADT=$(SOURCES_ADT:.c=.o)
all: $(BINARIES) $(OBJECTS_ADT)

$(OBJECTS_ADT): $(SOURCES_ADT)
	cd ADTs; make all

all: app slave view

app: app.c $(OBJECTS_ADT)
	$(CC) $(CFLAGS) app.c $(OBJECTS_ADT) -o md5

view: view.c $(OBJECTS_ADT)
	$(CC) $(CFLAGS) view.c $(OBJECTS_ADT) -o view

slave: slave.c $(OBJECTS_ADT)
	$(CC) $(CFLAGS) slave.c $(OBJECTS_ADT) -o slave



clean:
	rm -f view slave md5 result.txt *.o ADTs/*.o