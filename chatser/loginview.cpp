#include "loginview.h"
#include <json/json.h>
#include <stdio.h>
#include <map>
#include <string>
#include <string.h>
#include "mysqlserver.h"
#include <sys/socket.h>
#include "public.h"
string userName;
volatile int loginSign = 0;
////////////////////////////////////////////////////////
bool LoginModel::checkUserState(string name)
{
	char sql[1024] = { 0 };
	sprintf(sql,"select * from state where username='%s'",name.c_str());

	sem_wait(&sem);
	if(mysql_real_query(gMySqlServer.mpcon,sql,strlen(sql)))
	{
		return false;
	}

	
	gMySqlServer.mpres = mysql_store_result(gMySqlServer.mpcon);
	sem_post(&sem);
	if(gMySqlServer.mrow = mysql_fetch_row(gMySqlServer.mpres))
	{
		if(strcmp(name.c_str(), gMySqlServer.mrow[1]) == 0)
			return false;
	}
	return true;

	
}
bool LoginModel::checkUserAuth(string name, string pwd)
{
    /*
    string _pwd = memcached_get(name)
    
    */
    char sql[1024]={0};
    sprintf(sql, "select * from user;");
    
    /*
    先从memcached里面查询，ok返回结果
    fail，从数据库查询
    */
    
    //发送sql查询语句，查询表的内容
	sem_wait(&sem);
    if(mysql_real_query(gMySqlServer.mpcon,sql,strlen(sql)))
    {
        return false;
    }
    
    //从连接处获取select查询的结果 =》  二维表
    gMySqlServer.mpres = mysql_store_result(gMySqlServer.mpcon);
	sem_post(&sem);
    //循环从二维表中取出一个个记录,分别访问它的每一个字段
    while(gMySqlServer.mrow = mysql_fetch_row(gMySqlServer.mpres))
    {
        if(strcmp(name.c_str(), gMySqlServer.mrow[0]) == 0)
        {
            //把该用户的信息缓存到memcached当中
            //memcached_set(name, pwd);
            if(strcmp(pwd.c_str(), gMySqlServer.mrow[1]) == 0)
            {
				return true;
            }
            
        }
    }
    return false;
}

void LoginModel::addUserSocketInfo(string name, int fd)
{
    char sql[1024]={0};
    sprintf(sql, "delete from state where name='%s'", name.c_str());
    if(mysql_real_query(gMySqlServer.mpcon,sql,strlen(sql)))
    {
        cerr<<"delete "<<name<<" socket info failure!"<<endl;
    }
    sprintf(sql, "insert into state values(%d,'%s')", fd , name.c_str());
    if(mysql_real_query(gMySqlServer.mpcon,sql,strlen(sql)))
    {
        cerr<<"insert "<<name<<" socket info failure!"<<endl;
    }
}
multimap<string,string> LoginModel::getUserOfflinemsg(string name)
{
    multimap<string,string> vec;
    char sql[1024]={0};
    sprintf(sql, "select * from offlinemsg where username='%s';", name.c_str());

    if(mysql_real_query(gMySqlServer.mpcon,sql,strlen(sql)))
    {
        cerr<<"query user offlinemsg failure!"<<endl;
        return multimap<string,string>();
    }

    //从连接处获取select查询的结果 =》  二维表
    gMySqlServer.mpres = mysql_store_result(gMySqlServer.mpcon);
    //循环从二维表中取出一个个记录,分别访问它的每一个字段
    while(gMySqlServer.mrow = mysql_fetch_row(gMySqlServer.mpres))
    {
        vec.insert(make_pair(gMySqlServer.mrow[1],gMySqlServer.mrow[2]));
    }
    sprintf(sql, "delete from offlinemsg where username='%s';", name.c_str());
    if(mysql_real_query(gMySqlServer.mpcon,sql,strlen(sql)))
    {
        cerr<<"delete user offlinemsg failure!"<<endl;
    }

    return vec;
}
/////////////////////////////////////////////////////////
int LoginView::_testCount = 0;

void LoginView::process(Json::Value &root)
{
    _name = root["name"].asString();
    _pwd = root["pwd"].asString();
	userName = _name;
    
    //访问对应的LoginModel模块进行用户名，密码验证
    if(_loginModel.checkUserAuth(_name, _pwd))
    {
		if(_loginModel.checkUserState(_name))
		{
			_bloginState = true;
			_loginModel.addUserSocketInfo(_name, _clientfd);
		}
		else
		{
			_bloginState = false;
			loginSign = 1;
		}
    }
    else
    {
        _bloginState = false;
    }
}
void LoginView::response()
{
    Json::Value response;
    response["msgtype"] = MSG_TYPE_ACK;
    if(_bloginState)
    {
		response["reason"] = "ok";
		cout<<"login ok!"<<endl;
	}
    else
    {
		if(loginSign == 1)
			response["reason"] = "User has logged in!";
		else
			response["reason"] = "name or pwd is invalid!";
    }
	
    cout<<"response:"<<response<<endl;
    send(_clientfd, response.toStyledString().c_str(), 
        strlen(response.toStyledString().c_str())+1, 0);

    //如果登录成功，发送该用户的离线消息
	char endbuf[10] = "end";
    if(_bloginState)
    {
        multimap<string, string> vec = _loginModel.getUserOfflinemsg(_name);
        for(multimap<string,string>::iterator it = vec.begin();it != vec.end();++it)
		{
			response["type"] = 1;
			response["name"] = it->first;
			response["message"] = it->second;
			cout << it->first<<" "<<it->second<< endl;
            send(_clientfd, response.toStyledString().c_str(), 
                1024, 0);
		}
    }
}
