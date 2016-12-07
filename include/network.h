#ifndef NETWORK_H
#define NETWORK_H

#include <iostream>
#include <string>
#include <map>
#include <vector>
#include <thread>
#include <mutex>
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

		REQ_PACKET		requestPacket;
		DATA_PACKET		dataPacket;
		CONF_PACKET		confPacket;
		INFO_PACKET		infoPacket;
		bool			requestPacketAvailable;
		bool			dataPacketAvailable;
		bool			confPacketAvailable;
		bool			infoPacketAvailable;

		thread*			receive;
		mutex			sec;
	public:
		Network();
		~Network();

		bool			readfileinfos(string path);

		bool			init(string port);
		bool			send(REQ_PACKET* req);

		string			getfilename(string path);
				
		bool			recv();

		REQ_PACKET		getrequestpacket();
		DATA_PACKET		getdatapacket();
		CONF_PACKET		getconfpacket();
		INFO_PACKET		getinfopacket();

		bool			getrequestPacketAvailable();
		bool			getdataPacketAvailable();
		bool			getconfPacketAvailable();
		bool			getinfoPacketAvailable();

		template <class temp>
		bool		sendraw(const temp &pkt);

		bool		sendpkt(REQ_PACKET &pkt);
		bool		sendpkt(INFO_PACKET &pkt);
		bool		sendpkt(CONF_PACKET &pkt);
		bool		sendpkt(DATA_PACKET &pkt);
};

#endif /* NETWORK_H */
