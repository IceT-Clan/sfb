#include "command.h"

Command::Command() {
}

Command::~Command() {

}

bool Command::read(int argc, char* argv[]) {
	if (argc < 2) {
		// No command given, fallback to default
		if (cmd_map.find("") == cmd_map.end()) {
			// No default command set
			return false;
		}

		// Set cmd to default command
		cmd = cmd_map.find("")->second;

		return true;
	}

	// Determine which command was entered
	if (cmd_map.find(argv[1]) == cmd_map.end()) {
		// Command not found
		return false;
	}

	// Set cmd to given command
	cmd = cmd_map.find(argv[1])->second;

	return true;
}

bool Command::exec() {
	if (cmd == CMD_NONE) {
		// No command to execute
		return false;
	}

	return true;
}
