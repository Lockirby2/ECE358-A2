#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cstring>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <sstream>
#include <unordered_map>
#include <utility>
#include <tuple>
#include <arpa/inet.h>
#include <ifaddrs.h>
#include <map>
#include "message.h"
#include "TCB.h"

using namespace std;

class UDPServer{
public:
	UDPServer(int port, string server);
	~UDPServer(){};

	map<int, TCB> connections;

    void run();

    bool handle_msg(int client, const char *msg);
    void stop();

    int get_client_num(sockaddr_in source_addr);
    int remove_client(sockaddr_in source_addr);

    int get_server_port();
    string get_server_address();

    char* get_ip();
    int create_server_socket(int port);

    void send_message(Message msg, int socket);

public:
    int server;
    sockaddr_in socket_address_;
    std::vector<sockaddr_in> clients_;
    int port;
	string folder;

};