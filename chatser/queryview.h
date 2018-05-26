#ifndef QUERYVIEW_H
#define QUERYVIEW_H
#include "mysqlserver.h"
#include "view.h"
#include <string>
#include <vector>
using namespace std;
extern CMySqlServer gMySqlServer;
class QueryModel
{
public:
	void checkStateTable();
};

class QueryView : public View
{
public:
	void process(Json::Value &root);
	void response();
private:
	QueryModel _QueryModel;
};
#endif
