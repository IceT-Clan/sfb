#include "command.h"

Command::Command() {
}

Command::~Command() {

}

bool Command::read(int argc, char* argv[]) {
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

	if (cmd == CMD_NONE) {
		// No command to execute
		return false;
	}

	DIR* dirp = opendir("c:\\");
	while ((entry = readdir(dirp)))
		cout << entry->d_type << "\t" << entry->d_namlen << "\t" << entry->d_reclen << "\t" << entry->d_name << endl;

	closedir(dirp);

	return true;
}

bool Command::start(string port) {
	return true;
}

bool Command::cp(string origin, string destination) {
	return true;
}

bool Command::mv(string origin, string destination) {
	return true;
}

bool Command::ls() {
	return true;
}

bool Command::la() {
	return true;
}
