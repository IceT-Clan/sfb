#include "network.h"

#ifdef _WIN32
#define SLEEP(x) Sleep(x);
#else
#define SLEEP(x) sleep(x);
#endif

Network::Network() {
	serial = new Serial();
	threadRunning = true;
	receive = new thread(bind(&Network::recv, this));
}

Network::~Network() {
	threadRunning = false;
	// TODO: end thread
	exit(0);
	//receive->join();
	delete receive;
	delete serial;
}

string Network::getfilename(string path) {
	// Extracts The Filename
	int i = path.find_last_of('\\');
	if (i != string::npos) {
		path = path.substr(i + 1);
	}

	return path;
}

bool Network::readfileinfos(string path) {
	// Get File Name
	filename = getfilename(path);

	// Save binary data in 8bit vector
	ifstream input(path, std::ios::binary);

	return true;
}

// using boost::hash_combine
template <class boost>
inline void hash_combine(size_t& seed, boost const& v) {
	seed ^= hash<boost>()(v) + 0x9e3779b9 + (seed >> 2);
}

bool Network::init(string port /*= ""*/) {
	// Do we have opened a port before?
	if (serial->isOpen()) {
		serial->close();
	}

	// Set default port if none was given
	if (port.empty()) {
#ifdef _WIN32
		port = "COM1";
#else
		port = "/dev/ttyS0";
#endif
	}

	serial->setPort(port);

	// Check if port is open and return the result
	serial->open();

	// Set DTR to let our partner know we are there
	// (Currently not used)
	// serial->setDTR();

	return serial->isOpen();
}

bool Network::recv() {
	COUT("Background thread started..." << endl);
	while (true) {
		vector<uint8_t> buffer;

		// Wait until we received something
		if(outputReceiveMessage) COUT("Waiting..." << endl);
		bool received = false;
		while (!received) {
			SLEEP(10);
			{
				lock_guard<mutex> lock(sec);
				received = serial->available() > 0;
			}
		}
		readBytes(buffer, 1);

		if(outputReceiveMessage) COUT("Received something" << endl);
		switch (buffer[0]) {
		case REQUEST: {
			if(outputReceiveMessage) COUT("REQ received" << endl);
			buffer.clear();
			readBytes(buffer, sizeof(COMMANDS));
			REQ_PACKET packet;
			packet.cmd = *reinterpret_cast <COMMANDS*> (&buffer[0]);
			if(outputReceiveMessage) COUT("  Cmd: " << packet.cmd << endl);
			lock_guard<mutex> lock(sec);
			packet.path0 = serial->readline();
			packet.path1 = serial->readline();
			packet.path0 = packet.path0.erase(packet.path0.length() - 1);
			packet.path1 = packet.path1.erase(packet.path1.length() - 1);
			requestPacket = packet;
			if(outputReceiveMessage) COUT("  Path0: " << packet.path0 << endl << "  Path1: " << packet.path1 << endl);
			requestPacketAvailable = true;
		} break;
		case INFO: {
			if(outputReceiveMessage) COUT("INFO received" << endl);
			buffer.clear();
			readBytes(buffer, sizeof(INFO_PACKET::bytesnr));
			INFO_PACKET packet;
			packet.bytesnr = *reinterpret_cast <size_t*> (&buffer[0]);
			lock_guard<mutex> lock(sec);
			infoPacket = packet;
			if(outputReceiveMessage) COUT("  Bytesnr: " << packet.bytesnr << endl);
			infoPacketAvailable = true;
		} break;
		case CONF: {
			if(outputReceiveMessage) COUT("CONF received" << endl);
			buffer.clear();
			readBytes(buffer, sizeof(CONFIRMATION));
			CONF_PACKET packet;
			packet.confirmation = *reinterpret_cast <CONFIRMATION*> (&buffer[0]);
			lock_guard<mutex> lock(sec);
			confPacket = packet;
			if(outputReceiveMessage) COUT("  Confirmation: " << packet.confirmation << endl);
			confPacketAvailable = true;
		} break;
		case DATA: {
			if(outputReceiveMessage) COUT("DATA received" << endl);
			buffer.clear();
			readBytes(buffer, 252);
			DATA_PACKET packet;
			memcpy(packet.bytes, &buffer[0], 252);
			buffer.clear();
			readBytes(buffer, sizeof(uint32_t));
			packet.checksum = *reinterpret_cast <uint32_t*> (&buffer[0]);
			lock_guard<mutex> lock(sec);
			dataPacket = packet;
			if(outputReceiveMessage) COUT("  Checksum: " << packet.checksum << endl);
			dataPacketAvailable = true;
		} break;
		case MESSAGE: {
			if(outputReceiveMessage) COUT("MSG received" << endl);
			buffer.clear();
			readBytes(buffer, sizeof(uint32_t));
			MSG_PACKET packet;
			packet.size = *reinterpret_cast <uint32_t*> (&buffer[0]);
			lock_guard<mutex> lock(sec);
			packet.msg = serial->read(packet.size);			
			msgPacket = packet;
			msgPacketAvailable = true;
		} break;
		default:
			CERR( "An error occured: Unknown Package received!" << endl);
			break;
		}
	}
	return true;
}

