LDFLAGS = -lm

all:
	make server
	make client

server: server.o
	g++ -std=c++1z -ggdb -lpthread -o $@ $^ $(LDFLAGS)

client: client.o
	g++ -std=c++1z -ggdb -lpthread -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(wildcard *.o) server client
