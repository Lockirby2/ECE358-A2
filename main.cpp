#include <fstream>
#include <iostream>
#include <iterator>
#include <stdlib.h>     /* atoi */
#include <string>
#include <vector>
#include "UDPServer.h"
//#include "message.h"

using namespace std;

int main(int argc, char *argv[]) {
    string folder = "";

	if (argc != 3) {
		fprintf(stderr, "usage: ./pa2-358s17 [<port> <folder>]\n");
		exit(1);
    }

    int port = atoi(argv[1]);
    folder = argv[2];

    UDPServer server = UDPServer(port, folder);
    server.run();

    return 0;
}
