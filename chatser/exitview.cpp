#include "exitview.h"
#include <json/json.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include "public.h"
#include "mysqlserver.h"

bool ExitView::deleteStateFd(int fd)
{
	char sql[1024] = { 0 };
	sprintf(sql, "delete from state where sockfd = %d;",fd);
	if (mysql_real_query(gMySqlServer.mpcon, sql, strlen(sql)))
	{
		return true;
	}
	return false;
}


void ExitView::process(Json::Value &root)
{
	char sql[1024] = { 0 };
	sprintf(sql, "delete from state where sockfd = %d;",_clientfd);
	mysql_real_query(gMySqlServer.mpcon, sql, strlen(sql));
}
void ExitView::response() 
{
	close(_clientfd);
}
