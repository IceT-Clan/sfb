// Define macros for version numbers
#ifndef SFB_H
#define SFB_H
#include <string>

using namespace std;

#define SFB_VERSION_MAJOR 0
#define SFB_VERSION_MINOR 1
#define SFB_VERSION_PATCH 0
#define SFB_VERSION "0.1.0"

enum COMMANDS {
	CMD_NONE,
	CMD_HELP,
	CMD_START,
	CMD_COPY,
	CMD_MOVE,
	CMD_LS,
	CMD_LA,
	CMD_CD,
	CMD_PWD
};

// Request packet structure
typedef struct {
	COMMANDS	cmd;
	string		sourcePath;	// Null when file is on sender system
	string		targetPath;
} REQ_PACKET;

// Answer packet structure
typedef struct {
	COMMANDS	cmd;
	fstream*	file;
	bool		success;
} ANS_PACKET;

#endif /* SFB_H */
