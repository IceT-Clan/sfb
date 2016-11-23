#ifndef NETWORK_H
#define NETWORK_H

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include "sfb.h"
#include "serial/serial.h"

using namespace std;
using namespace serial;

class Command;

class Network 
{
	private:
		Serial*			serial;
		vector<uint8_t> bytes;
		string			filename;

		ANS_PACKET		anspacket;
	public:
		Network();
		~Network();

		bool		Network::readfileinfos(string path);
		bool		init(string port);
		bool		send(REQ_PACKET* req);
		string getfilename(string path);
				
		ANS_PACKET*	recv();

};

#endif /* NETWORK_H */
