#include "command.h"

Command::Command(int argc, char** argv) {
	string port;

	this->argc = argc;
	this->argv = argv;

	// Set up network class
	this->net = new Network();

	// Check if default port is available
	if (!net->init("COM1")) {
		CERR( "Failed to init network" << endl);
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

	string arg;
	bool debug = false;
	if (string(argv[argc - 1]).compare("-d") == 0) {
		debug = true;
		argc--;
	}
	net->setDebug(debug);

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
				CERR( "Not enough arguments were given!" << endl <<
					"Usage: " << argv[0] << " cp <path> <destination>" << endl);
				return false;
			}
			if (argc > 4) {
				CERR( "Too many arguments given!" << endl <<
					"Usage: " << argv[0] << " cp <path> <destination>" << endl);
				return false;
			}
			return moveOrCopy(false);
		case CMD_MOVE:
			if (argc < 4) {
				CERR( "Not enough arguments were given!" << endl <<
					"Usage: " << argv[0] << " mv <path> <destination>" << endl);
				return false;
			}
			if (argc > 4) {
				CERR( "Too many arguments given!" << endl <<
					"Usage: " << argv[0] << " mv <path> <destination>" << endl);
				return false;
			}
			return moveOrCopy(true);
		case CMD_LS:
			if (argc > 3) {
				CERR( "Too many arguments given!" << endl <<
					"Usage: " << argv[0] << " ls [path]" << endl);
			}
			return list();
		case CMD_LA:
			if (argc > 3) {
				CERR( "Too many arguments given!" << endl <<
					"Usage: " << argv[0] << " la [path]" << endl);
			}
			return listall();
		case CMD_PWD:
			if (argc > 2) {
				CERR( "Too many arguments given!" << endl <<
					"Usage: " << argv[0] << " pwd" << endl);
				return false;
			}
			return printworkingdirectory();
		case CMD_MKDIR:
			if (argc < 3) {
				CERR( "Not enough arguments were given!" << endl <<
					"Usage: " << argv[0] << " mkdir <path>" << endl);
				return false;
			}
			if (argc > 3) {
				CERR( "Too many arguments given!" << endl <<
					"Usage: " << argv[0] << " mkdir <path>" << endl);
				return false;
			}
			return makedirectory();
		case CMD_TOUCH:
			if (argc < 3) {
				CERR( "Not enough arguments were given!" << endl <<
					"Usage: " << argv[0] << " touch <path>" << endl);
				return false;
			}
			if (argc > 3) {
				CERR( "Too many arguments given!" << endl <<
					"Usage: " << argv[0] << " touch <path>" << endl);
				return false;
			}
			return makefile();
		case CMD_RM:
			if (argc < 3) {
				CERR( "Not enough arguments were given!" << endl <<
					"Usage: " << argv[0] << " rm <path>" << endl);
				return false;
			}
			if (argc > 3) {
				CERR( "Too many arguments given!" << endl <<
					"Usage: " << argv[0] << " rm <path>" << endl);
				return false;
			}
			return removefile();
		default:
			// Something went really wrong...
			return false;
	}

	return true;
}

