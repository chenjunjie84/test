#ifndef LOGINVIEW_H
#define LOGINVIEW_H
#include "mysqlserver.h"
#include "view.h"
#include <string>
#include <vector>
#include <semaphore.h>
#include "tcpserver.h"
#include <map>
using namespace std;
extern CMySqlServer gMySqlServer;
extern sem_t sem;
class LoginModel 
{
public:
	bool checkUserState(string name);
    bool checkUserAuth(string name, string pwd);
    void addUserSocketInfo(string name, int fd);
    multimap<string,string> getUserOfflinemsg(string name);
};

class LoginView : public View
{
public:
    void process(Json::Value &root);
    void response();
private:
    string _name;
    string _pwd;
    LoginModel _loginModel;
    bool _bloginState;
    static int _testCount;
};

#endif
