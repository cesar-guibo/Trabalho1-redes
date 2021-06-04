CC := gcc

CC_FLAGS := -Wall -fsanitize=address -g

all: ./bin
	$(CC) -o ./bin/client src/client.c $(CC_FLAGS)

./bin: 
	mkdir ./bin 

test_client:
	netcat -l -p 8080&
	./bin/client 127.0.0.1 8080

run:
	./client

clean:
	rm -rf programa bin *.bin
