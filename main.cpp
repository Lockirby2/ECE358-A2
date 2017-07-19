#include <fstream>
#include <iostream>
#include <iterator>
#include <string>
#include <vector>
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
		fprintf(stderr, "usage: ./addpeer [<port> <folder>]\n");
		exit(1);
	}
        folder = argv[2];

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
