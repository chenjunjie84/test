#ifndef CLIENT_H
#define CLIENT_H
#include "public.h"
#include "clientlogin.h"
#include "clientregister.h"
#include <iostream>
#include <sys/socket.h>
#include<stdio.h>
#include<stdlib.h>
#include<unistd.h>
#include<string.h>
#include<assert.h>
#include<arpa/inet.h>
#include<netinet/in.h>
#include "clientexit.h"

using namespace std;
class Client
{
public:
	Client();
	void run();
	int loginMode();
	void RegisterMode();
	int ExitMode();
private:
	int sockfd;
};
#endif
