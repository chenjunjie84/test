#include "tcpserver.h"
using namespace std;

int main()
{
	CTcpServer _CTcpServer("127.0.0.1",6000);
	_CTcpServer.run();
	return 0;
}
