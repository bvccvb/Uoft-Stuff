LDFLAGS = -lm

all:
	make server
	make deliver

server: server.o
	cc -o $@ $^ $(LDFLAGS)

deliver: deliver.o
	cc -o $@ $^ $(LDFLAGS)

.PHONY: clean
clean:
	rm -f $(wildcard *.o) server deliver
