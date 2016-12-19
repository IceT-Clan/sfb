// Define macros for version numbers
#ifndef SFB_H
#define SFB_H
#include <string>
#include <iostream>
#include <mutex>

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
	CMD_PWD,
	CMD_MKDIR,
	CMD_TOUCH,
	CMD_RM,
};
enum PACKETS : uint8_t {
	REQUEST = 0,
	INFO = 1,
	CONF = 2,
	DATA = 3,
	MESSAGE = 4
};
enum CONFIRMATION : uint8_t {
	OK = 0,
	NOT_ENOUGH_SPACE = 1,
	FILE_EXISTS = 2,
	DELETE_FILE,
	NOT_DELETE_FILE,
	ERROR_CREATING
};

// Request packet structure
typedef struct {
	COMMANDS	cmd;
	string		path0;	// Quelle
	string		path1;	// Ort
} REQ_PACKET;

// Data Info Answer
typedef struct {
	unsigned long long bytesnr;
} INFO_PACKET;

// Send? 
typedef struct {
	CONFIRMATION	confirmation;
} CONF_PACKET;

// Answer packet structure
typedef struct {
	uint8_t		bytes[252];
	uint32_t	checksum;
} DATA_PACKET;

typedef struct {
	uint32_t	size;
	string		msg;
} MSG_PACKET;

class Globals {
public:
	static mutex g_coutMutex;
#define COUT(x) {lock_guard<mutex> lock(Globals::g_coutMutex); cout << x;}
#define CERR(x) {lock_guard<mutex> lock(Globals::g_coutMutex); cerr << x;}
};
#endif /* SFB_H */
