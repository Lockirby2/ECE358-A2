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

using namespace std;

class Proxy{
public:
    void receive_from_socket(int socket, char* message);
    int read_length(int socket, int &length);
    int read_message(int socket, int length, char* message);
    char* get_ip();
    int create_server_socket(int port);
    int server;
};