#include "public.h"
void readJson(char *buff,char *type)
{
	string message;
	Json::Reader reader;
	Json::Value root;
	if (reader.parse(buff, root))		message = root[type].asString();
}
void write_Json(char *buff,int _clientfd,char *type)
{
	Json::Value response;
	buff[strlen(buff)] = 0;
	string message = buff;
	response[type] = message;
	send(_clientfd, response.toStyledString().c_str(),
		strlen(response.toStyledString().c_str()) + 1, 0);
}
void writeJson(char *buff, int _clientfd, char *type)
{
	Json::Value response;
	string message = buff;
	response[type] = message;
	send(_clientfd, response.toStyledString().c_str(),
		strlen(response.toStyledString().c_str()) + 1, 0);
}
