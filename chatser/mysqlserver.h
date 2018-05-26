#ifndef MYSQL_H
#define MYSQL_H
#include <mysql/mysql.h>
using namespace std;
class CMySqlServer
{
public:
    CMySqlServer();
    ~CMySqlServer();

    MYSQL *mpcon;
	MYSQL_RES *mpres;
	MYSQL_ROW mrow;
};

#endif
