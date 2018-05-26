#ifndef CLIENTEXIT1_H
#define CLIENTEXIT1_H
#include <iostream>
#include<sstream>
#include <string>
#include "public.h"
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "json/json.h"
#include <pthread.h>
#include <string.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <fcntl.h>
using namespace std;

enum select_ex
{
	FILE_GET,
	FILE_PUT,
	FILE_MV,
	FIEL_CP,
	FILE_LS,
	FILE_EXIT
};
void *RecvMsgg(void *arg);
void thread(int sockfd);
int get_file_size(const char* file);
void filePut(int sockfd);
void fileGet(int sockfd);
void fun(int sockfd);
void exitfun(int sockfd);
class FileView
{
public:
	void run(int sockfd)
	{
		Json::Value Send;
		Send["msgtype"] = MSG_TYPE_FILE;
		send(sockfd, Send.toStyledString().c_str(),
			strlen(Send.toStyledString().c_str()) + 1, 0);
		thread(sockfd);
		while (1)
		{
			cout<<endl;
			cout << "****************FILE**************************" << endl ;
			cout << "0.Download    1.Upload    2.Command     5.Exit"<<endl;
			cout << "**********************************************" << endl ;
			cout << "Please enter:";
			fflush(stdin);fflush(stdout);
			int select = 0;
			cin >> select;
			fflush(stdin);fflush(stdout);
			switch (select)
			{
			case 0:
				   cout <<"111"<<endl;
				fileGet(sockfd);
				break;
			case 1:
				filePut(sockfd);
				break;
			case 2:
				fun(sockfd);
				break;
			case 5:
				exitfun(sockfd);
				break;
			default:
				exitfun(sockfd);
				break;
			}
		}
	}
private:
	string username;
	string pwd;
};
#endif
