#include "sfb.h"
#include "command.h"

mutex Globals::g_coutMutex;

int main(int argc, char** argv) {

		Command cmd(argc, argv);
		// Print version
		COUT("sfb version " << SFB_VERSION << endl);

		// Read command
		if (!cmd.read()) {
			COUT("Command not found." << endl);
		}

		if (cmd.exec()) {
			COUT("Executed successfully." << endl);
		} else {
			COUT("Could not execute." << endl);
		}

	return 0;
}
