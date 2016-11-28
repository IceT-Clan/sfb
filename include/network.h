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
		//vector<uint8_t byte;
		string			filename;

		DATA_PACKET		anspacket;
	public:
		Network();
		~Network();

		bool		Network::readfileinfos(string path);

		bool		init(string port);
		bool		send(REQ_PACKET* req);

		string getfilename(string path);
				
		DATA_PACKET* recv();

		bool		send(const REQ_PACKET &pkt );
		bool		send(const INFO_PACKET &pkt);
		bool		send(const CONF_PACKET &pkt);
		bool		send(const DATA_PACKET &pkt);
};

#endif /* NETWORK_H */
