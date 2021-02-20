LDFLAGS = -lm

all:
	make server
	make deliver

server: server.o
	cc -Ofast -march=native -o $@ $^ $(LDFLAGS)

deliver: deliver.o
	cc -Ofast -march=native -o  $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(wildcard *.o) server deliver
