#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <iterator>
#include <iostream>
#include <fstream>
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
#include <string>
//#include "message.h"
#include "checkSumUtil.h"

typedef unsigned char BYTE;

using namespace std;

class UDPClient{
    int port;
    int server;
    int targetPort;
    char* targetIp;
public:
    UDPClient(char* targetIP, int targetPort, int port);
    void run();
    void receive_from_socket(int socket, string &message);
    void send_to_socket(int socket, string message);
    int connect_to_server(char *hostname, int port);
    int read_length(int socket, int &length);
    int read_message(int socket, int length, string &message);
    int send_length(int socket, int length);
//    int send_message(int socket, int length, string message );
    int create_server_socket();
    void send_message(Message msg, sockaddr_in);
    void print_message(Message msg);
    char* get_ip();
};