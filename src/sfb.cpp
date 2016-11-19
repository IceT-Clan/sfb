#include <iostream>
#include <string>
#include "sfb.h"
#include "command.h"

using namespace std;

int main(int argc, char** argv) {
	Command cmd(argc, argv);

	// Print version
	cout << "sfb version " << SFB_VERSION << endl;

	// Read command
	if (cmd.read()) {
		cout << "Command valid" << endl;
	} else {
		cout << "Command not found." << endl;
	}

	cmd.exec();

	return 0;
}
