#include "UDPClient.h"

using namespace std;

char* UDPClient::get_ip() {

    char *ip = 0;

    struct ifaddrs *ifa;
    if (getifaddrs(&ifa) < 0) {
        perror("could not get host addresses");
        exit(1);
    }

    struct in_addr srv_ip;
    for (struct ifaddrs *i = ifa; i != NULL; i = i->ifa_next) {
        if (i->ifa_addr == NULL) {
            continue;
        }

        if (i->ifa_addr->sa_family == AF_INET) {
            memcpy(&srv_ip, &(((struct sockaddr_in *)(i->ifa_addr))->sin_addr),
                   sizeof(struct in_addr));
            ip = inet_ntoa(srv_ip);
			string str(ip);
			if (!(str=="127.0.0.1"))
            	break;
        }
    }

    freeifaddrs(ifa);
    return ip;
}

int UDPClient::read_length(int socket, int &length){
    int status = recv(socket, &length, sizeof length, 0);
    if (status == 0) {
        //pending_remove.push_back(socket);
        return 0;
    } else if (status < 0){
        cerr << "recv failed" << endl;
        return - 1;
    }
    return 0;
}

int UDPClient::read_message(int socket, int length, string &message){
    char* msg = new char[length];
    int status = recv(socket, msg, length, 0);
    if (status < 0) {
        cerr << "recv failed" << endl;
        return -1;
    }
    string str_msg(msg);
    message = str_msg;
    delete[] msg;
    return 0;
}

int UDPClient::send_length(int socket, int length){
    int temp_len = length + 1;
    int status = send(socket, &temp_len, sizeof temp_len, 0);
    if (status < 0) {
        cerr << "send failed" << endl;
        return -1;
    }
    return 0;
}

int UDPClient::send_message(int socket, int length, string message ){
    int status = send(socket, message.c_str(), length + 1, 0);
    if (status < 0) {
        cerr << "send failed" << endl;
        return -1;
    }
    return 0;
}


int UDPClient::connect_to_server(char *hostname, int port)
{
    int client_fd = socket(AF_INET, SOCK_STREAM, 0);
    int temp_sock;

    struct sockaddr_in remote_addr;
    remote_addr.sin_family = AF_INET;
    remote_addr.sin_port = htons(port);

    struct hostent* server;
    server = gethostbyname(hostname);
    cout << server->h_name << endl;
    if(server == NULL)
    {
        close(client_fd);
        exit(1);
    }
    memcpy(&temp_sock, server->h_addr, server->h_length);
    cout << temp_sock << endl;
    memcpy(&remote_addr.sin_addr, &temp_sock, sizeof(int));

    if(connect(client_fd, (struct sockaddr *)&remote_addr, sizeof(remote_addr)) < 0)
    {
        cerr << "connect failed" << endl;
        close(client_fd);
    }
    return client_fd;
}

void UDPClient::send_to_socket(int socket, string message){
    int length = message.size();
    if (send_length(socket, length) < 0){
        cerr << "send_length failed" << endl;
        exit(1);
    }

    if (send_message(socket, length, message) < 0){
        cerr << "send_message failed" << endl;
        exit(1);
    }
    //cout << "Send: " << message << endl;
}

void UDPClient::receive_from_socket(int socket, string &message){
    int length = 0;
    if (read_length(socket, length) < 0){
        cerr << "read_length failed" << endl;
        exit(1);
    }

    if (read_message(socket, length, message) < 0){
        cerr << "read_message failed" << endl;
        exit(1);
    }
}



int UDPClient::create_server_socket()
{
    int sockfd = -1;
    char *ip = get_ip();
    cout << "ip: " << ip << endl;

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cerr << "socket create failed" << endl;
        return -1;
    }

    struct sockaddr_in sin;

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    inet_aton(ip, (struct in_addr *)&sin.sin_addr.s_addr);
    //sin.sin_port = htons(0);

    unsigned short port;
    for (port = 10000; port <= 11000; port++) {
        sin.sin_port = htons(port);
        if(::bind(sockfd, (struct sockaddr*)&sin, sizeof(struct sockaddr_in)) >= 0)
        {
            break;
        }
    }

    if (port > 11000) {
        cerr << "Port max exceeded" << endl;
        exit(1);
    }

    if(listen(sockfd, 5) < 0)
    {
        cerr << "listen failed" << endl;
        close(sockfd);
        exit(1);
    }
    cout << "port: " << port << endl;

    return sockfd;
}

UDPClient::UDPClient(char* targetIp, int targetPort) {
    this->targetIp = targetIp;
    this->targetPort = targetPort;
}

void UDPClient::run() {
//    int server = create_server_socket();
    int sock = connect_to_server(targetIp, targetPort);
    cout << "Huh... stuffs.... working?" << endl;
    send_to_socket(sock, "");
    //TODO listen for response
}