#ifndef CLIENTLOGIN_H
#define CLIENTLOGIN_H
#include <iostream>
#include <string>
#include "json/json.h"
#include "public.h"
#include<stdio.h>
#include<stdlib.h>
#include "controllerCli.h"
#include<unistd.h>
#include<string.h>
#include <string>
#include<assert.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include<sys/socket.h>

using namespace std;
class LoginView
{
public:
	LoginView();
	int run(int sockfd);
private:
	string username;
	string pwd;
	controllerCli fi;
};
#endif
