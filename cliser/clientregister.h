#ifndef REGISTERVIEW_H
#define REGISTERVIEW_H
#include <iostream>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include "json/json.h"
#include <stdio.h>
#include <string.h>
#include <pthread.h>
#include "public.h"
#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <time.h>
using namespace std;


void doThing(int fd);


class RView
{
public:
	RView()
	{
		cout << "*************Register**********" << endl;
		cout << "username:";
		cin >> username; 
		cout << "pwd:";
		cin >> pwd; 
		cout << "mail:";
		cin >> mail;
		cout << "********************************" << endl;
	}
	void run(int sockfd)
	{
	char buff[200];
	string stateMsg;
	Json::Value Send;
	Json::Value root;
	Send["msgtype"] = MSG_TYPE_REGISTER;
	Send["name"] = username;
	Send["pwd"] = pwd;
	Send["mail"] = mail;
	send(sockfd, Send.toStyledString().c_str(),
		strlen(Send.toStyledString().c_str()) + 1, 0);
	doThing(sockfd);
	}
private:
	string username;
	string pwd;
	string mail;
};

#endif
