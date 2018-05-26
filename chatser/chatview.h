#ifndef CHATVIEW_H
#define CHATVIEW_H
#include "mysqlserver.h"
#include "view.h"
#include <string>
#include <vector>
#include "tcpserver.h"
#include <semaphore.h>
using namespace std;
extern sem_t sem;

extern CMySqlServer gMySqlServer;
class ChatView : public View
{
public:
	void sendMsg(int _clientfd,Json::Value &root);
	void process(Json::Value &root);
	void response();
private:
	string _name;
	string _msg;
};
#endif
