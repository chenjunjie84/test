#include "clientfile.h"
#include <string>
#include <unistd.h>
//pthread_t id;
void *RecvMsgg(void *arg)
{
	char buff[1024] = { 0 };
	int _clientfd = (int)arg;
	string message;
	string messagei;
	Json::Reader reader;
	Json::Value root;
	while (1)
	{
		recv(_clientfd, buff, 1024, 0);
		if (reader.parse(buff, root))
			message = root["statemsg"].asString();
		cout << message;
		memset(buff, 0, 1024);
	}
	return NULL;
}
void thread(int sockfd)
{
	pthread_t id;
	pthread_create(&id, NULL, RecvMsgg, (void *)sockfd);
}
int get_file_size(const char* file) {
	struct stat tbuf;
	stat(file, &tbuf);
	return tbuf.st_size;
}
void filePut(int sockfd)
{
	Json::Value Send;
	Json::Value response;
	Send["select"] = FILE_PUT;
	send(sockfd, Send.toStyledString().c_str(),
		strlen(Send.toStyledString().c_str()) + 1, 0);
	string filename;
	cout << "Please enter the name of the file you need to download :";
	cin >> filename;
	int size = get_file_size(filename.c_str());
	cout << size;
	Send["filesize"] = size;
	Send["filename"] = filename;
	send(sockfd, Send.toStyledString().c_str(),
		strlen(Send.toStyledString().c_str()) + 1, 0);
	int file = open(filename.c_str(), O_RDONLY, 0600);
	string message;
	char buff[1024] = { 0 };
	while ((read(file, buff, 1024)) > 0)
	{

		message = buff;
		response["message"] = message;
		send(sockfd, response.toStyledString().c_str(),
			strlen(response.toStyledString().c_str()) + 1, 0);
		memset(buff, 0, sizeof(buff));
	}
	close(file);
}
void fileGet(int sockfd)
{

//	int a = 0;
//	if(a == 0)
//	{
//		a =1;
//		memset(buff_,0,sizeof(buff_));
//		recv(sockfd,buff_,sizeof(buff_),0);
//	}
	//if (reader.parse(buff_, root))
	//	 mss = root["statemsg"].asString();
	//cout<<mss<<endl;
	Json::Value Send;
	Send["select"] = FILE_GET;
	send(sockfd, Send.toStyledString().c_str(),
		strlen(Send.toStyledString().c_str()) + 1, 0);
	string filename;
	string mss;
    Json::Reader reader;
	Json::Value root;
	char buff_[1024] = {0};
	recv(sockfd,buff_,sizeof(buff_),0);
	cout<<buff_<<endl;

	sleep(2);
	cout<<endl;
	cout << "Please enter the name of the file you need to upload :";
	cin >> filename;
	Send["filename"] = filename;
	send(sockfd, Send.toStyledString().c_str(),
		strlen(Send.toStyledString().c_str()) + 1, 0);
	Json::Value response;
	char buff[1024];
	//int size = 0;
	//recv(sockfd, buff, sizeof(buff), 0);
	//if(reader.parse(buff, root))
	//	size = root["filesize"].asInt();
	string message;
	cout << filename <<endl;
	char gg[100] = {0};
	recv(sockfd,gg,sizeof(gg),0);
	int size=atoi(gg);
	cout << "size:"<<size<<endl;
	int file = open(filename.c_str(), O_WRONLY | O_CREAT, 0600);
	char str[1024] = { 0 };
	int sizefd = 0;
	while (1)
	{
		int n = recv(sockfd, str, sizeof(str), 0);
		cout <<n<<endl;
		write(file, str, strlen(str));
		sizefd = sizefd + n;
		cout << "sizefd:"<<sizefd <<endl<<"size:"<<size<<endl;
		if (sizefd == size)
			break;
	}
	close(file);
}
void fun(int sockfd)
{
	pthread_t id;
	pthread_create(&id, NULL, RecvMsgg, (void *)sockfd);
	Json::Value Send;
	Send["select"] = FILE_LS;
	send(sockfd, Send.toStyledString().c_str(),
		strlen(Send.toStyledString().c_str()) + 1, 0);
	string comm;
	char buff[1024] = {0};
	while(1)
	{
	usleep(1000);
	fflush(stdin);fflush(stdout);
	fgets(buff,1024,stdin);
	//cin>>comm;
	comm = buff;
	Send["comm"] = comm;
	send(sockfd, Send.toStyledString().c_str(),
		strlen(Send.toStyledString().c_str()) + 1, 0);
	if(strncmp(comm.c_str(),"exit",4) == 0)
		break;
	memset(buff,0,1024);
	}
	
}
void exitfun(int sockfd)
{
	Json::Value Send;
	Send["select"] = FILE_EXIT;
	send(sockfd, Send.toStyledString().c_str(),
		strlen(Send.toStyledString().c_str()) + 1, 0);
}
