#include "proxy.h"

using namespace std;


void Proxy::receive_from_socket(int socket, char* message){
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

int Proxy::read_length(int socket, int &length){
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

int Proxy::read_message(int socket, int length, char* message){
    message = new char[length];
    int status = recv(socket, message, length, 0);
    if (status < 0) {
        cerr << "recv failed" << endl;
        return -1;
    }
    return 0;
}

char* Proxy::get_ip() {
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


int Proxy::create_server_socket(int port)
{
    int sockfd = -1;
    char *ip = get_ip();

    if((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        cerr << "socket create failed" << endl;
        return -1;
    }

    struct sockaddr_in sin;

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    inet_aton(ip, (struct in_addr *)&sin.sin_addr.s_addr);
    sin.sin_port = htons(port);

//    unsigned short port;
//    for (port = 10000; port <= 11000; port++) {
//        sin.sin_port = htons(port);
//        if(::bind(sockfd, (struct sockaddr*)&sin, sizeof(struct sockaddr_in)) >= 0)
//        {
//            break;
//        }
//    }
//
//    if (port > 11000) {
//        cerr << "Port max exceeded" << endl;
//        exit(1);
//    }

    if(listen(sockfd, 5) < 0)
    {
        cerr << "listen failed" << endl;
        close(sockfd);
        exit(1);
    }

    //printf("%s %d\n", ip, ntohs(sin.sin_port));

    std::string str(ip);
    int pr = (int)ntohs(sin.sin_port);

    server = sockfd;
    return sockfd;
}
