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



int UDPServer::get_client_num(sockaddr_in source)
{
    sockaddr_in client_addr;

    for (unsigned int i = 0; i < addr_clients.size(); i++) {
        client_addr = addr_clients.at(i);

        if (client_addr.sin_port == source.sin_port &&
            client_addr.sin_addr.s_addr == source.sin_addr.s_addr) {
            return i;
        }
    }

    addr_clients.push_back(source);
    return (addr_clients.size() - 1);

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

    server = sockfd;
    return sockfd;
}

vector<BYTE> UDPServer::readFile(const char* filename) {
    cout << "Reading file at: " << filename << endl;
    // open the file:
    ifstream file(filename, ios::binary);
    file.unsetf(ios::skipws);

    // get filesize
    streampos fileSize;
    file.seekg(0, ios::end);
    fileSize = file.tellg();
    file.seekg(0, ios::beg);

    // reserve capacity
    vector<BYTE> vec;
    vec.reserve(fileSize);

    // read the data:
    vec.insert(vec.begin(),
               istream_iterator<BYTE>(file),
               istream_iterator<BYTE>());

    return vec;
}

void UDPServer::run()
{
    char buffer[65487];
    sockaddr_in source_addr;
    socklen_t fromLength = sizeof(source_addr);
    ssize_t recv_msg_size;

    cout <<  get_ip() << endl;

    create_server_socket(port);
    while (true) {
        recv_msg_size = recvfrom( server, (void *)buffer, sizeof(buffer),
                            0,
                            (struct sockaddr *)&source_addr,
                            &fromLength
                        );

        if (recv_msg_size < 0) {
            cerr << "error: Recv failed" << endl;
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
    msg.fix_endian();

    int check = msg.checksum;
    int segsize = msg.seg_size;
    vector<bool> flags = msg.decode_flags();

    msg.checksum = 0;

    msg.fix_endian();

    if (!CheckSumUtil::checkChecksum(check, (void*)msg.serialize(), segsize)){
        int sum = CheckSumUtil::computeSum((void*)msg.serialize(), segsize);
        cout << "checksum failed: " << check << " them|us " << sum << " sum: " << sum + check << endl;
        return 0;
    }

    msg.fix_endian();

    print_message(msg);

    if(flags.at(0)){ //if syn
        if(connections.find(msg.source_port) != connections.end()){
            cout << "received SYN from live connection, aborting new connection" << endl;
            return 0;
        }
        //initiate handshake
        // Should definitely do this with the beautiful constructor that sherwin made, but this helped me visually
        Message handshake = Message();

        //source port comes from us, dest port from the client
        handshake.source_port = port;
        handshake.dest_port = msg.source_port;

        handshake.ack_num = msg.seq_num + 1;
        //this means syn = 1, ack = 1, fin = 0
        handshake.encode_flags(1,1,0);
        // Without payload the seg size is 20 in bytes
        handshake.seg_size = 20;
        // The sequence is started at syn time I think.
        // Also I think real servers randomize it, but iirc we get to chose the first seq number
        handshake.seq_num = 2;

        TCB tcb = TCB();
        tcb.current = TCB::synrecd;
        connections.insert(pair<int, TCB>(msg.source_port, tcb));

        int size = handshake.seg_size;
        handshake.checksum = 0;

        const char* serial = handshake.serialize();

        cout << serial << endl;
        uint16_t sum = CheckSumUtil::computeSum((void*)serial, size);
        cout << "checksum set to: " << sum << endl;

        handshake.fix_endian();
        handshake.checksum = ~sum;
        send_message(handshake, client);
        bitset<8> x(handshake.checksum);
        cout << x << endl;
        return 1;
    }

    TCB* t;
    if(connections.find(msg.source_port) == connections.end()){
        cout << "received new connection without SYN aborting new connection" << endl;
        return 0;
    } else {
        t = &connections.at(msg.source_port);
    }

    if (flags.at(1) && !flags.at(0)) { // received ACK
        // if we have just established a connection, send a file
        if (connections.at(msg.source_port).current == TCB::synrecd) {
            cout << "Sending file data upon connection startup" << endl;
            vector<BYTE> vec;
            stringstream filenameStream;
            filenameStream << inet_ntoa(addr_clients.at(client).sin_addr) << ".";
            filenameStream << msg.source_port << ".";
            filenameStream << get_ip() << ".";
            filenameStream << get_server_port();
            
            ifstream ifile(this->folder + filenameStream.str());
            if (ifile) {
                try {
                    //TODO: Test this
                    vec = readFile((this->folder + filenameStream.str()).c_str());
                    uint32_t vecSize = vec.size();
                    vec.insert(vec.begin(), (vecSize | 0x000000FF));
                    vec.insert(vec.begin(), ((vecSize | 0x0000FF00) >> 8));
                    vec.insert(vec.begin(), ((vecSize | 0x00FF0000) >> 16));
                    vec.insert(vec.begin(), ((vecSize | 0xFF000000) >> 24));
                    for (unsigned int i = 0; i < vec.size(); i += 1) {
                        cout << vec.at(i) << endl;
                    }
                } catch ( ... ) {
                    fprintf(stderr, "File wasn't read correctly\n");
                }
            } else {
                vec.push_back(0x00);
                vec.push_back(0x00);
                vec.push_back(0x00);
                vec.push_back(0x00);
            }
//            unsigned char* test = "Hello, its me";
//            cout << test[0] << endl;
            Message response = Message(get_server_port(), msg.source_port, false, false, false, static_cast<unsigned char*>(vec.data()), vec.size());
            response.fix_endian();
            cout << response.payload << endl;
            send_message(response, client);
        }
    }


    //TODO Append to file and echo back

    //FSM for sending?
    return true;
}

void UDPServer::send_message(Message msg, int client_index)
{

    sockaddr_in addr_clientsaddr_ = addr_clients.at(client_index);

    cout << "sending data to: " << addr_clientsaddr_.sin_port << endl;
    int result = sendto( server, msg.serialize(), sizeof(Message),
                     0,
                     reinterpret_cast<sockaddr *>(&addr_clientsaddr_),
                     sizeof(sockaddr_in)
                 );

    if (result != sizeof(Message)) {
        cerr << "error: failed to send entire message" << endl;
        exit(1);
    }
}


void UDPServer::print_message(Message message){
    cout << "Source port: " << message.source_port << endl;
    cout << "Destination port: " << message.dest_port << endl;
    cout << "Segment Size: " << message.seg_size << endl;
    cout << "Sequence Number: " << message.seq_num<< endl;
    cout << "Ack number: " << message.ack_num << endl;
    cout << "Syn: " << message.decode_flags().at(0) << endl;
    cout << "Ack: " << message.decode_flags().at(1) << endl;
    cout << "Fin: " << message.decode_flags().at(2) << endl;
}


