#include <sys/types.h>
#include <sys/socket.h>
#include <arpa/inet.h>

using namespace std;

class TCB{
public:
	enum State {closed, listen, synsent, estab};
	State current;
	TCB();
	~TCB();
};