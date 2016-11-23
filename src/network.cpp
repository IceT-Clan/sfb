#include "network.h"

Network::Network() {
	serial = new Serial();
}

Network::~Network() {
}

bool Network::init(string port) {
	serial->setPort(port);

	// Check if port is open and return the result
	serial->open();
	return serial->isOpen();
}

bool Network::send(REQ_PACKET* req) {
	return true;
}

ANS_PACKET* Network::recv() {
	return NULL;
}
