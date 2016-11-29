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
			if (argc < 4) {
				cerr << "Not enough arguments were given!" << endl <<
					"Usage: " << argv[0] << " cp <path> <destination>" << endl;
				return false;
			}
			if (argc > 4) {
				cerr << "Too many arguments given!" << endl <<
					"Usage: " << argv[0] << " cp <path> <destination>" << endl;
				return false;
			}
			return copy();
		case CMD_MOVE:
			if (argc < 4) {
				cerr << "Not enough arguments were given!" << endl <<
					"Usage: " << argv[0] << " mv <path> <destination>" << endl;
				return false;
			}
			if (argc > 4) {
				cerr << "Too many arguments given!" << endl <<
					"Usage: " << argv[0] << " mv <path> <destination>" << endl;
				return false;
			}
			return move();
		case CMD_LS:
			if (argc > 3) {
				cerr << "Too many arguments given!" << endl <<
					"Usage: " << argv[0] << " ls [path]" << endl;
			}
			return list();
		case CMD_LA:
			if (argc > 3) {
				cerr << "Too many arguments given!" << endl <<
					"Usage: " << argv[0] << " la [path]" << endl;
			}
			return listall();
		case CMD_CD:
			if (argc < 3) {
				cerr << "Not enough arguments were given!" << endl <<
					"Usage: " << argv[0] << " cd <destination>" << endl;
				return false;
			}
			if (argc > 3) {
				cerr << "Too many arguments given!" << endl <<
					"Usage: " << argv[0] << " cd <destination>" << endl;
				return false;
			}
			return changedirectory();
		case CMD_PWD:
			if (argc > 2) {
				cerr << "Too many arguments given!" << endl <<
					"Usage: " << argv[0] << " pwd" << endl;
				return false;
			}
			return printworkingdirectory();
		case CMD_MKDIR:
			if (argc < 3) {
				cerr << "Not enough arguments were given!" << endl <<
					"Usage: " << argv[0] << " mkdir <path>" << endl;
				return false;
			}
			if (argc > 3) {
				cerr << "Too many arguments given!" << endl <<
					"Usage: " << argv[0] << " mkdir <path>" << endl;
				return false;
			}
		case CMD_TOUCH:
			if (argc < 3) {
				cerr << "Not enough arguments were given!" << endl <<
					"Usage: " << argv[0] << " touch <path>" << endl;
				return false;
			}
			if (argc > 3) {
				cerr << "Too many arguments given!" << endl <<
					"Usage: " << argv[0] << " touch <path>" << endl;
				return false;
			}
		case CMD_RM:
			if (argc < 3) {
				cerr << "Not enough arguments were given!" << endl <<
					"Usage: " << argv[0] << " rm <path>" << endl;
				return false;
			}
			if (argc > 3) {
				cerr << "Too many arguments given!" << endl <<
					"Usage: " << argv[0] << " rm <path>" << endl;
				return false;
			}
		default:
			// Something went really wrong...
			return false;
	}

	return true;
}

bool Command::print_help() {
	cout << endl << "Usage: " << argv[0] << endl << 
		"[help] " << "[start <port>] " << "[cp <path> <destination>] " << "[mv <path> <destination>]" << endl << 
		"[ls [path]] " << "[la [path]] " << "[cd <path>] " << "[pwd]" << "[mkdir <path>]" << "[touch <path>]" << "[rm [-rf] <path>]" << endl <<
		"Options:" << endl <<
		"help   | help                      Print this help" << endl <<
		"start  | start                     Start background deamon to establish a connection" << endl <<
		"                                   to another computer through your serial connection" << endl <<
		"cp     | copy                      Copy one or more files to another location" << endl <<
		"mv     | move                      Move or rename files or directories" << endl <<
		"ls     | list                      List information about files" << endl <<
		"la     | list all                  List more specific informations about (hidden) files" << endl <<
		"cd     | change directory          Change the current working directory to a specific Folder" << endl <<
		"pwd    | print working directory   Print the current directory" << endl <<
		"mkdir  | make directory            Create a directory at the given path" << endl <<
		"touch  | create file               Create a file at the given path" << endl <<
		"rm     | remove file               Remove a file or directory" << endl << endl << 
		"More informations on https://github.com/IceT-Clan/sfb or visit us on https://icet-clan.de/" << endl;

	return true;
}

