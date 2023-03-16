all: build

build:
	gcc server.c -o bin/server
	gcc client.c -o bin/client

clean:
	rm -rf bin/server bin/client

.PHONY: clean build all