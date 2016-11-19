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
		default:
			// Something went really wrong...
			return false;
	}

	return true;
}

bool Command::print_help() {
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