bool Command::start() {
	string port;
	bool background = false;
	bool hideConsole = false;
	if (argc > 4) return false;

	// Iterate over all arguments
	string arg;
	for (int i = 2; i < argc; i++) {
		arg = string(argv[i]);
		if (arg.compare("-b") == 0) {
			background = true;
		} else if (arg.compare("-h") == 0) {
			hideConsole = true;
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
		return startInBackground(port, hideConsole);
	}
	if (!background && hideConsole) {
#ifdef _WIN32
		FreeConsole();
#else
		// TODO
#endif
	}

	while (true);
	// Set up network class
	net = new Network();

	// Check if port is available and return result
	net->init(port);

	return true;
}

bool Command::copy() {
	REQ_PACKET			copy;

	//Build copy packet
	copy.cmd =			CMD_COPY;
	copy.path0 =		argv[2];
	copy.path1 =		argv[3];

	//send packet
	net->send(&copy);

	net->recv();


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
#ifdef _WIN32
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	if (argv[2][0] == ':') {
		DIR *dir;
		struct dirent *ent;

		//Open directory stream
		dir = opendir(argv[2] + 1);
		if (dir != NULL) {

			//Print all files and directories within the directory
			while ((ent = readdir(dir)) != NULL) {
				switch (ent->d_type) {
				case DT_REG:
					cout << ent->d_name << " ";
					break;

				case DT_DIR:
					//Set font color to blue
					SetConsoleTextAttribute(hConsole, 1);
					cout << ent->d_name << "/ ";
					//Reset font color
					SetConsoleTextAttribute(hConsole, 15);
					break;

				case DT_LNK:
					cout << ent->d_name << " ";
					break;

				default:
					cout << ent->d_name << " ";
				}
			}

			closedir(dir);

		}
		else {
			//Could not open directory
			cout << "Cannot open directory " << argv[2] + 1 << endl;
			exit(EXIT_FAILURE);
		}
	}
#endif
	return true;
}

bool Command::listall() {
#ifdef _WIN32
	HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);

	if (argv[2][0] == ':') {
		DIR *dir;
		struct dirent *ent;

		//Open directory stream
		dir = opendir(argv[2] + 1);
		if (dir != NULL) {

			//Print all files and directories within the directory
			while ((ent = readdir(dir)) != NULL) {
				switch (ent->d_type) {
				case DT_REG:
					cout << ent->d_reclen << " " << ent->d_type << " " << ent->d_namlen << "\t" << ent->d_name << endl;
					break;

				case DT_DIR:
					cout << ent->d_reclen << " " << ent->d_type << " " << ent->d_namlen << "\t";
					//Set font color to blue
					SetConsoleTextAttribute(hConsole, 1);
					cout << ent->d_name << "/" << endl;
					//Reset font color
					SetConsoleTextAttribute(hConsole, 15);
					break;

				case DT_LNK:
					cout << ent->d_reclen << " " << ent->d_type << " " << ent->d_namlen << "\t" << ent->d_name << endl;
					break;

				default:
					cout << ent->d_reclen << " " << ent->d_type << " " << ent->d_namlen << "\t" << ent->d_name << endl;
				}
			}

			closedir(dir);

		}
		else {
			//Could not open directory
			cout << "Cannot open directory " << argv[2] + 1 << endl;
			exit(EXIT_FAILURE);
		}
	}
#else
	// TODO
#endif
	return true;
}

bool Command::changedirectory() {
	if (argv[2][0] == ':') {
		//Change local directory
		chdir(argv[2] + 1);
	}

	REQ_PACKET changedirectory;

	//Build changedirectory packet
	changedirectory.cmd =			CMD_CD;
	changedirectory.path0 =			argv[2];

	//send packet
	net->send(&changedirectory);

	net->recv();


	return true;
}

bool Command::printworkingdirectory() {
	REQ_PACKET pwd;

	//Build printworkingdirectory packet
	pwd.cmd = CMD_PWD;

	//send packet
	net->send(&pwd);

	net->recv();
	return true;
}

bool Command::makedirectory() {
	string option, command;

	if (argv[2][0] == ':') {
#ifdef _WIN32
		option = argv[2] + 1;
		command = "mkdir " + option;
#else
	// TODO
#endif
	system((const char*)command.c_str());
	}
	return true;
}

bool Command::makefile() {
	string command, option;

	if (argv[2][0] == ':') {
		option = argv[2] + 1;
#ifdef _WIN32
		command = "copy nul > " + option;
#else
		command = "touch " + option;
#endif
		system((const char*)command.c_str());
	}
	return true;
}

bool Command::remove() {
	string command, option;

	if (argv[2][0] == ':') {
		if (argv[2][0] == '-rf' || argv[2][0] == '-rF') {
			string option = argv[2] + 1;
#ifdef _WIN32
			command = "rmdir" + option;
#else
			if (argv[2][0] == '-r') {
				command = "rm -r " + option;
			}
			 command = "rm -rf " + option;
#endif
			system((const char*)command.c_str());
		}
		option = argv[2] + 1;
#ifdef _WIN32
		 command = "del " + option;
#else
		 command = "rm " + option;
#endif
		system((const char*)command.c_str());
	}
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

	// TODO: Fix it. This code does not compile (tested under linux)
	//		 There is no remove(const char*) method/function
	// if (remove(name.c_str()) != 0) {
	// 	perror("Error deleting the file");
	// 	return true;
	// }
	return false;
}

#ifdef _WIN32
bool Command::startInBackground(string port, bool hideConsole) {
	LPTSTR lpCommandLine = strdup(("sfb.exe start " + port + (hideConsole ? " -h" : "")).c_str());

	STARTUPINFO lpStartupInfo;
	PROCESS_INFORMATION lpProcessInfo;

	memset(&lpStartupInfo, 0, sizeof(lpStartupInfo));
	memset(&lpProcessInfo, 0, sizeof(lpProcessInfo));

	/* Create the process */
	if (!CreateProcess(NULL, lpCommandLine, NULL, NULL, false, CREATE_NEW_CONSOLE, NULL, NULL, &lpStartupInfo, &lpProcessInfo)) {
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
bool Command::startInBackground(string port, bool hideConsole) {
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
#ifdef _WIN32
			waitpid(pid, status, 0); /* Wait for the process to complete */
#else
			// TODO: Get it working not only on windows only every time you write code...
#endif
		}

		std::cout << "Process exited with " << WEXITSTATUS(status) << "\n";

		return 0;
	}
}
#endif
