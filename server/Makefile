CC=gcc

INCLUDES= -Iincludes
CCFLAGS= -g -Wall $(INCLUDES)
LDFLAGS=
SOURCES=$(wildcard src/*.c)
OBJECTS=$(SOURCES:.c=.o)
TEMP=$(OBJECTS:src/%.o=%.o)
TARGET=Server

all: $(TARGET)
	mkdir -p object_files
	mkdir -p tmp
	mv *.o object_files
	rm -f tmp/*.pipe

$(TARGET): $(OBJECTS)
	$(CC) -o $@ $(TEMP) $(LDFLAGS)

%.o: %.c %.h
	$(CC) $(CCFLAGS) -c $<

%.o: %.c
	$(CC) $(CCFLAGS) -c $<

clean:
	rm -f tmp/*.pipe
	rm -f object_files/*.o $(TARGET)
