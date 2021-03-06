#include "chatview.h"
#include <json/json.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <string.h>
#include <sys/socket.h>
#include "public.h"
#include <time.h>
#include "mysqlserver.h"


void getName(string name, char *buff)
{
	strcpy(buff, name.c_str());
	char str[1024] = { 0 };
	int k = 0;
	for (int i = 0, j = 0; buff[i] != '\0'; i++)
	{
		if (buff[i] == ']'&&buff[i + 1] == ' '&&j == 0)
		{
			i++;
			j = 1;
			continue;
		}
		if(j == 1)
		str[k++] = buff[i];
	}
	str[k] = '\0';
	memset(buff, 0, 500);
	strcpy(buff, str);
}

//                  send who   send who fd    who send
bool checkFdState(char *name,int *fd,char *username)
{
	char sql[1024] = { 0 };
	sprintf(sql,"SELECT * FROM %s,state WHERE %s.name=state.username;",username,username);
	sem_wait(&sem);
	mysql_real_query(gMySqlServer.mpcon, sql, strlen(sql));
	gMySqlServer.mpres = mysql_store_result(gMySqlServer.mpcon);
	sem_post(&sem);
	if(gMySqlServer.mpres == 0)
		return false;
	while(gMySqlServer.mrow = mysql_fetch_row(gMySqlServer.mpres))
	{
		if (strcmp(name, gMySqlServer.mrow[0]) == 0)
		{
			*fd = atoi(gMySqlServer.mrow[2]);
			return true;
		}
	}
	return false;
}

bool judgeName(char *name,char *username)
{
	char sql[1024] = { 0 };
	sprintf(sql, "select * from %s where name='%s';",username,name);
	cout << sql <<endl;
	sem_wait(&sem);
	mysql_real_query(gMySqlServer.mpcon, sql, strlen(sql));
	gMySqlServer.mpres = mysql_store_result(gMySqlServer.mpcon);
	sem_post(&sem);
	if(gMySqlServer.mpres == 0)
	{
		return false;
	}
	if(gMySqlServer.mrow = mysql_fetch_row(gMySqlServer.mpres))
	{
		if (strcmp(name, gMySqlServer.mrow[0]) == 0)
			return true;
	}
	return false;
}

void getSendName(int fd,char *name)
{
	char sql[1024] = { 0 };

	sprintf(sql,"select * from state where sockfd=%d",fd);
	char fdd[100] = {0};
	sprintf(fdd,"%d",fd);
	sem_wait(&sem);
	mysql_real_query(gMySqlServer.mpcon, sql, strlen(sql));
	gMySqlServer.mpres = mysql_store_result(gMySqlServer.mpcon);
	sem_post(&sem);
	if(gMySqlServer.mrow = mysql_fetch_row(gMySqlServer.mpres))
		if(strcmp(fdd, gMySqlServer.mrow[0]) == 0)
			strcpy(name,gMySqlServer.mrow[1]);
}

void makeUp(string name, char *buff)
{
	strcpy(buff, name.c_str());
	int i = 0;
	for (; ; i++)
	{
		if (buff[i] == ']')
			break;
	}
	i++; i++; buff[i] = '\0';
}

void ChatView::sendMsg(int _clientfd,Json::Value &root)
{
		string msg = root["message"].asString();
		string _name = root["name"].asString();
		int	type = root["type"].asInt();
		Json::Value response;
		char buff[500] = { 0 };
		memset(buff,0,500);
		getName(_name,buff);
		int sendfd = 0;
		char name[500] = { 0 };
		memset(name,0,500);
		getSendName(_clientfd,name);
		char buff3[500] = { 0 };
		memset(buff3,0,500);
		makeUp(_name,buff3);
		strcat(buff3,name);
		if(checkFdState(buff,&sendfd,name))
		{
			response["type"] = 1;
			response["name"] = buff3;
			response["message"] = msg.c_str();
			send(sendfd, response.toStyledString().c_str(),1024, 0);
		}
		else{
			if(judgeName(buff,name))
			{
				char sql[300] = { 0 };
				sprintf(sql, "insert into offlinemsg values('%s','%s','%s');", buff , buff3 , msg.c_str());
				mysql_real_query(gMySqlServer.mpcon, sql, strlen(sql));
			}
			else
			{
				response["type"] = 1;
				response["name"] = "error";
				response["message"] = "The user is not your friend !";
				send(_clientfd, response.toStyledString().c_str(),1024, 0);
			}
		}
	
}

