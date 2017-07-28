#include "UDPClient.h"
#include <bitset>

using namespace std;

/*Put in Client*/
//sockaddr_in host_addr_;


//void UDPClient::get_server_info(string address, string port)
//{
//    addrinfo *lookup, *lookup_head;
//    int result = getaddrinfo(
//            address.c_str(),
//            port.c_str(),
//            NULL,
//            &lookup_head);
//
//    lookup = lookup_head;
//
//    while (lookup) {
//        if (lookup->ai_family != AF_INET) {
//            lookup = lookup->ai_next;
//            continue;
//        }
//
//        memcpy(&host_addr_, lookup->ai_addr, sizeof(sockaddr_in));
//        break;
//    }
//
//    freeaddrinfo(lookup_head);
//
//    host_addr_.sin_family = AF_INET;
//    host_addr_.sin_port = htons(atoi(port.c_str()));
//}

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
//
//int UDPClient::send_message(int socket, int length, string message ){
//    int status = send(socket, message.c_str(), length + 1, 0);
//    if (status < 0) {
//        cerr << "send failed" << endl;
//        return -1;
//    }
//    return 0;
//}


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

void UDPClient::send_message(Message msg, sockaddr_in client)
{


    cout << "sending data to: " << msg.dest_port << endl;
    int result = sendto(server, msg.serialize(), sizeof(Message),
                         0,
                         reinterpret_cast<sockaddr *>(&client),
                         sizeof(sockaddr_in)
    );

    if (result != sizeof(Message)) {
        cerr << "error: failed to send entire message" << endl;
        exit(1);
    }
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

//    server = sockfd;
    return sockfd;
}

UDPClient::UDPClient(char* targetIp, int targetPort, int port) {
    this->port = port;
    this->targetIp = targetIp;
    this->targetPort = targetPort;
}

void UDPClient::run() {
    struct sockaddr_in sin;

    sin.sin_family = AF_INET;
    sin.sin_addr.s_addr = htonl(INADDR_ANY);
    inet_aton(targetIp, (struct in_addr *)&sin.sin_addr.s_addr);
    sin.sin_port = htons(targetPort);

    server = create_server_socket();

    vector<BYTE> vec;



//    Message msg = Message();
    Message msg = Message(port,targetPort, true, false, false, static_cast<unsigned char*>(vec.data()), vec.size());
    msg.seg_size = 20;
    msg.checksum = 0;
    msg.ack_num = 0;
    msg.seq_num = 22;
    msg.fix_endian();
    uint16_t checksum = CheckSumUtil::computeSum((void*)msg.serialize(), 20);
    msg.fix_endian();
//    cout << "checksum: " << ~checksum << endl;
    msg.checksum = ~checksum;
    msg.fix_endian();
    send_message(msg, sin);

    char buffer[65487];
    sockaddr_in source_addr;
    socklen_t fromLength = sizeof(source_addr);
    ssize_t recv_msg_size;

    recv_msg_size = recvfrom( server, (void *)buffer, sizeof(buffer),
                              0,
                              (struct sockaddr *)&source_addr,
                              &fromLength
    );

    msg = Message::deserialize(buffer);
    bitset<8> x(msg.checksum);
    cout << x << endl;
    cout << msg.checksum << endl;
    msg.fix_endian();
    print_message(msg);

    Message second = Message(port,targetPort, false, true, false, static_cast<unsigned char*>(vec.data()), vec.size());
    //checksum fucked? idk

    second.seg_size = 20;
    second.checksum = 0;
    second.ack_num = msg.seq_num + 1;
    second.seq_num = msg.ack_num;
    second.fix_endian();
    checksum = CheckSumUtil::computeSum((void*)second.serialize(), 20);
    second.fix_endian();
//    cout << "checksum: " << ~checksum << endl;
    second.checksum = ~checksum;
    second.fix_endian();
    send_message(second, sin);

    recv_msg_size = recvfrom( server, (void *)buffer, sizeof(buffer),
                              0,
                              (struct sockaddr *)&source_addr,
                              &fromLength
    );

    int j;
    for(j = 0; j < 24; ++j)
        printf("%02x\n", ((uint8_t*) buffer)[j]);

    msg = Message::deserialize(buffer);

    msg.fix_endian();
    print_message(msg);

    cout << "Payload: " << msg.payload << endl;

    int payloadSize = msg.seg_size - 20;



    for (int i = 0; i < payloadSize; ++i) {
        cout << *static_cast<uint8_t*>((msg.payload + i)) << endl;
    }

//    msg.fix_endian();
    //TODO listen for response
}

void UDPClient::print_message(Message message){
    cout << "Source port: " << message.source_port << endl;
    cout << "Destination port: " << message.dest_port << endl;
    cout << "Segment Size: " << message.seg_size << endl;
    cout << "Sequence Number: " << message.seq_num<< endl;
    cout << "Ack number: " << message.ack_num << endl;
    cout << "Syn: " << message.decode_flags().at(0) << endl;
    cout << "Ack: " << message.decode_flags().at(1) << endl;
    cout << "Fin: " << message.decode_flags().at(2) << endl;
}