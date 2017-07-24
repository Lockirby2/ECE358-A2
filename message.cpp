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
	flags |= SYN << 2;
	flags |= ACK << 1;
	flags |= FIN << 0;
}

std::vector<bool> Message::decode_flags(){
	vector<bool> vec_flags;
	for (int i = 2; i >= 0; i--){
		vec_flags.push_back((flags >> i) & 1);
	}
	return vec_flags;
}

int Message::read_header(int socket, sockaddr_in addr){
/*	server_length = sizeof(struct sockaddr_in);
    recvfrom(mySocket, recMessage, STRLEN, 0, (SOCKADDR*) &myAddress, &server_length);
    cout<<recMessage<<endl;*/
}
