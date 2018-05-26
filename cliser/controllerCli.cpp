#include "controllerCli.h"
extern sem_t sem1;
extern queue<pair<string,string> > vecChat;
extern volatile int signch;
extern char recvName[100];
extern queue<string> ve;
extern char filename_[100];
extern volatile int size;
extern volatile int fr;
extern char recvName_[100];
extern void Division(char *name,char *buff);
void organization_(string name, string message, char *arr)
{
	const char *p = name.c_str();
	const char *s = message.c_str();
	int i = 0;
	for(int j = 0; *s != '\0'; i++)
	{
		if(*p != '\0')
		arr[i] = *p++;
		else if(*p == '\0'&&j == 0)
		{
			arr[i] = ':';
			j = 1;
		}
		else arr[i] = *s++;
	}
	 arr[i] = '\0';
}
void *fun(void* arg)
{
	int fd = (int)arg;
	char buff[1024] = { 0 };
	string message;
	string name;
	Json::Value response;
	Json::Reader reader;
	Json::Value root;
	int type = 0;
	int i = 0,j = 0;
	char arr[1024] = { 0 };
	while(1)
	{
		recv(fd,buff,1024,0);
		if(signch == 1)
		{
			if (reader.parse(buff, root))
				{
					type = root["type"].asInt();
					if(type == 1)
					{
						message = root["message"].asString();
						name = root["name"].asString();
						organization_(name, message, arr);
						cout << arr <<endl;
						memset(arr,0,1024);
					}
					if(type == 2)
					{
						int state = root["state"].asInt();

						if(state == 1)
						{
							message = root["message"].asString();
							size = root["size"].asInt();
							name = root["name"].asString();
							cout << buff <<endl;/////////////////////////////////////
							strcpy(recvName,name.c_str());
							char mess[100] = { 0 };
							strcpy(mess,name.c_str());
							strcat(strcat(mess," "),message.c_str());
							memset(filename_,0,100);
								
							char message_[100] = { 0 };
							strcpy(message_,message.c_str());
							Division(message_,filename_);
								
							cout << mess << endl;
							cout << "file size:"<< size << endl;
							cout <<"Whether to receive?"<<endl;
							cout <<"0.NO       1.Yes   "<<endl;
				
						}

						if(state == 3)
						{
							Json::Value response;
							char str1[100] = "user refused !";
							char str2[100] = "User agrees to accept!";
							name = root["name"].asString();
							if(strncmp(name.c_str(),str2,strlen(str2)) == 0)
							{
								response["msgtype"] = MSG_TYPE_TALK;
								response["select"] = 1;
								response["state"] = 2;
								response["name"] = recvName_;
								response["size"] = size;
								send(fd,response.toStyledString().c_str(),1024,0);
									 
								int num = 0;
								char data1[1024] = { 0 };
								Json::Value rot;
								while((num = read(fr,data1,1024)) > 0)
								{
									rot["text"] = data1;
									send(fd,rot.toStyledString().c_str() ,1024, 0);
								}
								close(fr);
							}
							else
								cout << name << endl;
							}
							if(state == 2)
							{
								int fr1 = open(filename_,O_WRONLY|O_CREAT,0600);
								assert(fr1 != -1);
								int n = 0;
								Json::Value root1;
								string text;
								int i = 0;
								char recvbuff[1024] = { 0 };
								int sum = 0;
								while(sum < size)
								{
									recv(fd,recvbuff,1024,0);
									if(reader.parse(recvbuff,root1))
										text = root1["text"].asString();
										n = strlen(text.c_str());
										sum += n;
										write(fr1,text.c_str(),n);
								}
								printf("下载完成!\n");
								close(fr1);
								memset(filename_,0,100);
							}
					}
					if(type == 3)
					{
						cout << "1111"<<endl;
						string messa = root["online"].asString();
						cout << messa << endl;
					}
					if(type == 4)
					{
						string messa = root["all"].asString();
						cout << messa << endl;
					}
									
					if(type == 5)
					{
						string messa = root["message"].asString();
						cout << messa << endl;
					}

					}
			break;
		}
		if(reader.parse(buff, root))
		{
			type = root["type"].asInt();
			if(type == 1)
			{
			message = root["message"].asString();
			name = root["name"].asString();
			sem_wait(&sem1);
			vecChat.push(make_pair(name,message));
			if(j == 0)
			{
				if(signch == 0)
					cout <<"You have new news " << endl;
				j = 1;
			}
			sem_post(&sem1);
			}
			if(type == 2)
			{
				int state = root["state"].asInt();
				if(state == 1)
				{
					message = root["message"].asString();
					size = root["size"].asInt();
					name = root["name"].asString();
					cout << "name:"<<name<<endl;
					strcpy(recvName,name.c_str());
					char mess[100] = { 0 };
					strcpy(mess,name.c_str());
					strcat(strcat(mess," "),message.c_str());
					memset(filename_,0,100);
					
					char message_[100] = { 0 };
					strcpy(message_,message.c_str());
					Division(message_,filename_);
					if(j == 0)
					{
						if(signch == 0)
							cout << "You have a file transfer request."<< endl << "Please check it in time!"<<endl;
						j = 1;
					}

					char buff[1024] = { 0 };
					char buff1[100] = { 0 };
					sprintf(buff,"%s \nfile size:%d\nWhether to receive?\n0.NO       1.Yes\n",mess,size);
				
					string mess_ = buff;
					sem_wait(&sem1);
					ve.push(mess_);
					sem_post(&sem1);
				}

			}
			if(type == 3)
			{
				cout << "1111"<<endl;
				string messa = root["online"].asString();
				cout << messa << endl;
			}
			if(type == 4)
			{
				string messa = root["all"].asString();
				cout << messa << endl;
			}

			if(type == 5)
			{
				string messa = root["message"].asString();
				cout << messa << endl;
			}
		}
		message.clear();name.clear();
		memset(buff,0,1024);
	}
}
void *threadDoing(int fd)
{
	pthread_t id;
	pthread_create(&id, NULL, fun, (void *)fd);
}
void query_(int fd)
{
	Json::Value response;
	int select = 0;
	cout <<"1. online friends      2. all friends  "<<endl;
	while(1)
	{
		cin >> select;
		if(select < 1|| select >2)
		{
			cout << "Wrong input parameter !"<<endl<<"Please re-enter:"<<endl;
			continue;
		}
		else
			break;
	}
	if(select == 1)
	{
		response["msgtype"] = MSG_TYPE_TALK;
		response["select"] = 2;
		send(fd, response.toStyledString().c_str(),1024, 0);
	}
	if(select == 2)
	{
		response["msgtype"] = MSG_TYPE_TALK;
		response["select"] = 3;
		send(fd, response.toStyledString().c_str(),1024, 0);
	}
}
void addfriend(int fd)
{
	cout << "Please enter your friend's username :";
	fflush(stdin);
	char getname[1024] = { 0 };
	cin >> getname ;
	Json::Value response;
	response["msgtype"] = MSG_TYPE_FRIEND;
	response["name"] = getname;
	send(fd, response.toStyledString().c_str(),1024, 0);
	fflush(stdin);
}
void groupDisplay(int fd)
{
	Json::Value response;
	cout << "1. create group    2. add group " << endl;

	cout <<"Please enter:";
	int select = 0 ;
	while(1)
	{
		fflush(stdin);
		cin >> select;
		if(select < 1|| select >2)
		{
			cout << "Wrong input parameter !"<<endl<<"Please re-enter:";
			continue;
		}
		else
			break;
	}
	char groupName[1024] =  { 0 };
	if(select == 1)
	{
		cout << "Please enter the name of the group you want to create :";
		fflush(stdin);
		cin >> groupName;
		response["msgtype"] = MSG_TYPE_GROUP;
		response["select"] = 1;
		response["name"] = groupName;
		send(fd, response.toStyledString().c_str(),1024, 0);
	}
	if(select == 2)
	{
		cout <<"Please enter the name of the group you want to add :";
		fflush(stdin);
		cin >> groupName;
		response["msgtype"] = MSG_TYPE_GROUP;
		response["select"] = 2;
		response["name"] = groupName;
		send(fd, response.toStyledString().c_str(),1024, 0);
	}

}
void controllerCli::run(int sockfd)
{
	sem_init(&sem1,0,1);
	while (1)
	{
		char signmesg[1024] = { 0 };
		int endsign = 0,sle = 0;
		cout << "***********Controller**********" << endl;
		cout << "1.chat         2.query   3.file" <<endl; 
		cout << "4.add_friends  5.group   6.exit" << endl;
		cout << "*******************************" << endl;
		threadDoing(sockfd);
		if(vecChat.size() != 0)
		{
			cout<<" You have new news " << endl;
			sle = 1;
		}
		int select;
		
		fflush(stdout);
		while(1)
		{
			fflush(stdin);
			cin >> select;
			if(select < 1 || select > 6)
			{
				cout << "wrong input parameter!\n"<<endl;
				fflush(stdin);
			}
			else break;
		}
			switch(select)
			{
			case 1:
				if(sle == 1)
					sleep(1);
				signch = 1;
				_ChatView.run(sockfd);
				_ChatView.fun(sockfd);
				signch = 0;
				sle = 0;
				break;
			case 2:
				   query_(sockfd);
				   break;
			case 3:
			case 4:
				   addfriend(sockfd);
				   break;
			case 5:
				   groupDisplay(sockfd);
				   break;
			case 6:
				endsign = 1;
				break;
			default:
				   break;
			}
		if(endsign == 1)
		{
			endsign = 0;
			break;
		}
	}
}
