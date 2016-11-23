#include "network.h"

Network::Network() {
	serial = new Serial();
}

Network::~Network() {
}

bool Network::init(string port) {
	serial->setPort(port);

	// Check availability of serial port
	serial->open();
	if (!serial->isOpen()) {
		// Port could not be opened
		cerr << "Port \"" << port << "\" could not be opened." << endl;
		return false;
	}

	// Wait until partner is ready
	cout << "Waiting for connection..." << endl;
	while (!serial->getCD()) {};

	return true;
}

bool Network::send(REQ_PACKET* req) {
	return true;
}

ANS_PACKET* Network::recv() {
	return NULL;
}
