#include <iostream>
#include <string>
#include <map>
#include "sfb.h"

using namespace std;

class Command {
private:
	enum COMMANDS {
		CMD_NONE,
		CMD_HELP,
		CMD_COPY,
		CMD_MOVE,
		CMD_LS,
		CMD_LA,
	};

	COMMANDS				cmd;
	map<string, COMMANDS>	cmd_map = {
		{"", CMD_HELP},
		{"help", CMD_HELP},
		{"cp", CMD_COPY},
		{"mv", CMD_MOVE},
		{"ls", CMD_LS},
		{"la", CMD_LA},
	};
public:
	Command();
	~Command();

	bool	read(int argc, char* argv[]);
	bool	exec();
};