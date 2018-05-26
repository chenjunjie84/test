#ifndef PUBLIC_H
#define PUBLIC_H
#include <string.h>
#include <string>
#include <sys/socket.h>
#include"json/json.h"
using namespace std;
enum msgtype
{
	MSG_TYPE_ACK,
	MSG_TYPE_LOGIN,
	MSG_TYPE_REGISTER,
	MSG_TYPE_QUERY,
	MSG_TYPE_TALK,
	MSG_TYPE_BROADCAST,
	MSG_TYPE_EXIT,
	MSG_TYPE_FILE,
	MSG_TYPE_FRIEND,
	MSG_TYPE_GROUP
};
void readJson(char *buff, char *type);
void write_Json(char *buff, int _clientfd, char *type);
void writeJson(char *buff, int _clientfd, char *type);
#endif