// Send REQ
bool Network::sendpkt(REQ_PACKET &pkt) {
	sendraw(PACKETS::REQUEST);

	pkt.path0 += "\n";
	pkt.path1 += "\n";

	sendraw(pkt.cmd);
	serial->write(pkt.path0);
	serial->write(pkt.path1);
	if (outputReceiveMessage) COUT("SENT REQ_PACKET" << endl);
	return true;
}

REQ_PACKET Network::getrequestpacket() {
	while (!getrequestPacketAvailable()) { SLEEP(10); }
	lock_guard<mutex> lock(sec);
	requestPacketAvailable = false;
	return requestPacket;
}

INFO_PACKET Network::getinfopacket() {
	while (!getinfoPacketAvailable()) { SLEEP(10); }
	lock_guard<mutex> lock(sec);
	infoPacketAvailable = false;
	return infoPacket;
}

MSG_PACKET Network::getmsgpacket()
{
	while (!getmsgPacketAvailable()) { SLEEP(10); }
	lock_guard<mutex> lock(sec);
	msgPacketAvailable = false;
	return msgPacket;
}

CONF_PACKET Network::getconfpacket() {
	while (!getconfPacketAvailable()) { SLEEP(10); }
	lock_guard<mutex> lock(sec);
	confPacketAvailable = false;
	return confPacket;
}

DATA_PACKET Network::getdatapacket() {
	while (!getdataPacketAvailable()) { SLEEP(10); }
	lock_guard<mutex> lock(sec);
	dataPacketAvailable = false;
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

bool Network::getmsgPacketAvailable() {
	lock_guard<mutex> lock(sec);
	return msgPacketAvailable;
}

// SEND INFO
bool Network::sendpkt(INFO_PACKET &pkt) {
	sendraw(PACKETS::INFO);
	sendraw(pkt);
	if (outputReceiveMessage) COUT("SENT INFO_PACKET" << endl);
	return true;
}

// SEND CONF
bool Network::sendpkt(CONF_PACKET &pkt) {
	sendraw(PACKETS::CONF);
	sendraw(pkt);
	if (outputReceiveMessage) COUT("SENT CONF_PACKET" << endl);
	return true;
}

// SEND DATA
bool Network::sendpkt(DATA_PACKET &pkt) {
	sendraw(PACKETS::DATA);

	 //hashcode
	createCheckSum(pkt);

	writeBytes(pkt.bytes, 252);
	sendraw(pkt.checksum);
	if (outputReceiveMessage) COUT("SENT DATA_PACKET" <<endl);
	return true;
}

bool Network::sendpkt(MSG_PACKET & pkt) {
	sendraw(PACKETS::MESSAGE);
	pkt.size = pkt.msg.size();
	sendraw(pkt.size);
	lock_guard<mutex> lock(sec);
	serial->write(pkt.msg);
	if (outputReceiveMessage) COUT("SENT MSG_PACKET" << endl);
	return true;
}

void Network::readBytes(vector<uint8_t>& buffer, size_t count) {
	lock_guard<mutex> lock(sec);
	buffer.clear();
	size_t readBytes = 0;
	vector<uint8_t> tbuffer;
	while (readBytes < count) {
		size_t bytes = serial->read(tbuffer, count - readBytes);
		for (const uint8_t& byte : tbuffer)
			buffer.push_back(byte);
		tbuffer.clear();
		readBytes += bytes;
	}
}

void Network::writeBytes(uint8_t* buffer, size_t count) {
	lock_guard<mutex> lock(sec);
	size_t bytesWritten = 0;
	while (bytesWritten < count) {
		bytesWritten += serial->write(buffer + bytesWritten, count - bytesWritten);
	}
}

void Network::setDebug(bool debug) {
	this->outputReceiveMessage = debug;
}

template <class temp>
bool Network::sendraw(const temp &pkt) {
	lock_guard<mutex> lock(sec);
	size_t bytes = sizeof(temp);
	size_t bytesWritten = 0;
	while (bytesWritten < bytes) {
		bytesWritten += serial->write(reinterpret_cast<const uint8_t*>(&(pkt)) + bytesWritten, bytes - bytesWritten);
	}
	return true;
}

void Network::createCheckSum(DATA_PACKET& pkt) {
	size_t seed = sizeof(pkt.bytes);
	for (const uint8_t& b : pkt.bytes) {
		hash_combine(seed, b);
	}

	pkt.checksum = seed;
}
