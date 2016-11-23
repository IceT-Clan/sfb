#include "command.h"
#include <fstream>
#include <errno.h>
#ifdef _WIN32
#include <Windows.h>
#else
#include <unistd.h>
#endif

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
			return print_help();
		case CMD_START:
			return start();
		case CMD_COPY:
			return copy();
		case CMD_MOVE:
			return move();
		case CMD_LS:
			return list();
		case CMD_LA:
			return listall();
		case CMD_CD:
			return changedirectory();
		case CMD_PWD:
			return printworkingdirectory();
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
		"More informations on https://github.com/IceT-Clan/sfb or visit us on https://icet-clan.de/" << endl;

	return true;
}

bool Command::start() {
	string port;
	bool background = false;
	if (argc > 4) return false;

	// Iterate over all arguments
	string arg;
	for (int i = 2; i < argc; i++) {
		arg = string(argv[i]);
		if (arg.compare("-b") == 0) {
			background = true;
		} else {
			if (!port.empty()) {
				cerr << "Invalid arguments" << endl;
				return false;
			}
			port = arg;
		}
	}
	if (port.empty()) {
#ifdef _WIN32
		port = "COM1";
#else
		port = "/dev/ttyS0";
#endif
	}
	if (background) {
		return startInBackground(port);
	}
	// Set up network class
	net = new Network();

	// Check if port is available and return result
	return net->init(port);
}

bool Command::copy() {
	return true;
}

bool Command::move() {
	if (argc != 4) {
		// Invalid argument count
		return false;
	}
	//Paths
	string sourcePath(argv[2]);
	string targetPath(argv[3]);

	bool isSourceHere = true;
	bool isTargetHere = true;

	//Test for and remove ':'
	if (sourcePath[0] == ':') {
		isSourceHere = false;
		sourcePath.erase(0, 1);
	}
	if (targetPath[0] == ':') {
		isTargetHere = false;
		targetPath.erase(0, 1);
	}

	// Test target file
	if (checkFile(argv[3], true)) return false;

	//files
	ifstream source;
	ofstream target;

	//Open files
	source.open(argv[2], ios_base::in | ios_base::binary);
	if (!source.is_open()) {
		cerr << "Error opening source: " << strerror(errno) << endl;
	}
	target.open(argv[2], ios_base::out | ios_base::binary);
	if (!target.is_open()) {
		cerr << "Error opening target: " << strerror(errno) << endl;
	}

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

bool Command::checkFile(string name, bool askForOverride) {
	ifstream target;
	target.open(argv[3], ios_base::in | ios_base::binary);
	if (!target.is_open()) {
		string error = strerror(errno);
		if (error.compare("No such file or directory") == 0) {
			return false;
		}
	}
	target.close();
	cout << "File with the same name already exists! Do you want to override it? [y/N]" << endl;
	string input;
	cin >> input;
	if (input.compare("y") != 0) {
		return true;
	}
	if (remove(name.c_str()) != 0) {
		perror("Error deleting the file");
		return true;
	}
	return false;
}

#ifdef _WIN32
bool Command::startInBackground(string port) {
	LPCTSTR lpApplicationName = "\"M:\\Visual Studio 2015\\Projects\\sfb\\build\\Debug\\sfb.exe\"";
	LPTSTR lpCommandLine = strdup(("start " + port + " -b").c_str());

	STARTUPINFO lpStartupInfo;
	PROCESS_INFORMATION lpProcessInfo;

	memset(&lpStartupInfo, 0, sizeof(lpStartupInfo));
	memset(&lpProcessInfo, 0, sizeof(lpProcessInfo));

	/* Create the process */
	if (!CreateProcess(lpApplicationName, lpCommandLine, NULL, NULL, false, 0, NULL, NULL, &lpStartupInfo, &lpProcessInfo)) {
		cerr << "Failed to create background process" << endl;
		return false;
	}

	return true;
}
#else
/**
* Source:
* http://www.cplusplus.com/forum/lounge/17684/
*/
bool Command::startInBackground(string port) {
	char* programPath = "/sfb.exe";

	pid_t pid = fork(); /* Create a child process */

	switch (pid) {
	case -1: /* Error */
		std::cerr << "Uh-Oh! fork() failed.\n";
		exit(1);
	case 0: /* Child process */
		execl(programPath, NULL); /* Execute the program */
		std::cerr << "Uh-Oh! execl() failed!"; /* execl doesn't return unless there's an error */
		exit(1);
	default: /* Parent process */
		std::cout << "Process created with pid " << pid << "\n";
		int status;

		while (!WIFEXITED(status)) {
			waitpid(pid, status, 0); /* Wait for the process to complete */
		}

		std::cout << "Process exited with " << WEXITSTATUS(status) << "\n";

		return 0;
	}
}
#endif