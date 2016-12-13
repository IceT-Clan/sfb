#ifndef COMMAND_H
#define COMMAND_H

#include <iostream>
#include <string>
#include <map>
#include <algorithm>
#include "sfb.h"
#include "serial/serial.h"
#include "network.h"
#ifdef _WIN32
	#include "dirent.h"
	#include "direct.h"
#else
	#include <dirent.h>
	#include <unistd.h>
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
		{"man", CMD_HELP},
		{"manual", CMD_HELP},
		{"start", CMD_START},
		{"cp", CMD_COPY},
		{"copy", CMD_COPY},
		{"mv", CMD_MOVE},
		{"mov", CMD_MOVE},
		{"move", CMD_MOVE},
		{"ls", CMD_LS},
		{"list", CMD_LS},
		{"la", CMD_LA},
		{"listall", CMD_LA},
		{"cd", CMD_CD},
		{"changedirectory", CMD_CD},
		{"pwd", CMD_PWD},
		{"printworkingdirectory", CMD_PWD},
		{"mkdir", CMD_MKDIR},
		{"makedirectory", CMD_MKDIR},
		{"touch", CMD_TOUCH},
		{"create", CMD_TOUCH},
		{"rm", CMD_RM},
		{"remove", CMD_RM},
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
	bool	move_b(REQ_PACKET& pkt);
	bool	ls_b(REQ_PACKET & pkt);
	bool	la_b(REQ_PACKET & pkt);
	bool	mkdir_b(REQ_PACKET & pkt);
	bool	pwd_b(REQ_PACKET & pkt);
	bool	rm_b(REQ_PACKET & pkt);
	bool	makefile_b(REQ_PACKET & pkt);
	bool	moveOrCopy(bool move);
	bool	moveOrCopy_b(REQ_PACKET& pkt, bool move);
	bool	sendFile(string path, bool move);	// Move or copies file from this pc to other pc
	bool	recvFile(string path, bool move);	// Move or copies file from other pc to this pc
	bool	handleFile(string sourceP, string targetP, bool move);	// Move or copies file on this pc
	bool	list();
	bool	listall();
	bool	changedirectory();
	bool	printworkingdirectory();
	bool	makedirectory();
	bool	makefile();
	bool	removefile();
	bool	startInBackground(string port, bool hideConsole);
	/**
	*Checks if a file exists
	*
	*returns true when the file exists after the method returns
	*/
	bool	checkFileExists(string name, bool question);
	void	list_files(vector<string>* files, const char* dirname);

	void	OutErrror(string err);
};

#endif /* COMMAND_H */
