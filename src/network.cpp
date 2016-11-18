#include "network.h"

Network::Network() {
}

Network::~Network() {
}

bool Network::send(REQ_PACKET* req) {
	return true;
}

ANS_PACKET* Network::recv() {
	return NULL;
}
