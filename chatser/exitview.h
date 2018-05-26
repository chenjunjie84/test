#ifndef EXITVIEW_H
#define EXITVIEW_H
#include "mysqlserver.h"
#include "view.h"
#include <string>
#include <vector>
using namespace std;
extern CMySqlServer gMySqlServer;


class ExitView : public View
{
public:
	void process(Json::Value &root);
	void response();
	bool deleteStateFd(int fd);
};
#endif
