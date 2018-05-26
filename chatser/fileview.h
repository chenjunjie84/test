#ifndef FILEVIEW_H
#define FILEVIEW_H
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include "json/json.h"
#include "view.h"
#include "mysqlserver.h"
#include <sys/select.h>
#include <string>
#include <sys/time.h>
#include <assert.h>
#include <string.h>
#include <pthread.h>
#include <fcntl.h>
#include <sys/stat.h>
#include<sys/types.h>
#include<pwd.h>
#include<assert.h>
#include<sys/wait.h>
#include<sys/stat.h>
using namespace std;
extern CMySqlServer gMySqlServer;
extern int filefddd[2];
extern string userName;
int get_file_size(const char* file);
char **exchange(char *p);
enum _select
{
	FILE_GET,
	FILE_PUT,
	FILE_MV,
	FIEL_CP,
	FILE_LS,
	FILE_EXIT
};

void fun_ex(int _clientfd, string name);
class FileMode
{
public:
	void fileGet(int _clientfd)
	{
		Json::Value response;
		char str[1024] = { 0 };
		char buff[1024] = { 0 };
		string name;
		string message;
		Json::Reader reader;
		Json::Value root;
		pid_t pid = fork();
		if(pid == 0)
		{
			close(1);
			close(2);
			dup2(filefddd[1],2);
			dup2(filefddd[1],1);
			execlp("ls","ls", (char*)0);
		}
		if(read(filefddd[0], str, sizeof(str)))
			cout<<"read succeed!"<<endl;
		cout <<str<<endl;
		send(_clientfd,str,sizeof(str),0);
		//message = str;
		//response["statemsg"] = message;
		//send(_clientfd, response.toStyledString().c_str(),
		//	strlen(response.toStyledString().c_str()) + 1, 0);
	
		recv(_clientfd, buff, sizeof(buff), 0);
		if (reader.parse(buff, root))
			name = root["filename"].asString();
		int file = open(name.c_str(), O_RDONLY, 0600);
		cout<<name<<endl;
		if (file == -1)
		{
			memset(buff, 0, sizeof(buff));
			sprintf(buff, "%s not lived!", name.c_str());
			message = buff;
			response["statemsg"] = message;
			send(_clientfd, response.toStyledString().c_str(),
				strlen(response.toStyledString().c_str()) + 1, 0);
		}
		int size = get_file_size(name.c_str());
		cout << size << endl;
		//response["filesize"] = size;
		//send(_clientfd, response.toStyledString().c_str(),
		//	strlen(response.toStyledString().c_str()) + 1, 0);
		int n = 0;
		int nsize = 0;
		char gg[100]  = {0};
		sprintf(gg,"%d",size);
		cout << "gg:"<<gg<<endl;
		send(_clientfd,gg,sizeof(gg),0);
		while ((n = read(file, buff, 1023)) > 0)
		{
			send(_clientfd, buff,n, 0);
			nsize = nsize + n;
			cout << "n:"<<n <<endl<<"nsize:"<<nsize<<endl<<"size:"<<size<<endl;
			if(nsize == size)
				break;
		}
		strcpy(buff, "Transfer complete!");
		message = buff;
		response["statemsg"] = message;
		send(_clientfd, response.toStyledString().c_str(),
			strlen(response.toStyledString().c_str()) + 1, 0);
		close(file);
	}
	void filePut(int _clientfd)
	{
		Json::Value response;
		Json::Reader reader;
		Json::Value root;
		char buff[1024];
		string message;
		string name;
		int size = 0;
		recv(_clientfd, buff, sizeof(buff), 0);
		if (reader.parse(buff, root))
		{
			name = root["filename"].asString();
			size = root["filesize"].asInt();
		}
		int file = open(name.c_str(), O_WRONLY | O_CREAT, 0600);
		if (file == -1)
		{
			memset(buff, 0, sizeof(buff));
			sprintf(buff, "Transfer failure!", name.c_str());
			message = buff;
			response["statemsg"] = message;
			send(_clientfd, response.toStyledString().c_str(),
				strlen(response.toStyledString().c_str()) + 1, 0);
		}
		int n = 0;
		char str[1024] = { 0 };
		int sizefd = 0;
		while (1)
		{
			recv(_clientfd, str, sizeof(1024), 0);
			if (reader.parse(str, root))
				message = root["message"].asString();
			memset(str, 0, sizeof(str));
			strcpy(buff, message.c_str());
			write(file, buff, strlen(buff));
			sizefd = strlen(buff) + size;
			if (sizefd == size)
				break;
		}
		strcpy(buff, "Transfer complete!");
		message = buff;
		response["statemsg"] = message;
		send(_clientfd, response.toStyledString().c_str(),
			strlen(response.toStyledString().c_str()) + 1, 0);
		close(file);
	}
	void fileFun(int _clientfd, string name)
	{
		fun_ex(_clientfd, name);
	}
};
int judge_ex(char *buff, int len, char *crr);
void get_pwd_ex(char *str, int len, char *crr);
void alter_buff_ex(char *str, int len);
//char **Separate(char *buff, int len);
class FileView : public View
{
	void process(Json::Value &root)
	{

		pipe(filefddd);
		int _select_ = 0;
		Json::Reader reader;
		Json::Value root2;
		char buff[1024] = { 0 };
		recv(_clientfd, buff, sizeof(buff), 0);
		cout <<buff<<endl;
		if(reader.parse(buff, root2))
		_select_ = root2["select"].asInt();
		int a = 0;
		while (1)
		{
			if (a == 1)
				break;
			switch (_select_)
			{
			case 0:
				   cout<<"111"<<endl;
				filefun.fileGet(_clientfd);
				break;
			case 1:
				filefun.filePut(_clientfd);
				break;
			case 2:
				 filefun.fileFun(_clientfd, userName);
				 break;
			case 3:
				 filefun.fileFun(_clientfd, userName);
				 break;
			case 4:
				filefun.fileFun(_clientfd, userName);
				break;
			case 5:
				cout<<"222222"<<endl;
				a = 1;
				break;
			default:
				break;
			}
		}
	}
	void response(){}
private:
	FileMode filefun;

};
#endif
