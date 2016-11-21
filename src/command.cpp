#include <iomanip>
#include "command.h"

Command::Command(int argc, char** argv) {
	this->argc = argc;
	this->argv = argv;
}

Command::~Command() {
}

bool Command::read() {
	map<string, COMMANDS>::iterator it;

	if (argc < 2) {
		it = cmd_map.find("");
		// No command given, fallback to default
		if (it == cmd_map.end()) {
			// No default command set
			return false;
		}

		// Set cmd to default command
		cmd = it->second;

		return true;
	}

	// Determine which command was entered
	it = cmd_map.find(argv[1]);
	if (it == cmd_map.end()) {
		// Command not found
		return false;
	}

	// Set cmd to given command
	cmd = it->second;

	return true;
}

bool Command::exec() {
	struct dirent *entry;

	// Decide which command should be executed
	switch (cmd) {
		case CMD_NONE:
			// No command to execute
			return false;
		case CMD_HELP:
			print_help();
			break;
		case CMD_START:
			start();
			break;
		case CMD_COPY:
			copy();
			break;
		case CMD_MOVE:
			move();
			break;
		case CMD_LS:
			list();
			break;
		case CMD_LA:
			listall();
			break;
		case CMD_CD:
			changedirectory();
			break;
		case CMD_PWD:
			printworkingdirectory();
			break;
		default:
			// Something went really wrong...
			return false;
	}

	return true;
}

bool Command::print_help() {
	cout << endl << "Usage: " << argv[0] << endl << 
		"[help] " << "[start <port>] " << "[cp <path> <destination>] " << "[mv <path> <destination>]" << endl << 
		"[ls [path]] " << "[la [path]] " << "[cd <path>] " << "[pwd]" << endl <<
		"Options:" << endl <<
		"help   | help                      Print this help" << endl <<
		"start  | start                     Start background deamon to establish a connection" << endl <<
		"                                   to another computer through your serial connection" << endl <<
		"cp     | copy                      Copy one or more files to another location" << endl <<
		"mv     | move                      Move or rename files or directories" << endl <<
		"ls     | list                      List information about files" << endl <<
		"la     | list all                  List more specific informations about (hidden) files" << endl <<
		"cd     | changedirectory           Change the current working directory to a specific Folder" << endl <<
		"pwd    | print working directory   Print the current directory" << endl << endl <<
		"More informations on https://github.com/IceT-Clan/sfb" << endl << 
		"or visit us on https://icet-clan.de/" << endl;

	return true;
}

bool Command::start() {
	string port;

	//Check if there is a port given
	if (argv[2] == "") {
		//Check for Linux or Windows
		if (argv[0] == "sfb" || argv[0] == "./sfb") port = "/dev/ttyS0";
		else { port = "COM1"; }
	}
	//Use given port
	else { port = argv[2]; }

	//Ändern hab die default vorschäge genutzt!!!
	//Serial serial(port, 9600, Timeout::simpleTimeout(250), eightbits, PARITY_NONE, ONESTOPBIT, flowcontrol_none);

	cout << "Serial init complete on Port " << port << endl;
	
	return true;
}

bool Command::copy() {
	return true;
}

bool Command::move() {
	return true;
}

bool Command::list() {
	return true;
}

bool Command::listall() {
	return true;
}

bool Command::changedirectory() {
	return true;
}

bool Command::printworkingdirectory() {
	return true;
}
