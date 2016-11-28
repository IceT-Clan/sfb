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

	if (argc < 3) {
		// No port given, use default
#ifdef _WIN32
		port = "COM1";
#else
		port = "/dev/ttyS0";
#endif
	} else {
		// Use given port
		port = argv[2];
	}

	// Set up network class
	net = new Network();

	// Check if port is available and return result
	return net->init(port);
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
	REQ_PACKET			move;

	//Build move packet
	move.cmd =			CMD_MOVE;
	move.path0 =		argv[2];
	move.path1 =		argv[3];

	//send packet
	net->send(&move);

	net->recv();


	return true;
}

bool Command::list() {
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
	return true;
}

bool Command::listall() {
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
	return true;
}

bool Command::changedirectory() {

	if (argv[2][0] == ':') {
		//Change local directory
		chdir(argv[2] + 1);
	}

	REQ_PACKET						changedirectory;

	//Build changedirectory packet
	changedirectory.cmd =			CMD_CD;
	changedirectory.path0 =			argv[2];

	//send packet
	net->send(&changedirectory);

	net->recv();


	return true;
}

bool Command::printworkingdirectory() {
	REQ_PACKET						pwd;

	//Build printworkingdirectory packet
	pwd.cmd = CMD_PWD;

	//send packet
	net->send(&pwd);

	net->recv();
	return true;
}

bool Command::makedirectory() {
	if (argv[2][0] == ':') {
#ifdef _WIN32
		string option = argv[2] + 1;
		string command = "mkdir " + option;
		system((const char*)command.c_str());
#else

#endif
	}
	return true;
}

bool Command::makefile() {
	if (argv[2][0] == ':') {
		string option = argv[2] + 1;
#ifdef _WIN32
		string command = "copy nul > " + option;
		system((const char*)command.c_str());
#else
		string command = "touch " + option;
		system(command);
#endif
	}
	return true;
}

bool Command::remove() {
	if (argv[2][0] == ':') {
		if (argv[2][0] == '-rf' || argv[2][0] == '-rF') {
			string option = argv[2] + 1;
#ifdef _WIN32
			string command = "rmdir" + option;
			system((const char*)command.c_str());
#else
			if (argv[2][0] == '-r') {
				string command = "rm -r " + option;
				system(command);
			}
			string command = "rm -rf " + option;
			system(command);
#endif
		}
		string option = argv[2] + 1;
#ifdef _WIN32
		string command = "del " + option;
		system((const char*)command.c_str());
#else
		string command = "rm " + option;
		system(command);
#endif
	}
	return true;
}
