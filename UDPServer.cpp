#include "UDPServer.h"

using namespace std;

UDPServer::UDPServer(int port, string folder) {
    this->port = port;
    this->folder = folder;
}

int UDPServer::get_server_port() {
    struct sockaddr_in sin;
    socklen_t len = sizeof(sin);
    getsockname(server, (struct sockaddr *)&sin, &len);
    //cout << "SERVER_PORT " << ntohs(sin.sin_port) << endl;
    return ntohs(sin.sin_port);
}

string UDPServer::get_server_address() {
    char hostname[256];
    gethostname(hostname, 256);
    //cout << "SERVER_ADDRESS " << hostname << endl;
    return string(hostname);
}



int UDPServer::get_client_num(sockaddr_in source_addr)
{
    sockaddr_in client_addr;

    for (int i = 0; i < clients_.size(); i++) {
        client_addr = clients_.at(i);

        if (client_addr.sin_port == source_addr.sin_port &&
            client_addr.sin_addr.s_addr == source_addr.sin_addr.s_addr) {
            return i;
        }
    }

    clients_.push_back(source_addr);
    return (clients_.size() - 1);

}

char* UDPServer::get_ip() {
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


int UDPServer::create_server_socket(int port)
{
    int sockfd = -1;
    char *ip = get_ip();

    if((sockfd = socket(AF_INET, SOCK_DGRAM, IPPROTO_UDP)) < 0) {
        cerr << "socket create failed" << endl;
        return -1;
    }

    struct sockaddr_in sin;

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    inet_aton(ip, (struct in_addr *)&sin.sin_addr.s_addr);
    sin.sin_port = htons(port);

    if(::bind(sockfd, (struct sockaddr*)&sin, sizeof(struct sockaddr_in)) < 0)
    {
        cout << "bind failed" << endl;
        close(sockfd);
        exit(1);
    }

    if(listen(sockfd, 5) < 0)
    {
        cerr << "listen failed" << endl;
        close(sockfd);
        exit(1);
    }

    server = sockfd;
    return sockfd;
}


void UDPServer::run()
{
    char buffer[65487];
    sockaddr_in source_addr;
    socklen_t fromLength = sizeof(source_addr);
    ssize_t recv_msg_size;

    while (true) {
        recv_msg_size = recvfrom( server, (void *)buffer, sizeof(buffer),
                            0,
                            (struct sockaddr *)&source_addr,
                            &fromLength
                        );

        if (recv_msg_size < 0) {
            cerr << "error: could not receive message" << endl;
            exit(1);
        }

        int client_num = get_client_num(source_addr);
        handle_msg(client_num, buffer);
    }

    //2. receive connection,

    //2.1 make TCB object,
    //2.2 create segment object from void* received from client,

    //3. Handshake


    //4. After estab Open File Based on connectors IP and port

    //5. Create and populate new segment to be sent to client

    //6. Send segment to client

    //7. Go to 1.

    //I feel like 2 - 6 could be compartmentalized and maybe even parallellized, but idk

}

void UDPServer::stop()
{
    close(server);
    exit(1);
}

bool UDPServer::handle_msg(int client, const char *reply)
{
    Message msg = Message::deserialize(reply);

    vector<bool> flags = msg.decode_flags();


    if(flags.at(0)){ //if syn
        //initiate handshake
        Message handshake = Message();

        //source port comes from us, dest port from the client
        handshake.source_port = port;
        handshake.dest_port = msg.source_port;

        handshake.ack_num = msg.seq_num + 1;
        //this means syn = 0, ack = 1, fin = 0
        handshake.encode_flags(0,1,0);
        // Without payload the seg size is 20 in bytes
        handshake.seg_size = 20;
        // The sequence is started at syn time I think.
        // Also I think real servers randomize it, but iirc we get to chose the first seq number
        handshake.seq_num = 1;
        //TODO:
        //SET CHECKSUM
        //DO WE NEED ANY PAYLOAD?
    }
    //FSM for sending?
    return true;
}

void UDPServer::send_message(Message msg, int client_index)
{

    sockaddr_in clients_addr_ = clients_.at(client_index);
    int result = sendto( server, msg.serialize(), sizeof(Message),
                     0,
                     reinterpret_cast<sockaddr *>(&clients_addr_),
                     sizeof(sockaddr_in)
                 );

    if (result != sizeof(Message)) {
        cerr << "error: failed to send entire message" << endl;
        exit(1);
    }
}