bool Command::print_help() {
	COUT(endl << "Usage: " << argv[0] << endl << 
		"[help] " << "[start <port>] " << "[cp <path> <destination>] " << "[mv <path> <destination>]" << endl << 
		"[ls [path]] " << "[la [path]] " << "[pwd] " << "[mkdir <path>] " << "[touch <path>] " << "[rm <path>]" << endl <<
		"Options:" << endl <<
		"help   | help                      Print this help" << endl <<
		"start  | start                     Start background deamon to establish a connection" << endl <<
		"                                   to another computer through your serial connection" << endl <<
		"cp     | copy                      Copy one or more files to another location" << endl <<
		"mv     | move                      Move or rename files or directories" << endl <<
		"ls     | list                      List information about files" << endl <<
		"la     | list all                  List more specific informations about (hidden) files" << endl <<
		"pwd    | print working directory   Print the current directory" << endl <<
		"mkdir  | make directory            Create a directory at the given path" << endl <<
		"touch  | create file               Create a file at the given path" << endl <<
		"rm     | remove file               Remove a file or directory" << endl << endl << 
		"More informations on https://github.com/IceT-Clan/sfb or visit us on https://icet-clan.de/" << endl);

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
				CERR( "Invalid arguments" << endl);
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

	COUT("Start listening for commands..." << endl);
	while (true) {
		bool result = false;
		while (!net->getrequestPacketAvailable()) {}
		REQ_PACKET pkt =  net->getrequestpacket();
		switch (pkt.cmd) {
		case CMD_MOVE:
			COUT("MOVE command started" << endl);
			result = moveOrCopy_b(pkt, true);
			break;
		case CMD_COPY:
			COUT("COPY command started" << endl);
			result = moveOrCopy_b(pkt, false);
			break;
		case CMD_LS:
			COUT("LS command started" << endl);
			result = ls_b(pkt);
			break;
		case CMD_LA:
			COUT("LA command started" << endl);
			result = la_b(pkt);
			break;
		case CMD_PWD:
			COUT("PWD command started" << endl);
			result = pwd_b(pkt);
			break;
		case CMD_MKDIR:
			COUT("MKDIR command started" << endl);
			result = mkdir_b(pkt);
			break;
		case CMD_RM:
			COUT("Remove command started" << endl);
			result = rm_b(pkt);
			break;
		case CMD_TOUCH:
			COUT("Touch command started" << endl);
			result = makefile_b(pkt);
			break;
		default:
			CERR( "Unknown command!" << endl);
			break;
		}
		if(result)	COUT("Executed successfully." << endl)
		else		COUT("Could not execute." << endl);
	}
}

bool Command::move_b(REQ_PACKET& pkt) {
	if (pkt.path0 != "" && pkt.path1 != "") {
		ifstream source(pkt.path0, ios::binary | ios::in);
		if (!source.is_open()) {
			CERR( "Can't open source file!" << endl);
			return false;
		}
		ofstream target(pkt.path1, ios::binary | ios::out | ios::trunc);
		if (!target.is_open()) {
			CERR( "Can't open target file!" << endl);
			return false;
		}

		// Copy source to target
		target << source.rdbuf();

		// Remove source
		if (remove(pkt.path0.c_str()) != 0) {
			CERR( "Could not delete file(s)." << endl);
			return true;
		}
	}

	return true;
}

bool Command::ls_b(REQ_PACKET& pkt) {
	DIR *dir;
	struct dirent *ent;
	stringstream msg;
	string msg_la;

	//Open directory stream
	dir = opendir((pkt.path0).c_str());
	if (dir != NULL) {

		//Print all files and directories within the directory
		while ((ent = readdir(dir)) != NULL) {
			switch (ent->d_type) {
			case DT_DIR:
				msg << ent->d_name << "\\  ";
				break;
			default:
				msg << ent->d_name << "  ";
				break;
			}
		}

		closedir(dir);

	}
	else {
		//Could not open directory
		COUT("Cannot open directory " << (pkt.path0).c_str() << endl);
		exit(EXIT_FAILURE);
	}

	msg_la = msg.str();
	MSG_PACKET asw_pkt;
	asw_pkt.msg = msg_la;

	net->sendpkt(asw_pkt);
	return true;
}

