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
	cout << "Usage: " << argv[0] << " [help] " << "[start <port>] " << "[cp <path> <destination>] " << "[mv <path> <destination>]" << endl
		<< "[ls [path]] " << "[la [path]] " << "[cd <path>] " << "[pwd]" << endl <<
		"Options:" << endl <<
		setw(33) << "help	| help"						<< "Print this help" << endl <<
		setw(33) << "start	| start"					<< "Start background deamon to establish a connection" << endl <<
		setw(33) << ""									<< "to another computer through your serial connection" << endl <<
		setw(33) << "cp		| copy"						<< "Copy one or more files to another location" << endl <<
		setw(33) << "mv		| move"						<< "Move or rename files or directories" << endl <<
		setw(33) << "ls		| list"						<< "List information about files" << endl <<
		setw(33) << "la		| listall"					<< "List more specific informations about (hidden) files" << endl <<
		setw(33) << "cd		| changedirectory"			<< "Change the current working directory to a specific Folder" << endl <<
		setw(33) << "pwd	| print working directory"	<< "Print the current directory";

	return true;
}

bool Command::start() {
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
