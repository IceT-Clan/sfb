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

		bool		readfileinfos(string path);

		bool		init(string port);
		bool		send(REQ_PACKET* req);

		string getfilename(string path);
				
		DATA_PACKET* recv();

		template <class temp>
		bool		sendraw(const temp &pkt);

		bool		sendpkt(REQ_PACKET &pkt);
		bool		sendpkt(INFO_PACKET &pkt);
		bool		sendpkt(CONF_PACKET &pkt);
		bool		sendpkt(DATA_PACKET &pkt);
};

#endif /* NETWORK_H */