bool Command::la_b(REQ_PACKET& pkt) {
	DIR *dir;
	struct dirent *ent;
	stringstream msg;
	string msg_la;

	//Open directory stream
	dir = opendir((pkt.path0).c_str());
	if (dir != NULL) {

		//Print all files and directories within the directory
		while ((ent = readdir(dir)) != NULL) {
			switch (ent->d_type) {
			case DT_DIR:
				msg << ent->d_reclen << " " << ent->d_type << " " << ent->d_namlen << "\t" << ent->d_name << "\\" << endl;
				break;
			default:
				msg << ent->d_reclen << " " << ent->d_type << " " << ent->d_namlen << "\t" << ent->d_name << endl;
				break;
			}
		}

		closedir(dir);

	}
	else {
		//Could not open directory
		COUT("Cannot open directory " << (pkt.path0).c_str() << endl);
		exit(EXIT_FAILURE);
	}
	msg_la = msg.str();
	MSG_PACKET asw_pkt;
	asw_pkt.msg = msg_la;

	net->sendpkt(asw_pkt);
	return true;
}

bool Command::mkdir_b(REQ_PACKET& pkt) {
	string command, option;

	/*option = (pkt.path0).c_str(); 
	command = "mkdir " + option;
	system((const char*)command.c_str());*/
	if (_mkdir(pkt.path0.c_str())) {
		cout << "Directory " << pkt.path0 << " successfully created!" << endl;
		CONF_PACKET conf;
		conf.confirmation = CONFIRMATION::OK;
		net->sendpkt(conf);
		return true;
	}
	else {
		CONF_PACKET conf;
		conf.confirmation = CONFIRMATION::ERROR_CREATING;
		net->sendpkt(conf);
		cout << "Cant create Directory " << pkt.path0 << " !" << endl;
		return false;
	}	
}

bool Command::pwd_b(REQ_PACKET& pkt) {
#ifdef _WIN32
	char buffer[MAX_PATH];
	GetModuleFileName(NULL, buffer, MAX_PATH);
#else
	char buffer[PATH_MAX];
	readlink("/proc/self/exe", buffer, sizeof(buffer) - 1);
#endif
	string::size_type pos = string(buffer).find_last_of("\\/");
	string msg = string(buffer).substr(0, pos);

	MSG_PACKET asw_pkt;
	asw_pkt.msg = msg;

	net->sendpkt(asw_pkt);
	return true;
}

bool Command::rm_b(REQ_PACKET& pkt) {
	string command;

#ifdef _WIN32
	/*command = "del " + pkt.path0;
	system((const char*)command.c_str());*/
	std::remove(pkt.path0.c_str());
	_rmdir(pkt.path0.c_str());
#else
	command = "rm -rf" + pkt.path0;
	system((const char*)command.c_str());
#endif	

	CONF_PACKET conf;
	conf.confirmation = CONFIRMATION::OK;
	net->sendpkt(conf);

	return true;
}

bool Command::makefile_b(REQ_PACKET& pkt) {
	string command, option;
	option = (pkt.path0).c_str();
#ifdef _WIN32
	command = "copy nul > " + option;
#else
	command = "touch " + option;
#endif
	system((const char*)command.c_str());

	CONF_PACKET conf;
	conf.confirmation = CONFIRMATION::OK;
	net->sendpkt(conf);

	return true;
}

bool Command::copy() {
	REQ_PACKET			copy;

	//Build copy packet
	copy.cmd = CMD_COPY;
	copy.path0 = argv[2];
	copy.path1 = argv[3];

	//send packet
	net->sendpkt(copy);

	return true;
}

