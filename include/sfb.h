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
enum PACKETS {
	REQUEST,
	INFO,
	CONF,
	DATA
};

// Request packet structure
typedef struct {
	COMMANDS	cmd;
	string		path0;	// Quelle
	string		path1;	// Ort
} REQ_PACKET;

// Data Info Answer
typedef struct {
	size_t		bytesnr;
} INFO_PACKET;

// Send? 
typedef struct {
	bool		confirmation;
} CONF_PACKET;

// Answer packet structure
typedef struct {
	uint8_t		bytes[252];
	uint32_t	checksum;
} DATA_PACKET;


#endif /* SFB_H */
