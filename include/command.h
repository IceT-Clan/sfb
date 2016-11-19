#include <iostream>
#include <string>
#include <map>
#include "sfb.h"
#ifdef _WIN32
	#include "dirent.h"
#else
	#include <dirent.h>
#endif

using namespace std;

class Command {
public:
	enum COMMANDS {
		CMD_NONE,
		CMD_HELP,
		CMD_START,
		CMD_COPY,
		CMD_MOVE,
		CMD_LS,
		CMD_LA,
	};
private:
	// Arguments
	int						argc;
	char**					argv;

	// Commands
	COMMANDS				cmd;
	map<string, COMMANDS>	cmd_map = {
		{"", CMD_HELP},
		{"help", CMD_HELP},
		{"start", CMD_START},
		{"cp", CMD_COPY},
		{"mv", CMD_MOVE},
		{"ls", CMD_LS},
		{"la", CMD_LA},
	};
public:
	Command(int argc, char** argv);
	~Command();

	bool	read();
	bool	exec();
private:
	// Methods for all commands
	bool	print_help();
	bool	start();
	bool	copy();
	bool	move();
	bool	list();
	bool	listall();
};