bool Command::move() {
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

	if (!(isSourceHere && isTargetHere)) {
		REQ_PACKET pkt{ CMD_MOVE, "", "" };
		if (!isSourceHere) pkt.path0;
		if (!isTargetHere) pkt.path1;
		net->sendpkt(pkt);
		return true;
	}

	// Test target file
	if (checkFileExists(argv[3], true)) return false;

	//files
	ifstream source;
	ofstream target;

	//Open files
	source.open(argv[2], ios_base::in | ios_base::binary);
	if (!source.is_open()) {
		OutError("Error opening source");
	}
	target.open(argv[3], ios_base::out | ios_base::binary);
	if (!target.is_open()) {
		OutError("Error opening target");
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

				case DT_DIR:
					//Set font color blue
					SetConsoleTextAttribute(hConsole, 1);
					COUT(ent->d_name << "/ ");
					//Reset font color
					SetConsoleTextAttribute(hConsole, 15);
					break;
						
				default:
					COUT(ent->d_name << " ");
				}
			}

			closedir(dir);

		}
		else {
			//Could not open directory
			COUT("Cannot open directory " << argv[2] + 1 << endl);
			exit(EXIT_FAILURE);
		}
	}

	else {
		REQ_PACKET list;

		//Build listall packet
		list.cmd = CMD_LS;
		list.path0 = argv[2];

		//send packet
		net->sendpkt(list);

		MSG_PACKET msg = net->getmsgpacket();
		COUT(msg.msg);
	}

#endif
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

				case DT_DIR:
					COUT(ent->d_reclen << " " << ent->d_type << " " << ent->d_namlen << "\t");
					//Set font color blue
					SetConsoleTextAttribute(hConsole, 1);
					COUT(ent->d_name << "/" << endl);
					//Reset font color
					SetConsoleTextAttribute(hConsole, 15);
					break;

				default:
					COUT(ent->d_reclen << " " << ent->d_type << " " << ent->d_namlen << "\t" << ent->d_name << endl);
				}
			}

			closedir(dir);

		}
		else {
			//Could not open directory
			COUT("Cannot open directory " << argv[2] + 1 << endl);
			exit(EXIT_FAILURE);
		}
	}

	else {
		REQ_PACKET listall;

		//Build listall packet
		listall.cmd = CMD_LA;
		listall.path0 = argv[2];

		//send packet
		net->sendpkt(listall);

		MSG_PACKET msg = net->getmsgpacket();
		COUT(msg.msg);		
	}
	return true;
}

bool Command::printworkingdirectory() {
	REQ_PACKET pwd;

	//Build printworkingdirectory packet
	pwd.cmd = CMD_PWD;

	//send packet
	net->sendpkt(pwd);

	MSG_PACKET asw_pkt = net->getmsgpacket();
	COUT(asw_pkt.msg << endl);

	return true;
}

bool Command::makedirectory() {
	string option, command;

	if (argv[2][0] == ':') {
		option = argv[2] + 1;
		command = "mkdir " + option;
	system((const char*)command.c_str());
	}

	REQ_PACKET makedirectory;

	//Build printworkingdirectory packet
	makedirectory.cmd = CMD_MKDIR;
	makedirectory.path0 = argv[2];

	//send packet
	net->sendpkt(makedirectory);

	return true;
}

bool Command::makefile() {
	string command, option;
	if (argv[2][0] == ':') {
		checkFileExists(option, true);
		option = argv[2] + 1;
#ifdef _WIN32
		command = "copy nul > " + option;
#else
		command = "touch " + option;
#endif
		system((const char*)command.c_str());
		return true;
	}
	
	REQ_PACKET makefile;

	//Build makefile packet
	makefile.cmd = CMD_TOUCH;
	makefile.path0 = argv[2];

	//send packet
	net->sendpkt(makefile);

	return true;
}

bool Command::removefile() {
	string command, option;

	if (argv[2][0] == ':') {
#ifdef _WIN32
	//command = "del " + option;
	std::remove(argv[2] + 1);
	_rmdir(argv[2] + 1);
#else
		 command = "rm -rf" + option;
#endif
		system((const char*)command.c_str());
	}

	REQ_PACKET removefile;

	//Build makefile packet
	removefile.cmd = CMD_RM;
	removefile.path0 = argv[2];

	//send packet
	net->sendpkt(removefile);

	CONF_PACKET conf = net->getconfpacket();
	COUT(conf.confirmation);
	return true;
}

