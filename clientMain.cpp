#include "UDPClient.h"
#include <stdlib.h>     /* atoi */


using namespace std;

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "usage: ./testClient [<IP> <port>]\n");
        exit(1);
    }

    char* ip = argv[1];
    int port = atoi(argv[2]);

    cout << "Starting Client" << endl;
    UDPClient client = UDPClient(ip, port);
    client.run();
    cout << "wait what" << endl;
}