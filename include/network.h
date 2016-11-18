#include <iostream>
#include <string>
#include <map>
#include "sfb.h"
#include "command.h"
#include "serial/serial.h"

using namespace std;
using namespace serial;

class Network {
private:
	// Request packet structure
	struct REQ_PACKET {
		Command::COMMANDS	cmd;
		string				param;
	};

	// Answer packet structure
	struct ANS_PACKET {
		Command::COMMANDS	cmd;
		string				answer;
	};

	Serial ser;
public:
	Network();
	~Network();
};