bool Command::checkFileExists(string name, bool question) {
	ifstream target;
	target.open(name, ios_base::in | ios_base::binary);
	if (!target.is_open()) {
		char errorMsg[64];
		strerror_s(errorMsg, 64, errno);
		string error(errorMsg);
		if (error.compare("No such file or directory") == 0) {
			return false;
		}
	}
	target.close();

	// File already exists
	if (question) {
		COUT("File '" << name << "' with the same name already exists! do you want to override it? [y/N]" << endl);
		string input;
		cin >> input;
		if (input == "y" | input == "Y") {
			//Delete file
			if (std::remove(name.c_str()) != 0) {
				perror("Error deleting the file");
				return true;
			}
			return false;
		}
	}
	return true;
}

#ifdef _WIN32
bool Command::startInBackground(string port, bool hideConsole) {
	LPTSTR lpCommandLine = _strdup(("sfb.exe start " + port + (hideConsole ? " -h" : "")).c_str());

	STARTUPINFO lpStartupInfo;
	PROCESS_INFORMATION lpProcessInfo;

	memset(&lpStartupInfo, 0, sizeof(lpStartupInfo));
	memset(&lpProcessInfo, 0, sizeof(lpProcessInfo));

	/* Create the process */
	if (!CreateProcess(NULL, lpCommandLine, NULL, NULL, false, CREATE_NEW_CONSOLE, NULL, NULL, &lpStartupInfo, &lpProcessInfo)) {
		CERR( "Failed to create background process" << endl);
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
		CERR( "Uh-Oh! fork() failed.\n";
		exit(1);
	case 0: // Child process
		execl(programPath.c_str(), NULL); // Execute the program
		CERR( "Uh-Oh! execl() failed!"; // execl doesn't return unless there's an error
		exit(1);
	default: // Parent process
		COUT("Process created with pid " << pid << "\n";
		int status;

		while (!WIFEXITED(status)) {
#ifdef _WIN32
			waitpid(pid, status, 0); // Wait for the process to complete
#else
			// TODO: Get it working not only on windows only every time you write code...
#endif
		}

		COUT("Process exited with " << WEXITSTATUS(status) << "\n";

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
        CERR( "Cannot open directory " << dirname << endl);
    }
	return;
}

void Command::OutError(string err) {
	char errMsg[64];
	if (strerror_s(errMsg, 64, errno) != 0) {
		CERR("[Command-Error]" << err << ": " << "unknown error" << endl)
	}
	else {
		CERR("[Command-Error]" << err << ": " << errMsg << endl);
	}
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
			REQ_PACKET pkt{ move ? CMD_MOVE : CMD_COPY, "", targetPath };
			net->sendpkt(pkt);
			return sendFile(sourcePath, move);
		}
	}
	else {
		if (isTargetHere) {	// Recieve file
			REQ_PACKET pkt{ move ? CMD_MOVE : CMD_COPY, sourcePath, "" };
			net->sendpkt(pkt);
			return recvFile(targetPath, move);
		}
		else {				// All files on other pc
			REQ_PACKET pkt{ move ? CMD_MOVE : CMD_COPY, sourcePath, targetPath };
			net->sendpkt(pkt);
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
		OutError("Error opening source");
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
	if (cPkt.confirmation != CONFIRMATION::OK) {
		if ((cPkt.confirmation & CONFIRMATION::FILE_EXISTS) != 0) {
			COUT("File with the same name already exists! do you want to override it? [y/N]" << endl);
			string input;
			cin >> input;
			CONF_PACKET cPkt;
			if (input == "y" | input == "Y") {
				cPkt.confirmation = CONFIRMATION::DELETE_FILE;
				net->sendpkt(cPkt);
			}
			else {
				cPkt.confirmation = CONFIRMATION::NOT_DELETE_FILE;
				net->sendpkt(cPkt);
				return true;
			}

		}
		if ((cPkt.confirmation & CONFIRMATION::NOT_ENOUGH_SPACE) != 0) {
			CERR("Connected computer denied moving the file: Not enough space ;)" << endl);
			return false;
		}
	}
	
	DATA_PACKET dPkt;
	while (!source.eof()) {
		source.read(reinterpret_cast<char*>(dPkt.bytes), 252);
		if (!source) break;
		net->sendpkt(dPkt);
	}
	for (size_t i = static_cast<size_t>(source.gcount()); i < 252; i++)
		dPkt.bytes[i] = '\0';
	net->sendpkt(dPkt);

	source.close();

	// Remove source
	if (move) {
		if (remove(path.c_str()) != 0) {
			OutError("Could not delete source file");
			return true;
		}
	}

	return true;
}

bool Command::recvFile(string path, bool move) {
	//file
	ofstream target;

	CONF_PACKET cPkt = { CONFIRMATION::OK };

	// Test target file
	bool isFileExisting = false;
	if (checkFileExists(path, false)) {
		cPkt.confirmation = static_cast<CONFIRMATION>(cPkt.confirmation | CONFIRMATION::FILE_EXISTS);
		isFileExisting = true;
	}
	//Open file
	target.open(path, ios_base::out | ios_base::binary);
	if (!target.is_open()) {
		OutError("Error opening target");
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
		CERR( "Error getting free disc space: " << lpMsgBuf << endl);
		LocalFree(lpMsgBuf);
		target.close();
		return false;
	}
	freeSpace = li.QuadPart;
#else
	static_assert(true, "Not implemenmted: Linux free disc space function");
#endif

	if(freeSpace < iPkt.bytesnr)
		cPkt.confirmation = static_cast<CONFIRMATION>(cPkt.confirmation | CONFIRMATION::NOT_ENOUGH_SPACE);
	net->sendpkt(cPkt);
	if (cPkt.confirmation != CONFIRMATION::OK) {
		if (isFileExisting) {
			cPkt = net->getconfpacket();
			if (cPkt.confirmation == CONFIRMATION::DELETE_FILE) {
				target.close();
				if (remove(path.c_str()) != 0) {
					OutError("Could not delete file");
					return false;
				}
				target.open(path, ios_base::out | ios_base::binary);
				if (!target.is_open()) {
					OutError("Error opening target");
					return false;
				}
			}
			else {
				target.close();
				return true;
			}
		}
		else {
			target.close();
			return false;
		}
	}
	DATA_PACKET dPkt;
	unsigned long long packetCount = (iPkt.bytesnr / 252ULL) + (iPkt.bytesnr % 252ULL ? 1 : 0);
	unsigned long long neededBytes = iPkt.bytesnr;
	unsigned long long packet = 0ULL;
	while (packet < packetCount) {
		dPkt = net->getdatapacket();
		size_t recievedCheckSum = dPkt.checksum;
		Network::createCheckSum(dPkt);
		if (recievedCheckSum != dPkt.checksum) {
			CERR( "Error on sending! Packet " << packet << "broke" << endl);
			target.close();
			return false;
		}
		target.write(reinterpret_cast<char*>(dPkt.bytes), neededBytes > 252 ? 252 : neededBytes);
		neededBytes -= 252;
		packet++;
	}

	return true;
}

bool Command::handleFile(string sourceP, string targetP, bool move) {
	//files
	ifstream source;
	ofstream target;

	//Open files
	source.open(sourceP, ios_base::in | ios_base::binary);
	if (!source.is_open()) {
		OutError("Error opening source");
		return false;
	}
	// Test target file
	if (checkFileExists(targetP, true)) {
		source.close();
		return false;
	}
	target.open(targetP, ios_base::out | ios_base::binary);
	if (!target.is_open()) {
		OutError("Error opening target");
		return false;
	}

	// Copy source to target
	target << source.rdbuf();

	// Remove source
	if (move) {
		if (remove(targetP.c_str()) != 0) {
			CERR( "Could not delete source file." << endl);
		}
	}
	return true;
}
