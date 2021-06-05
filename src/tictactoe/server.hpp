#ifndef SERVER_H
#define SERVER_H

#include <thread>

class ServerConnector
{
	public:
		ServerConnector(int socket);
		~ServerConnector();
		void update();
		bool is_connected();

	private:
		int socket;
		char *get_buffer;
		char *send_buffer;
		bool connected;
		std::thread update_thread;
};

#endif
