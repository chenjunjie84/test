#include "queryview.h"
#include <json/json.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "public.h"
#include "mysqlserver.h"

void QueryModel::checkStateTable()
{
	char sql[1024] = "";
	sprintf(sql, "select * from state;");
	if (mysql_real_query(gMySqlServer.mpcon, sql, strlen(sql)))
	{
		return ;
	}
	gMySqlServer.mpres = mysql_store_result(gMySqlServer.mpcon);
	while (gMySqlServer.mrow = mysql_fetch_row(gMySqlServer.mpres))
	{
		printf("%s\t", gMySqlServer.mrow[1]);
	}
}

void QueryView::process(Json::Value &root)
{
	_QueryModel.checkStateTable();
}
void QueryView::response() {};
