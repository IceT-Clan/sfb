#include "network.h"
#include <iostream>
#include <fstream>
#include <string>

Network::Network() {
	serial = new Serial();
	receive = new thread(bind(&Network::recv, this));
}

Network::~Network() {
	receive->join();
	delete receive;
	delete serial;
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

bool Network::recv() {
	cout << "Background process started..." << endl;
	while (true) {
		vector <uint8_t> buffer;
		serial->read(buffer, 1);
		cout << "Received something" << endl;
		switch (buffer[0]) {
		case REQUEST: {
			buffer.clear();
			serial->read(buffer, sizeof(COMMANDS));
			REQ_PACKET packet;
			packet.cmd = *reinterpret_cast <COMMANDS*> (&buffer[0]);
			packet.path0 = serial->readline();
			packet.path1 = serial->readline();
			packet.path0 = packet.path0.erase(packet.path0.length() - 1);
			packet.path1 = packet.path1.erase(packet.path1.length() - 1);
			lock_guard<mutex> lock(sec);
			requestPacket = packet;
			cout << "REQ received" << endl << "  Path0: " << packet.path0 << endl << "  Path1: " << packet.path1 << endl;
			requestPacketAvailable = true;
		}
					  break;
		case INFO: {
			buffer.clear();
			serial->read(buffer, sizeof(size_t));
			INFO_PACKET packet;
			packet.bytesnr = *reinterpret_cast <size_t*> (&buffer[0]);
			lock_guard<mutex> lock(sec);
			infoPacket = packet;
			cout << "INFO received" << endl << "  Bytesnr: " << packet.bytesnr << endl;
			infoPacketAvailable = true;
		}
				   break;
		case CONF: {
			buffer.clear();
			serial->read(buffer, sizeof(bool));
			CONF_PACKET packet;
			packet.confirmation = *reinterpret_cast <bool*> (&buffer[0]);
			lock_guard<mutex> lock(sec);
			confPacket = packet;
			cout << "CONF received" << endl << "  Confirmation: " << packet.confirmation << endl;
			confPacketAvailable = true;
		}
				   break;
		case DATA: {
			buffer.clear();
			serial->read(buffer, 252);
			DATA_PACKET packet;
			memcpy(packet.bytes, &buffer[0], 252);
			buffer.clear();
			serial->read(buffer, sizeof(uint32_t));
			packet.checksum = *reinterpret_cast <uint32_t*> (&buffer[0]);
			lock_guard<mutex> lock(sec);
			dataPacket = packet;
			cout << "DATA received" << endl << "  Checksum: " << packet.checksum << endl;
			dataPacketAvailable = true;
		}
				   break;
		default:
			cerr << "An error occured!" << endl;
			break;
		}
	}
	return true;
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

REQ_PACKET Network::getrequestpacket() {
	while (!getrequestPacketAvailable()) { _sleep(10); }
	return requestPacket;
}

INFO_PACKET Network::getinfopacket() {
	while (!getinfoPacketAvailable()) { _sleep(10); }
	return infoPacket;
}

CONF_PACKET Network::getconfpacket() {
	while (!getconfPacketAvailable()) { _sleep(10); }
	return confPacket;
}

DATA_PACKET Network::getdatapacket() {
	while (!getdataPacketAvailable()) { _sleep(10); }
	return dataPacket;
}

bool Network::getrequestPacketAvailable() {
	lock_guard<mutex> lock(sec);
	return requestPacketAvailable;
}

bool Network::getdataPacketAvailable() {
	lock_guard<mutex> lock(sec);
	return dataPacketAvailable;
}

bool Network::getconfPacketAvailable() {
	lock_guard<mutex> lock(sec);
	return confPacketAvailable;
}

bool Network::getinfoPacketAvailable() {
	lock_guard<mutex> lock(sec);
	return infoPacketAvailable;
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
