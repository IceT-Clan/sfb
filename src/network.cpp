#include "network.h"
#include <iostream>
#include <fstream>
#include <string>

Network::Network() {
	serial = new Serial();
}

Network::~Network() {
}

string Network::getfilename(string path) {
	///
	///Extracts The Filename
	///
	int i = path.find_last_of('\\');
	if (i != string::npos) {
		path = path.substr(i + 1);
	}

	return path;
}

bool Network::readfileinfos(string path) {
	///
	///Get File Name
	///
	anspacket.filename = getfilename(path);
	///
	///Save binary data in 8bit vector
	///
	ifstream input(path, std::ios::binary);


	return true;
}


bool Network::init(string port) {
	serial->setPort(port);

	// Check if port is open and return the result
	return serial->isOpen();
}

bool Network::send(REQ_PACKET* req) {
	return true;
}

ANS_PACKET* Network::recv() {
	return NULL;
}

