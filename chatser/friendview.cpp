#include "friendview.h"
extern CMySqlServer gMySqlServer;
extern sem_t sem;
extern void getSendName(int fd,char *name);
void FriendView::process(Json::Value &root)
{
	Json::Value response;
	string name  = root["name"].asString();
	char sql[1024]={0};
 	sprintf(sql, "select * from user;");
	sem_wait(&sem);
    if(mysql_real_query(gMySqlServer.mpcon,sql,strlen(sql)))
	{
		response["type"] = 5;
		response["message"] = "Add failed !";
		send(_clientfd, response.toStyledString().c_str(),1024, 0);
    	gMySqlServer.mpres = mysql_store_result(gMySqlServer.mpcon);
		sem_post(&sem);
		return ;
	}
	gMySqlServer.mpres = mysql_store_result(gMySqlServer.mpcon);
	sem_post(&sem);
    while(gMySqlServer.mrow = mysql_fetch_row(gMySqlServer.mpres))
   	{
        if(strcmp(name.c_str(), gMySqlServer.mrow[0]) == 0)
        {
			char sql1[1024] = { 0 };
			char username[1024] = { 0 };
			getSendName(_clientfd,username);
           	sprintf(sql1, "insert into %s values('%s','1');", username,name.c_str());
    		mysql_real_query(gMySqlServer.mpcon,sql1,strlen(sql1));
        	response["type"] = 5;
			response["message"] = "Add success!";
			send(_clientfd, response.toStyledString().c_str(),1024, 0);	
			return ;	
		}
    }
	response["type"] = 5;
	response["message"] = "This user does not exist !";
	send(_clientfd, response.toStyledString().c_str(),1024, 0);
}
void FriendView::response()
{

}
void GroupView::process(Json::Value &root)
{
	Json::Value response;
	int select = root["select"].asInt();
	string name = root["name"].asString();
	if(select == 1)
	{
		char sql[1024] = { 0 };
           	sprintf(sql, "create table %s( username varchar(20) not NULL);", name.c_str());
    		if(mysql_real_query(gMySqlServer.mpcon,sql,strlen(sql)))
		{
			response["type"] = 5;
			response["message"] = "The group name already exists. Please re-enter it!";
			send(_clientfd, response.toStyledString().c_str(),1024, 0);
			return ;
		}
		response["type"] = 5;
		response["message"] = "Create success!";
		send(_clientfd, response.toStyledString().c_str(),1024, 0);
		char str3[1024] = { 0 };
		sprintf(str3,"mkdir %s",name.c_str());
		system(str3);

	}
	if(select == 2)
	{
		char sql[1024] = { 0 };
		sprintf(sql, "select * from %s;",name.c_str());
		sem_wait(&sem);
    	if(mysql_real_query(gMySqlServer.mpcon,sql,strlen(sql)))
		{
			gMySqlServer.mpres = mysql_store_result(gMySqlServer.mpcon);

			response["type"] = 5;
			response["message"] = "The group does not exist  !";
			send(_clientfd, response.toStyledString().c_str(),1024, 0);
			sem_post(&sem);
			return;
		}
		gMySqlServer.mpres = mysql_store_result(gMySqlServer.mpcon);
		sem_post(&sem);
		char sql1[1024] = { 0 };
		char ss[1024] = { 0 };				
		getSendName(_clientfd,ss);
		cout << "ss:"<<ss<<endl;
		sprintf(sql1, "insert into %s values('%s')",name.c_str(),ss);
 		if(mysql_real_query(gMySqlServer.mpcon,sql1,strlen(sql1)))
 	    {
         	response["type"] = 5;
			response["message"] = "Add failed !";
			send(_clientfd, response.toStyledString().c_str(),1024, 0);
			return;
 	    }
		char str[1024] = { 0 };
		strcat(strcat(str,name.c_str()),"*");
		char sql2[1024] = { 0 };

		sprintf(sql2, "insert into %s values('%s','1')",ss,str);
		mysql_real_query(gMySqlServer.mpcon,sql2,strlen(sql2));
		response["type"] = 5;
		response["message"] = "Add success !";
		send(_clientfd, response.toStyledString().c_str(),1024, 0);
		return;
		
	}	
}
void GroupView::response()
{
					  
}

