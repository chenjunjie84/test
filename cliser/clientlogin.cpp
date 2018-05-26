#include "clientlogin.h"

LoginView::LoginView()
{
	cout << "***************LOGIN*************" << endl;
	cout << "username:";
	cin >> username;
	cout << "pwd:";
	cin >> pwd;
	cout << "********************************" << endl << endl;
}


int LoginView::run(int sockfd)
{
	char stateMsg[200];
	Json::Value Send;
	Json::Value root;
	Send["msgtype"] = MSG_TYPE_LOGIN;
	Send["name"] = username;
	Send["pwd"] = pwd;
	cout <<Send.toStyledString().c_str();
	send(sockfd, Send.toStyledString().c_str(),
		strlen(Send.toStyledString().c_str()) + 1, 0);
	Json::Reader reader;
	recv(sockfd, stateMsg, 200, 0);
	string _stateMsg(stateMsg);
	if (reader.parse(_stateMsg, root))
	{
		_stateMsg = root["reason"].asString();
	}
	cout << endl << _stateMsg << endl << endl;
	if (strcmp(_stateMsg.c_str(), "ok") == 0)
	{
		fi.run(sockfd);
		cout << endl << "Exit successfully!" << endl <<endl;
		return 1;
	}
	else return 0 ;
}






