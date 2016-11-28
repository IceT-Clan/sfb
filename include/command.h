#ifndef COMMAND_H
#define COMMAND_H

#include <iostream>
#include <string>
#include <map>
#include "sfb.h"
#include "serial/serial.h"
#include "network.h"

#ifdef _WIN32
	#include "dirent.h"
	#include "direct.h"
#else
	#include <dirent.h>
#endif

using namespace std;
using namespace serial;

class Command {
private:
	// Arguments
	int						argc;
	char**					argv;

	// Network
	Network*				net;

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
		{"cd", CMD_CD},
		{"pwd", CMD_PWD},
		{"mkdir", CMD_MKDIR},
		{"touch", CMD_TOUCH},
		{"rm", CMD_RM}
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
	bool	changedirectory();
	bool	printworkingdirectory();
	bool	makedirectory();
	bool	makefile();
	bool	remove();
};

#endif /* COMMAND_H */
