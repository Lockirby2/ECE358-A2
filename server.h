#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <iostream>
#include <string>

using namespace std;

class Server{
private:
	int port;
	string folder;
public:
	Server(int port, string folder);
	~Server();
	void Run();
};