bool deleteFd(int fd)
{
	char sql[1024] = { 0 };
	sprintf(sql, "delete from state where sockfd = %d;",fd);
	if(mysql_real_query(gMySqlServer.mpcon, sql, strlen(sql)))
	{
		cout << "sockfd:"<<fd<<"exit successfully!"<<endl;
		return true;
	}
	cout << "sockfd:"<<fd<<"exit failure!"<<endl;
	return false;
}

void sendTx(int _clientfd,Json::Value &root)
{
	int state = root["state"].asInt();
	if(state == 0)
	{
		string name = root["name"].asString();
		string message = root["message"].asString();
		int size = root["size"].asInt();

		char sendname[100] = { 0 };
		getSendName(_clientfd,sendname);

		Json::Value response;
		Json::Reader reader;
		char recvname[100] = { 0 };
		strcpy(recvname,name.c_str());
		int recvfd = 0;
		if(judgeName(recvname,sendname))
		{
			if(checkFdState(recvname,&recvfd,sendname))
			{
				response["type"] = 2;
				response["state"] = 1;
				response["name"] = sendname;
				response["size"] = size;
				response["message"] = message;

				cout << response.toStyledString().c_str()<<endl;
				send(recvfd,response.toStyledString().c_str(),1024,0);
				return;
			}

			else
			{
				response["type"] = 2;
				response["state"] = 3;
				response["name"] = "This user is not your friends!";
				send(_clientfd,response.toStyledString().c_str(),1024,0);
				return ; 
			}
		}
		else
		{
			response["type"] = 2;
			response["state"] = 3;
			response["name"] = "This user is not your friends!";
			send(_clientfd,response.toStyledString().c_str(),1024,0);
			return;
		}
	}
	if(state == 1)
	{
		Json::Value response;
		int sendfd;
		int ack = root["ack"].asInt();
		string name_ = root["name"].asString();
		char sendname[100] = { 0 };
		strcpy(sendname,name_.c_str());
		cout << "sendname:"<<sendname <<endl;
		char tt[1024] = { 0 };
		getSendName(_clientfd,tt);
		checkFdState(sendname,&sendfd,tt);
		response["type"] = 2;
		response["state"] = 3;
		if(ack == 0)
		{
			response["name"] = "user refused !";
			cout << "sendfd:"<<sendfd<<endl;
			send(sendfd,response.toStyledString().c_str(),1024,0);
		}
		else
		{
			response["name"] = "User agrees to accept!";
			cout << "sendfd:"<<sendfd<<endl;
			send(sendfd,response.toStyledString().c_str(),1024,0);
		}
		
	}
	if(state == 2)
	{
		Json::Value response;
		Json::Reader reader;
		Json::Value root4;
		int size = root["size"].asInt();
		string	name  = root["name"].asString();
		int sendfd;
		char buff6[200] = { 0 };
		strcpy(buff6,name.c_str());
		char tt[1024] = { 0 };
		getSendName(_clientfd,tt);
		checkFdState(buff6,&sendfd,tt);
		response["type"] = 2;
		response["state"] = 2;
		send(sendfd,response.toStyledString().c_str(),1024,0);
		char buff4[1024] = { 0 };
		char buff5[1024] = { 0 };
		int sum = 0, n = 0;
		string text;
		cout << "size:"<<size<<endl;
		while(sum < size)
		{
			recv(_clientfd,buff4,1024,0);
			cout << buff4 <<endl;
			if(reader.parse(buff4,root4))
				text = root4["text"].asString();
			n = strlen(text.c_str());
			strcpy(buff5,text.c_str());
			sum += n;
			response["text"] = buff5;
			send(sendfd,response.toStyledString().c_str(),1024,0);
		}
	}
}
void onLine(int fd)
{
	Json::Value response;
	char name[100] = { 0 };
	getSendName(fd,name);
	char sql[1024] = { 0 };
	sprintf(sql,"SELECT * FROM %s,state WHERE %s.name=state.username;",name,name);
	sem_wait(&sem);
	mysql_real_query(gMySqlServer.mpcon,sql,strlen(sql));
	gMySqlServer.mpres = mysql_store_result(gMySqlServer.mpcon);
	if(gMySqlServer.mpres == 0)
	{
		response["type"] = 3;
		response["online"] = "No good friends online!";
		send(fd,response.toStyledString().c_str(),1024, 0);
		sem_post(&sem);
		return ;
	}
	sem_post(&sem);
	char arr[1024] = { 0 };
	char arr1[1024] = "--OnLine friends--\n";
	char arr2[1024] = "-----------------\n";
	while(gMySqlServer.mrow = mysql_fetch_row(gMySqlServer.mpres))
	{
	
		strcat(arr,gMySqlServer.mrow[0]);
		strcat(arr,"\n");
	}
	strcat(arr1,arr);
	strcat(arr1,arr2);
	if(strcmp(arr,"") == 0)
	{
		response["type"] = 3;
		response["online"] = "No good friends online!";
		send(fd,response.toStyledString().c_str(),1024, 0);
		return ;
	}
	response["type"] = 3;
	response["online"] = arr1;
	send(fd,response.toStyledString().c_str(),1024, 0);
}
void All(int fd)
{
	char name[100] = { 0 };
	getSendName(fd,name);
	char sql[1024] = { 0 };
	sprintf(sql,"SELECT * FROM %s;",name);
	sem_wait(&sem);
	if(mysql_real_query(gMySqlServer.mpcon,sql,strlen(sql)))
		cout <<"query fail"<<endl;
	gMySqlServer.mpres = mysql_store_result(gMySqlServer.mpcon);
	sem_post(&sem);
	char arr[1024] = { 0 };
	char arr1[1024] = "--All friends--\n";
	char arr2[1024] = "---------------\n";
	while(gMySqlServer.mrow = mysql_fetch_row(gMySqlServer.mpres))
	{	
		strcat(arr,gMySqlServer.mrow[0]);
		strcat(arr,"\n");
	}
	strcat(arr1,arr);
	strcat(arr1,arr2);
	Json::Value response;
	response["type"] = 4;
	response["all"] = arr1;
	send(fd,response.toStyledString().c_str(),1024, 0);
}
char *timefun(char *buff)
{
	time_t timer;
	struct tm *tblock;
	timer = time(NULL);
	tblock = localtime(&timer);
	strcpy(buff,asctime(tblock));
	char *p = &buff[0];
	char str[500] = { 0 };
	str[0] = '[';
	int i = 0,j = 1;
	for(;p[i]!='\0';i++,j++)
		str[j] = p[i];
	str[--j] = ']';
	str[++j] = ' ';
	str[++j] = '\0';
	strcpy(buff,str);
	return buff;
}
void talkgroup(int fd,Json::Value &root)
{
	Json::Value response;
	Json::Reader reader;
	string name = root["group"].asString();
	string message = root["message"].asString();
	string _time = root["time"].asString();
	char groupname[1024] = { 0 };
	char message_[1024] = { 0 };
	char _Time[1024] = { 0 };
	strcpy(groupname,name.c_str());

	strcpy(message_,message.c_str());
	strcpy(_Time,_time.c_str());
	char sendName[1024] = { 0 };
	getSendName(fd,sendName);
	char sql[1024] = { 0 };
	sprintf(sql,"SELECT * FROM %s;",sendName);
	sem_wait(&sem);
	if(mysql_real_query(gMySqlServer.mpcon,sql,strlen(sql)))
		cout <<"query fail"<<endl;
	gMySqlServer.mpres = mysql_store_result(gMySqlServer.mpcon);
	sem_post(&sem);
	char arr[1024] = { 0 };
	cout << "111"<<endl;
	int si = 0;
	while(gMySqlServer.mrow = mysql_fetch_row(gMySqlServer.mpres))
	{   
		char arr1[200] =  { 0 };
		strcpy(arr1,gMySqlServer.mrow[0]);
		if(arr1[strlen(arr1)-1] == '*')
			if(strncmp(gMySqlServer.mrow[0],groupname,strlen(groupname)) == 0)
			{
				si = 1;	
				break;
			}
	}
	if(si == 0)
	{
		response["type"] = 1;
		response["name"] = "error";
		response["message"] = "You haven't joined this group chat yet!";
		send(fd, response.toStyledString().c_str(),1024, 0);
		return ;
	}
	if(si == 1)
	{
		char sql1[1024] = { 0 };
		sprintf(sql1,"SELECT * FROM %s;",groupname);
		sem_wait(&sem);
		if(mysql_real_query(gMySqlServer.mpcon,sql1,strlen(sql1)))
		{
			response["type"] = 1;
			response["name"] = "error";
			response["message"] = "fail in send !";
			send(fd, response.toStyledString().c_str(),1024, 0);
			gMySqlServer.mpres = mysql_store_result(gMySqlServer.mpcon);
			sem_post(&sem);
			return ;
		}
		gMySqlServer.mpres = mysql_store_result(gMySqlServer.mpcon);
		sem_post(&sem);
		int i = 0;
		char username[1024][50];
		while(gMySqlServer.mrow = mysql_fetch_row(gMySqlServer.mpres))
		{
			strcpy(username[i],gMySqlServer.mrow[0]);
			cout << username[i]<<endl;
			i++;
		}
		char timebuff[1024] = { 0 };
		timefun(timebuff);
		char str[1024] = { 0 };
		char sendname[1024] = { 0 };
		strcat(strcat(strcat(strcat(strcat(str,timebuff)," "),groupname)," "),sendName);
		int sendfd = 0;
		int x = 0;
		for(int j = 0; j < i ;j++)
		{
			x = 0;
			cout << "j:"<<j<<endl;
			int fdd = 0;
			char sql3[1024] = { 0 };
			cout << "username[j]:"<<username[j]<<endl;
			sprintf(sql3,"SELECT * FROM state where username='%s'",username[j]);
			cout <<"username:"<<username[j]<<endl;
			sem_wait(&sem);
			mysql_real_query(gMySqlServer.mpcon, sql3, strlen(sql3));
			if(gMySqlServer.mpres = mysql_store_result(gMySqlServer.mpcon))
			{
				cout <<"x = 1"<<endl;
				x = 1;
			}
			if(gMySqlServer.mpres == 0)
			{
				cout <<"x = 1"<<endl;
				x = 1;
			}
			sem_post(&sem);
			gMySqlServer.mrow = mysql_fetch_row(gMySqlServer.mpres);
			char arrr[1024] = { 0 };
			cout <<"1112"<<endl;
			cout <<"gMySqlServer.mrow[1]:"<<gMySqlServer.mrow[1]<<endl;
			strcpy(arrr,gMySqlServer.mrow[1]);
			cout << "arrr:"<<arrr<<endl;
			if(strcmp(arrr,"") == 0)
				x = 1;
			if(x == 1)
			{   
				cout <<"555"<<endl; 
				char sql4[1024] = { 0 };
				sprintf(sql4,"SELECT * FROM user where username='%s'",username[j]);
				sem_wait(&sem);
				mysql_real_query(gMySqlServer.mpcon, sql4, strlen(sql4));
				gMySqlServer.mpres = mysql_store_result(gMySqlServer.mpcon);
				sem_post(&sem);
				gMySqlServer.mrow = mysql_fetch_row(gMySqlServer.mpres);
				if (strcmp(username[j], gMySqlServer.mrow[1]) == 0)
				{
					char sql2[300] = { 0 };
					sprintf(sql2, "insert into offlinemsg values('%s','%s','    %s');", username[j] , str , message.c_str());
					cout << sql2 <<endl;
					mysql_real_query(gMySqlServer.mpcon, sql2, strlen(sql2))    ;
				}
				continue;
			}

			if(x == 0)
			{
				cout <<"122;"<<endl;
				if (strcmp(username[j], gMySqlServer.mrow[1]) == 0)
				{
					fdd = atoi(gMySqlServer.mrow[0]);
				}
				response["type"] = 1;
				response["name"] = str;
				response["message"] = message;
				send(fdd, response.toStyledString().c_str(),1024, 0);
			}
		}
	}
}
void ChatView::process(Json::Value &root)
{
	int select = root["select"].asInt();
	switch(select)
	{
		case 0:
			sendMsg(_clientfd,root);
			break;
		case 1:
			sendTx(_clientfd,root);
			break;
		case 2:
			onLine(_clientfd);
			break;
		case 3:
			All(_clientfd);
			break;
		case 4:
			talkgroup(_clientfd,root);
			break;
		default:
			break;
	}
}
void ChatView::response(){}
