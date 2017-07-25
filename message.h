#include <string>
#include <vector>
#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <vector>
#include <algorithm>
#include <cassert>
#include <cstring>
#include <errno.h>
#include <netdb.h>
#include <unistd.h>
#include <sstream>
#include <unordered_map>
#include <utility>
#include <tuple>
#include <arpa/inet.h>
#include <ifaddrs.h>

using namespace std;

#define bytes_to_u16(MSB,LSB) (((unsigned int) ((unsigned char) MSB)) & 255)<<8 | (((unsigned char) LSB)&255)
typedef uint8_t byte;

class Message{
	public:
		Message(int src, int dst, bool SYN, bool ACK, bool FIN, char* payload);
		Message(int source_port_p, int dest_port_p);
		Message();
		void encode_flags(bool SYN, bool ACK, bool FIN);
		int read_header(int socket, sockaddr_in addr);
		vector<bool> decode_flags();
		void fix_endian();
		void SwapEndian(uint16_t &val);
		void SwapEndian(uint32_t &val);

		//int read_header(int socket, sockaddr_in addr);

		const char *serialize() const {
        	return reinterpret_cast<const char *>(this);
    	}
    	static const Message deserialize(const char *buffer) {
        	return *reinterpret_cast<const Message *>(buffer);
    	}

	public:
		uint16_t source_port;
		uint16_t dest_port;
		uint32_t seg_size;
		uint32_t seq_num;
		uint32_t ack_num;
		uint16_t flags;
		uint16_t checksum;
		//char buf[1] = "A";
		//vector<char> payload;
		//char* x;
		//char x[65];
		char* payload;

};
