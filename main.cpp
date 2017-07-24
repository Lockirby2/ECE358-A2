#include <fstream>
#include <iostream>
#include <iterator>
#include <stdlib.h>     /* atoi */
#include <string>
#include <vector>
#include "UDPServer.h"
//#include "message.h"

typedef unsigned char BYTE;

using namespace std;

vector<BYTE> readFile(const char* filename) {
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

    //This code should be placed elsewhere, to be called when a connection is established


    try {
        vector<BYTE> vec = readFile((folder + "filename").c_str());
        for (unsigned int i = 0; i < vec.size(); i += 1) {
            cout << vec.at(i) << endl;
        }
    } catch ( ... ) {
        fprintf(stderr, "File wasn't read correctly\n");
    }

	return 0;
}
