#include <sys/socket.h>
#include <stdio.h>
#include <stdlib.h>
#include <iostream>
#include <string>
#include <string.h>
#include <bitset>
#include "message.h"

//Send SYN
Message::Message(int src, int dst, bool SYN, bool ACK, bool FIN, char* payload): source_port(src), dest_port(dst), payload(payload){
	encode_flags(SYN, ACK, FIN);
	seg_size = sizeof(payload)/sizeof(char) + 20;

};

Message::Message(){};

void Message::encode_flags(bool SYN, bool ACK, bool FIN){
	flags = 0;
	flags |= SYN << 15;
	flags |= ACK << 14;
	flags |= FIN << 13;
}

std::vector<bool> Message::decode_flags(){
	vector<bool> vec_flags;
	for (int i = 2; i >= 0; i--){
		vec_flags.push_back((flags >> (i+13)) & 1);
	}
	return vec_flags;
}

int Message::read_header(int socket, sockaddr_in addr){
/*	server_length = sizeof(struct sockaddr_in);
    recvfrom(mySocket, recMessage, STRLEN, 0, (SOCKADDR*) &myAddress, &server_length);
    cout<<recMessage<<endl;*/
}

void Message::SwapEndian(uint16_t &val)
{
    val = (val<<8) | (val>>8);
}			
void Message::SwapEndian(uint32_t &val)
{
    val = (val<<24) | ((val<<8) & 0x00ff0000) |
          ((val>>8) & 0x0000ff00) | (val>>24);
}

void Message::fix_endian(){
	SwapEndian(source_port);
	SwapEndian(dest_port);
	SwapEndian(seg_size);
	SwapEndian(seq_num);
	SwapEndian(ack_num);
	SwapEndian(flags);
	SwapEndian(checksum);
}
