CC=clang
LIBS=-lssl -lcrypto -lpthread

all: ssl-server

ssl-server: ssl_server.c
	$(CC) ssl_server.c -o ssl-server $(LIBS)

clean:
	rm *.o ssl-server
