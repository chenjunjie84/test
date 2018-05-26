#include "controller.h"
#include "loginview.h"
#include "registerview.h"
#include "chatview.h"
#include "exitview.h"
#include "queryview.h"
#include "json/json.h"
#include "public.h"
#include "fileview.h"
#include "friendview.h"
#include <string.h>
#include <sys/socket.h>

//Controller gController;

Controller::Controller()
{
    //添加系统支持的所有视图
    _viewMap.insert(make_pair("login", new LoginView()));
    _viewMap.insert(make_pair("register", new RegisterView()));
    _viewMap.insert(make_pair("chat", new ChatView()));
    _viewMap.insert(make_pair("exit", new ExitView()));
    _viewMap.insert(make_pair("query", new QueryView()));
    _viewMap.insert(make_pair("filex", new FileView()));
	_viewMap.insert(make_pair("friend", new FriendView()));
	_viewMap.insert(make_pair("group", new GroupView()));
	cout<<"controller init success! it has init LoginView RegisterView ChatView ExitView QueryView!"<<endl;
}
void Controller::process(string json, int fd)
{
	Json::Reader reader;
	Json::Value root;
	if(strcmp(json.c_str(), "client_close_exception") == 0)
	{
		static_cast<ExitView*>(_viewMap["exit"])->deleteStateFd(fd);
		return;
	}
	     
    if(0 == reader.parse(json.c_str(), root))
    {
        cerr<<"json format invalid:"<<json<<endl;
        return;
    }

    cout<<root.toStyledString()<<endl;
    
    /*
    代码设计修改成由Controller根据不同的消息请求分发到不同的View，
    由各自功能的View通过Model处理完数据以后，生成新的json消息自行
    返回给客户端!  和HTTP WEB设计的MVC完全一致!
    */
    int msgtype = root["msgtype"].asInt();
    string viewname;
    switch(msgtype)
    {
        case MSG_TYPE_QUERY:
        viewname = "query";
        break;
        case MSG_TYPE_LOGIN:
        viewname = "login";
        break;
        case MSG_TYPE_REGISTER:
        viewname = "register";
        break;
        case MSG_TYPE_TALK:
        case MSG_TYPE_BROADCAST:
        viewname = "chat";
        break;
		case MSG_TYPE_FILE:
		viewname = "filex";
		break;
        case MSG_TYPE_EXIT:
        viewname = "exit";
		break;
		case MSG_TYPE_FRIEND:
		viewname = "friend";
		break;
		case MSG_TYPE_GROUP:
		viewname = "group";
        break;
    }
    map<string, View*>::iterator it = _viewMap.find(viewname.c_str());
    if(it == _viewMap.end())
    {
        cerr<<"find view failure! json:"<<json<<endl;
        return;
    }
    it->second->setClientfd(fd);
    it->second->process(root);
    it->second->response();
}
