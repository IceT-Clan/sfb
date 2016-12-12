#include "sfb.h"
#include "command.h"

mutex Globals::g_coutMutex;

int main(int argc, char** argv) {
	
	//if (argc == 1) {
	//	COUT("PRESS A FANCY KEY TO SEND THE INFOPACKET" << endl);
	//	system("Pause");
	//	Network net;
	//	if (!net.init("COM2"))
	//		COUT("failed to init network" << endl);

	//	INFO_PACKET pkt1;
	//	pkt1.bytesnr = 23;
	//	net.sendpkt(pkt1);

	//	REQ_PACKET pkt;
	//	pkt.cmd = CMD_MOVE;
	//	pkt.path0 = "VON\\LE\\SCHAFT";
	//	pkt.path1 = "IN\\LE\\PUSS";
	//	net.sendpkt(pkt);

	//	CONF_PACKET pkt3;
	//	pkt3.confirmation = true;
	//	net.sendpkt(pkt3);

	//	DATA_PACKET pkt4;
	//	pkt4.bytes[1] = 23;
	//	pkt4.bytes[6] = 26;
	//	pkt4.bytes[7] = 27;
	//	net.sendpkt(pkt4);
	//	COUT(pkt4.checksum << endl);
	//	system("Pause");
	//}
	//else {
		Command cmd(argc, argv);
		// Print version
		COUT("sfb version " << SFB_VERSION << endl);

		// Read command
		if (!cmd.read()) {
			COUT("Command not found." << endl);
		}

		if (cmd.exec()) {
			COUT("Executed successfully." << endl);
		} else {
			COUT("Could not execute." << endl);
		}
	//}
	return 0;
}
