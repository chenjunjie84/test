#include"clientregister.h"
#include<stdio.h>


void doThing(int fd)
{	
	Json::Value Send;
	Json::Value root;
	Json:: Reader reader;
	char buf[1024] = {0};
	string buff;
	recv(fd,buf,1024,0);
	if(reader.parse(buf,root))
		buff = root["statemsg"].asString();
	cout << endl << buff << endl << endl;
	if(strcmp(buff.c_str(),"User name already exists !") == 0)
		return ;

	int Code = 0;
	fflush(stdin);
	printf("Please enter a verification code:");
	fflush(stdout);
	cin >> Code;
	Send["checkCode"] = Code;
	send(fd, Send.toStyledString().c_str(),
		1024, 0);
	memset(buf,0,1024);
	recv(fd,buf,1024,0);
	if(reader.parse(buf,root))
		buff = root["statemsg"].asString();
	cout << endl << buff << endl<<endl;
}

