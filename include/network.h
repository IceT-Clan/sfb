#include <iostream>
#include <string>
#include <map>
#include "sfb.h"
#include "serial/serial.h"

using namespace std;
using namespace serial;

class Network {
private:
	Serial ser;
public:
	Network();
	~Network();
};
