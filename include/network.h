#include <iostream>
#include <string>
#include <map>
#include "sfb.h"
#include "command.h"
#include "serial/serial.h"

using namespace std;
using namespace serial;

// Request packet structure
typedef struct {
	Command::COMMANDS	cmd;
	string				param;
} REQ_PACKET;

// Answer packet structure
typedef struct {
	Command::COMMANDS	cmd;
	string				answer;
} ANS_PACKET;

class Network {
private:
	Serial serial;
public:
	Network();
	~Network();

	bool		send(REQ_PACKET* req);
	ANS_PACKET*	recv();
};
