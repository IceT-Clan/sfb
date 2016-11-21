#ifndef NETWORK_H
#define NETWORK_H

#include <iostream>
#include <string>
#include <map>
#include "sfb.h"
#include "serial/serial.h"

using namespace std;
using namespace serial;

class Command;

class Network {
private:
	Serial* serial;
public:
	Network();
	~Network();

	bool		init(string port);
	bool		send(REQ_PACKET* req);
	ANS_PACKET*	recv();
};

#endif /* NETWORK_H */
