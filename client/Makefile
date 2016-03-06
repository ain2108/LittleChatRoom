CC=gcc

INCLUDES= -Iincludes
CCFLAGS= -g -Wall $(INCLUDES)
LDFLAGS= -lcrypto
SOURCES=$(wildcard src/*.c)
OBJECTS=$(SOURCES:.c=.o)
TEMP=$(OBJECTS:src/%.o=%.o)
TARGET=test

all: $(TARGET)
	mkdir -p object_files
	mv *.o object_files

$(TARGET): $(OBJECTS)
	$(CC) -o $@ $(TEMP) $(LDFLAGS)

%.o: %.c %.h
	$(CC) $(CCFLAGS) -c $<

%.o: %.c
	$(CC) $(CCFLAGS) -c $<

clean:
	rm -f object_files/*.o $(TARGET)
