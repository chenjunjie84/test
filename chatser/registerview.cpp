#include "registerview.h"
#include <stdlib.h>
#include "mysqlserver.h"
bool RegisterModel::checkUserName(string name)
{
	/*
	string _pwd = memcached_get(name)

	*/
	char sql[1024] = { 0 };
	sprintf(sql, "select * from user where username='%s';", name.c_str());

	sem_wait(&sem);
	mysql_real_query(gMySqlServer.mpcon, sql, strlen(sql));
	//从连接处获取select查询的结果 =》  二维表
	gMySqlServer.mpres = mysql_store_result(gMySqlServer.mpcon);
	sem_post(&sem);
	if(gMySqlServer.mrow = mysql_fetch_row(gMySqlServer.mpres))
	{
		if(strcmp(name.c_str(), gMySqlServer.mrow[0]) == 0)
			return false;
	}
	return true;
}
int RegisterView::checkMail(string _mail)
{
	Json::Value response;
	Json::Reader reader;
	Json::Value root;
	email.setSmtpServer("smtp.163.com");
	email.setAuthName("413246753@163.com");
	email.setAuthPwd("Apptree2009");
	email.setDestEmail(_mail);
	srand(time(NULL));
	//checkCode = (rand() * 12341) % 10000;
	checkCode = 1234;
	char checkCodeBuf[1024] = {0};
	sprintf(checkCodeBuf,"%d",checkCode);
	email.setEmailText(checkCodeBuf);
	if (email.sendEmail())
	{
		memset(checkCodeBuf, 0,sizeof(checkCodeBuf));
		response["statemsg"] = "mail send successfully!";
		send(_clientfd, response.toStyledString().c_str(),
			1024, 0);
		recv(_clientfd, checkCodeBuf, 1024, 0);
		int _checkCode = 0;
		if (reader.parse(checkCodeBuf, root))
			_checkCode = root["checkCode"].asInt();
		if (checkCode == _checkCode)
		{
			memset(checkCodeBuf,0, sizeof(checkCodeBuf));
			int passwod = atoi(_pwd.c_str());
			sprintf(checkCodeBuf, "insert into user values('%s',%d,'%s');", _name.c_str(), passwod, _mail.c_str());
			if (mysql_real_query(gMySqlServer.mpcon, checkCodeBuf, strlen(checkCodeBuf)))
				return 0;
			else
				return 1;
		}
		return 2;
	}
	else
		return 3;
}

void RegisterModel::addUserInfo(string name, string password,string mail)
{
	char sql[1024] = {0};
	sprintf(sql, "insert into user values('%s', '%s','%s')", name.c_str(), password.c_str(), mail.c_str());
	if (mysql_real_query(gMySqlServer.mpcon, sql, strlen(sql)))
	{
		cerr << "insert " << name << " user info success!" << endl;
	}
}

void RegisterView::process(Json::Value &root)
{
	_name = root["name"].asString();
	_pwd = root["pwd"].asString();
	_mail = root["mail"].asString();
	
	if (_RegisterModel.checkUserName(_name))
	{
		int ret = checkMail(_mail);
		if(ret == 0)
			_registerState = 0;
		else if(ret == 1)
			_registerState = 1;
		else if(ret == 2)
			_registerState = 2;
		else
			_registerState = 3;
	}
	else
	{
		_registerState = 4;
	}
}
void RegisterView::response()
{
	Json::Value response;
	char str[1024] = "ok";
	if(_registerState == 0)
	{
		response["statemsg"] = "register failure!";
		cout << "response:" << response << endl;
		send(_clientfd, response.toStyledString().c_str(),
			1024, 0);
	}
	else if(_registerState == 1)
	{
		response["statemsg"] = "register successfully!";
		cout << "response:" << response << endl;
		send(_clientfd, response.toStyledString().c_str(),
			1024, 0);
	}
	else if(_registerState == 2)
	{
		response["statemsg"] = "checkcode error!";
		cout << "response:" << response << endl;
		send(_clientfd, response.toStyledString().c_str(),
			1024, 0);
	}
	else if(_registerState == 3)
	{
		response["statemsg"] = "mail send failure!";
		cout << "response:" << response << endl;
		send(_clientfd, response.toStyledString().c_str(),
			1024, 0);
	}
	else
	{
		response["statemsg"] = "User name already exists !";
		cout << "response:" << response << endl;
		send(_clientfd, response.toStyledString().c_str(),
			1024, 0);
	}
	char buff[1024] = {0};
	sprintf(buff,"create table %s(name varchar(20) NOT NULL,state varchar(2) NULL);",_name.c_str());
	mysql_real_query(gMySqlServer.mpcon, buff, strlen(buff));
}
