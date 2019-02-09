CC = gcc
CCFLAGS = -W -Wall

BINARY = boiler-controller-web-server

all:
	$(CC) $(CCFLAGS) -IIncludes Sources/Boiler.c Sources/Main.c -lmicrohttpd -o $(BINARY)

clean:
	rm -f $(BINARY)
