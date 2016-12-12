#include "command.h"

Command::Command(int argc, char** argv) {
	string port;

	this->argc = argc;
	this->argv = argv;

	// Set up network class
	this->net = new Network();

	// Check if default port is available
	if (!net->init()) {
		cerr << "Failed to init network" << endl;
	}

}

Command::~Command() {
	delete net;
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
			return moveOrCopy(false);
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
			return moveOrCopy(true);
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
		"[ls [path]] " << "[la [path]] " << "[cd <path>] " << "[pwd]" << "[mkdir <path>]" << "[touch <path>]" << "[rm <path>]" << endl <<
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

	if (!port.empty()) {
		net->init(port);
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

	cout << "Start listening for commands..." << endl;
	while (true) {
		bool result = false;
		while (!net->getrequestPacketAvailable()) {}
		REQ_PACKET pkt =  net->getrequestpacket();
		switch (pkt.cmd) {
		case CMD_MOVE:
			result = moveOrCopy_b(pkt, true);
			break;
		case CMD_COPY:
			result = moveOrCopy_b(pkt, false);
			break;
		default:
			cerr << "Unknown command!" << endl;
			break;
		}
		if(result)	cout << "Executed successfully." << endl;
		else		cout << "Could not execute." << endl;
	}
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
					//Set font color blue
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
					//Set font color blue
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
	net->sendpkt(changedirectory);

	net->recv();


	return true;
}

bool Command::printworkingdirectory() {
	REQ_PACKET pwd;

	//Build printworkingdirectory packet
	pwd.cmd = CMD_PWD;

	//send packet
	net->sendpkt(pwd);

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

bool Command::removefile() {
	string command, option;

	if (argv[2][0] == ':') {
#ifdef _WIN32
		 command = "del " + option;
#else
		 command = "rm -rf" + option;
#endif
		system((const char*)command.c_str());
	}
	return true;
}

bool Command::checkFileExists(string name) {
	ifstream target;
	target.open(argv[3], ios_base::in | ios_base::binary);
	if (!target.is_open()) {
		string error = strerror(errno);
		if (error.compare("No such file or directory") == 0) {
			return false;
		}
	}
	target.close();

	vector<string> files;
	list_files(&files, ".");
	if(find(files.begin(), files.end(), name) == files.end()) {
		// File already exists
		cout << "File '" << name << "' with the same name already exists! do you want to override it? [y/N]" << endl;
		string input;
		cin >> input;
		if (input == "y" | input == "Y") {
			return true;
		}
	}

	 if (std::remove(name.c_str()) != 0) {
	 	perror("Error deleting the file");
	 	return true;
	 }
	 
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
	string programPath = "/sfb.exe";

	pid_t pid = fork(); // Create a child process

	switch (pid) {
	case -1: // Error
		cerr << "Uh-Oh! fork() failed.\n";
		exit(1);
	case 0: // Child process
		execl(programPath.c_str(), NULL); // Execute the program
		cerr << "Uh-Oh! execl() failed!"; // execl doesn't return unless there's an error
		exit(1);
	default: // Parent process
		cout << "Process created with pid " << pid << "\n";
		int status;

		while (!WIFEXITED(status)) {
#ifdef _WIN32
			waitpid(pid, status, 0); // Wait for the process to complete
#else
			// TODO: Get it working not only on windows only every time you write code...
#endif
		}

		cout << "Process exited with " << WEXITSTATUS(status) << "\n";

		return 0;
	}
}
#endif

/*
 * List files and directories within a directory.
 */
void Command::list_files(vector<string>* files, const char* dirname) {
    DIR *dir;
    struct dirent *ent;
                
    // Open directory stream
    dir = opendir(dirname);
    if (dir != NULL) {
        // Print all files and directories within the directory
        while ((ent = readdir(dir)) != NULL) {
			files->push_back(ent->d_name);
        }

		// Close directory
		closedir(dir);
    } else {
        // Could not open directory
        cerr << "Cannot open directory " << dirname << endl;
    }
	return;
}

bool Command::moveOrCopy(bool move) {
	//Paths
	string sourcePath(argv[2]);
	string targetPath(argv[3]);

	bool isSourceHere = false;
	bool isTargetHere = false;

	//Test for and remove ':'
	if (sourcePath[0] == ':') {
		isSourceHere = true;
		sourcePath.erase(0, 1);
	}
	if (targetPath[0] == ':') {
		isTargetHere = true;
		targetPath.erase(0, 1);
	}
	
	if (isSourceHere) {
		if (isTargetHere) {	// All files on this pc
			return handleFile(sourcePath, targetPath, move);
		}
		else {				// Send source file
			return sendFile(sourcePath, move);
		}
	}
	else {
		if (isTargetHere) {	// Recieve file
			return recvFile(targetPath, move);
		}
		else {				// All files on other pc
			REQ_PACKET pkt{ move ? CMD_MOVE : CMD_COPY, sourcePath, targetPath };
			return true;
		}
	}

}

bool Command::moveOrCopy_b(REQ_PACKET& pkt, bool move) {
	bool isSourceHere = pkt.path0 != "";
	bool isTargetHere = pkt.path1 != "";

	if (isSourceHere) {
		if (isTargetHere) {	// All files on this pc
			return handleFile(pkt.path0, pkt.path1, move);
		}
		else {				// Send source file
			return sendFile(pkt.path0, move);
		}
	}
	else {
		if (isTargetHere) {	// Recieve file
			return recvFile(pkt.path1, move);
		}
		else {				// All files on other pc
			REQ_PACKET pkt{ move ? CMD_MOVE : CMD_COPY, pkt.path0, pkt.path1 };
			return true;
		}
	}
}

bool Command::sendFile(string path, bool move) {
	//file
	ifstream source;

	//Open file
	source.open(path, ios_base::in | ios_base::binary);
	if (!source.is_open()) {
		cerr << "Error opening source: " << strerror(errno) << endl;
		return false;
	}

	// Send file size
	INFO_PACKET iPkt;
	source.seekg(0, std::ios_base::end);
	iPkt.bytesnr = source.tellg();
	source.seekg(0, std::ios_base::beg);
	net->sendpkt(iPkt);

	// Receive confirmation
	CONF_PACKET cPkt = net->getconfpacket();
	if (!cPkt.confirmation) {
		cerr << "Connected computer denied moving the file" << endl;
		return false;
	}
	DATA_PACKET dPkt;
	while (!source.eof()) {
		source.read(reinterpret_cast<char*>(dPkt.bytes), 252);
		if (!source) break;
		net->sendpkt(dPkt);
	}
	for (size_t i = source.gcount(); i < 252; i++)
		dPkt.bytes[i] = '\0';
	net->sendpkt(dPkt);

	// Remove source
	if (move) {
		if (remove(path.c_str()) != 0) {
			cerr << "Could not delete source file" << endl;
			return true;
		}
	}
}

bool Command::recvFile(string path, bool move) {
	//file
	ofstream target;

	//Open file
	// Test target file
	if (checkFileExists(path)) {
		return false;
	}
	target.open(path, ios_base::out | ios_base::binary);
	if (!target.is_open()) {
		cerr << "Error opening target: " << strerror(errno) << endl;
		return false;
	}

	INFO_PACKET iPkt = net->getinfopacket();
	unsigned long long freeSpace;
#ifdef _WIN32
	ULARGE_INTEGER li;
	if (!GetDiskFreeSpaceEx(path.substr(0, 3).c_str(), &li, NULL, NULL)) {
		// https://msdn.microsoft.com/de-de/library/windows/desktop/ms680582(v=vs.85).aspx
		LPVOID lpMsgBuf;
		DWORD dw = GetLastError();

		FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM | FORMAT_MESSAGE_IGNORE_INSERTS, NULL,
			dw, MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&lpMsgBuf, 0, NULL);
		cerr << "Error getting free disc space: " << lpMsgBuf << endl;
		LocalFree(lpMsgBuf);
		target.close();
		return false;
	}
	freeSpace = li.QuadPart;
#else
	static_assert(true, "Not implemenmted: Linux free disc space function");
#endif
	CONF_PACKET cPkt;
	cPkt.confirmation = freeSpace >= iPkt.bytesnr;
	net->sendpkt(cPkt);
	if (!cPkt.confirmation) {
		cerr << "Not enough free space on disc" << endl;
		target.close();
		return false;
	}
	DATA_PACKET dPkt;
	unsigned long long packets = iPkt.bytesnr / 252ULL;
	while (packets) {
		dPkt = net->getdatapacket();
		size_t recievedCheckSum = dPkt.checksum;
		Network::createCheckSum(dPkt);
		if (recievedCheckSum != dPkt.checksum) {
			cerr << "Error on sending!" << endl;
			target.close();
			return false;
		}
		target.write(reinterpret_cast<char*>(dPkt.bytes), 252);
	}
}

bool Command::handleFile(string sourceP, string targetP, bool move) {
	//files
	ifstream source;
	ofstream target;

	//Open files
	source.open(sourceP, ios_base::in | ios_base::binary);
	if (!source.is_open()) {
		cerr << "Error opening source: " << strerror(errno) << endl;
		return false;
	}
	// Test target file
	if (checkFileExists(targetP)) {
		source.close();
		return false;
	}
	target.open(targetP, ios_base::out | ios_base::binary);
	if (!target.is_open()) {
		cerr << "Error opening target: " << strerror(errno) << endl;
		return false;
	}

	// Copy source to target
	target << source.rdbuf();

	// Remove source
	if (move) {
		if (remove(targetP.c_str()) != 0) {
			cerr << "Could not delete source file." << endl;
		}
	}
	return true;
}