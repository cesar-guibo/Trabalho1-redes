all: run

run:
	#g++ -g server.cpp -std=c++17 -pthread -o server -fsanitize=address
	g++ -g src/server.cpp -std=c++0x -lpthread -o server
	./server 127.0.0.1 1338

debug:
	g++ -g src/server.cpp -pthread -o server
	gdb --args server 127.0.0.1 1338
