all: build

build:
	gcc server.c -o server
	gcc client.c -o client

clean:
	rm -rf server client

.PHONY: clean build all