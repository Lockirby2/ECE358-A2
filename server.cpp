#include "server.h"
#include "proxy.h"

using namespace std;

Server::Server(int port, string folder) {
    this->port = port;
    this->folder = folder;
}

Server::~Server() {

}

void Server::Run() {
    cout << "running away" << endl;
    Proxy proxy;

    int server = proxy.create_server_socket(port);

    while(true){
        //1. Await connection
        int client_socket;
        struct sockaddr_in client;
        socklen_t alen = sizeof(struct sockaddr_in);

        cout << "accepting client" << endl;
        if ((client_socket = accept(server, (struct sockaddr *)&client, &alen)) < 0) {
            cerr << "Accept failed" << endl;
            exit(1);
        }
        //2. receive connection,

        char* message;
        proxy.receive_from_socket(client_socket, message);

        cout << message << endl;
        //2.1 make TCB object,
        //2.2 create segment object from void* received from client,

        //3. Handshake

    }




    //4. After estab Open File Based on connectors IP and port

    //5. Create and populate new segment to be sent to client

    //6. Send segment to client

    //7. Go to 1.

    //I feel like 2 - 6 could be compartmentalized and maybe even parallellized, but idk
}