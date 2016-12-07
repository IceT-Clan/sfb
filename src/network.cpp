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

//using boost::hash_combine
template <class boost>
inline void hash_combine(size_t& seed, boost const& v) {
	seed ^= hash<boost>()(v) + 0x9e3779b9 + (seed >> 2);
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

// SEND REQUEST
bool Network::sendpkt(REQ_PACKET &pkt) {
	sendraw(PACKETS::REQUEST);

	pkt.path0 += "\n";
	pkt.path1 += "\n";

	sendraw(pkt.cmd);
	serial->write(pkt.path0);
	serial->write(pkt.path1);
	cout << "SENT" << endl;
	return true;
}

// SEND INFO
bool Network::sendpkt(INFO_PACKET &pkt) {
	sendraw(PACKETS::INFO);
	sendraw(pkt);
	cout << "SENT" << endl;
	return true;
}

// SEND CONF
bool Network::sendpkt(CONF_PACKET &pkt) {
	sendraw(PACKETS::CONF);
	sendraw(pkt);
	cout << "SENT" << endl;
	return true;
}

// SEND DATA
bool Network::sendpkt(DATA_PACKET &pkt) {
	sendraw(PACKETS::DATA);
	 //hashcode

	size_t seed = sizeof(pkt.bytes);
	for (const uint8_t& b : pkt.bytes) {
		hash_combine(seed, b);
	}

	pkt.checksum = seed;
	serial->write(pkt.bytes, 252);
	sendraw(pkt.checksum);
	cout << "SENT" <<endl;
	return true;
}

template <class temp>
bool Network::sendraw(const temp &pkt) {
	serial->write(reinterpret_cast<const uint8_t*>(&(pkt)), sizeof(pkt));
	return true;
}
