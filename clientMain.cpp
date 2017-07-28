#include "UDPClient.h"
#include <stdlib.h>     /* atoi */


using namespace std;

int main(int argc, char *argv[]) {
    if (argc != 4) {
        fprintf(stderr, "usage: ./testClient [<IP> <port>]\n");
        exit(1);
    }

    char* ip = argv[1];
    int tport = atoi(argv[2]);
    int port = atoi(argv[3]);

    cout << "Starting Client" << endl;
    UDPClient client = UDPClient(ip, tport, port);
    client.run();
    cout << "wait what" << endl;
}