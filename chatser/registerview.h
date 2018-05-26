#include "mysqlserver.h"
#include "view.h"
#include <map>
#include <string>
#include "mail.h"
#include <json/json.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <stdlib.h>
#include "public.h"
#include <time.h>
#include <stdlib.h>
#include <fcntl.h>
#include <sys/socket.h>
#include "tcpserver.h"
#include<semaphore.h>
using namespace std;
extern CMySqlServer gMySqlServer;
extern sem_t sem;

class RegisterModel
{
public:
	bool checkUserName(string name);
	void addUserInfo(string name, string password, string mail);
};

class RegisterView : public View
{
public:
	void process(Json::Value &root);
	int checkMail(string _mail);
	void response();
private:
	string _name;
	string _pwd;
	string _mail;
	RegisterModel _RegisterModel;
    CSendEmail email;
	int checkCode;
	int _registerState;
};


