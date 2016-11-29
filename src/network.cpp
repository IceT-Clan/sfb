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
	filename = getfilename(path);
	///
	///Save binary data in 8bit vector
	///
	ifstream input(path, std::ios::binary);


	return true;
}


bool Network::init(string port) {
	serial->setPort(port);

	// Check if port is open and return the result
	serial->open();

	// Set DTR to let our partner know we are there
	serial->setDTR();

	return serial->isOpen();
}

bool Network::send(REQ_PACKET* req) {
	return true;
}

DATA_PACKET* Network::recv() {
	return NULL;
}

bool		Network::send(const REQ_PACKET &pkt) {

}
bool		Network::send(const INFO_PACKET &pkt) {

}
bool		Network::send(const CONF_PACKET &pkt) {

}
bool		Network::send(const DATA_PACKET &pkt) {

}

