#include <iostream>
#include <string>
#include <Windows.h>
#include "sfb.h"
#include "command.h"

using namespace std;

int main(int argc, char** argv) {


	Command cmd(argc, argv);
	
	/*cout << "PRESS A FANCY KEY TO SEND THE INFOPACKET\n";
	system("Pause");
	Network net;
	net.init("COM1");
	
	INFO_PACKET pkt1;
	pkt1.bytesnr = 23;
	net.sendpkt(pkt1);
	
	REQ_PACKET pkt;
	pkt.cmd = CMD_MOVE;
	pkt.path0 = "VON\\LE\\SCHAFT";
	pkt.path1 = "IN\\LE\\PUSS";
	net.sendpkt(pkt);

	CONF_PACKET pkt3;
	pkt3.confirmation = true;
	net.sendpkt(pkt3);

	DATA_PACKET pkt4;
	pkt4.bytes[1] = 23;
	pkt4.bytes[6] = 26;
	pkt4.bytes[7] = 27;
	net.sendpkt(pkt4);
	cout << pkt4.checksum << endl ;
	system("Pause");*/

	// Print version
	cout << "sfb version " << SFB_VERSION << endl;

	// Read command
	if (!cmd.read()) {
		cout << "Command not found." << endl;
	}

	if (cmd.exec()) {
		cout << "Executed successfully." << endl;
	} else {
		cout << "Could not execute." << endl;
	}
	return 0;
}
