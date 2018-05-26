#include "fileview.h"
#include <stdio.h>
#include <stdlib.h>
#include <fcntl.h>
int filefddd[2];

char **Separate(char*buff, int len)
{
	char **p = (char **)calloc(sizeof(char *) * 10, sizeof(char *));
	assert(p != NULL);
	int i = 1;
	while (i < len)
	{
		p[0] = &buff[0];
		int j = 1, z = 1;
		for (; i < len; i++)
		{
			if (buff[i] == ' ' && z == 1)
			{
				buff[i] = 0;
				z = 0;
			}
			else if (buff[i] != ' ' && z == 1)	continue;

			else if (buff[i] != ' ' && z == 0)
			{
				p[j++] = &buff[i];
				z = 1;
			}
			else	continue;
		}
	}
	return p;
}

/*char **exchange_ex(char *p)
{
	char **s = (char **)calloc(sizeof(char *) * 10, sizeof(char *));
	int len = strlen(p);
	for (int i = 0;;i++)
	{
		if(i == 0)
		s[i] = strtok(p, " ");
		else s[i] = strtok(NULL, " ");
		if (s[i] == NULL)
			break;
	}
	return s;
}*/
void alter_buff_ex(char *str, int len)
{
	char *s = (char *)calloc(sizeof(char)*len, sizeof(char));
	assert(s != NULL);
	int i = 0, z = 1, j = 0;
	for (; i < len; i++)
	{
		if (str[i] == ' '&&z == 1)
		{
			s[j++] = str[i];
			z = z*(-1);
			continue;
		}
		else if (str[i] != ' '&&z != 1)
		{
			s[j++] = str[i];
			z = 1;
			continue;
		}
		else if (str[i] == ' '&&z == -1)	continue;

		else	s[j++] = str[i];

	}
	memset(str, 0, len);
	memmove(str, s, len);
	free(s);
}

void get_pwd_ex(char *str, int len, char *crr)
{
	memset(crr, 0, 128);
	memset(str, 0, 127);
	getcwd(str, 127);
	len = strlen(str);
	if (len == 1 && str[0] == '/')
	{
		memmove(crr, str, len);
		return;
	}
	int i = len - 1, j = 0, z = 0;
	char *s;
	while (str[i] != '/' && i >= 0)
	{
		i--;
		z++;
	}
	s = &str[++i];
	for (; j < z; j++, s++)
		crr[j] = *s;

}

int judge_ex(char *buff, int len, char *crr)
{
	char *s = (char *)calloc(sizeof(char)*len, sizeof(char));
	assert(s != NULL);
	if (strncmp(buff, "cd", 2) == 0)
	{
		int y = len;
		memmove(s, buff, y);
		buff[strlen(buff) - 1] = 0;
		len = strlen(buff);
		alter_buff_ex(buff, len);
		if (chdir(buff + 3) != 0)
		{
			memmove(buff, s, y);
			free(s);
			return 0;
		}
		else
		{
			len = strlen(buff);
			get_pwd_ex(buff, len, crr);
			return 1;
		}
	}

	return 0;
}
int get_file_size(const char* file) {
	struct stat tbuf;
	stat(file, &tbuf);
	return tbuf.st_size;
}
void fun_ex(int _clientfd,string name)
{
	Json::Value response;
	Json::Reader reader;
	Json::Value root;
	char str[1024] = {0};
	struct passwd *pwd;
	pwd = getpwuid(getuid());
	char buff[1024] = { 0 };
	char crr[1024] = { 0 };
	getcwd(crr, 128);
	string name_;
	int len1 = strlen(crr);
	get_pwd_ex(crr, len1, buff);
	memmove(crr, buff, len1);
	sprintf(str, "\n[%s] Please enter [%s]:", name.c_str(), crr);
	string message = str;
	response["statemsg"] = message;
	send(_clientfd, response.toStyledString().c_str(),
		strlen(response.toStyledString().c_str()) + 1, 0);
	int a = 0;
	if(fcntl(filefddd[0],F_SETFL,O_NONBLOCK) == -1)
		cout <<"pipefd[0] error!"<<endl;

	while (1)
	{

		memset(buff, 0, 1024);
		name_.clear();
		recv(_clientfd, buff, 1024, 0);
		if (reader.parse(buff, root))
			name_ = root["comm"].asString();
		if(a == 0)
		{
			name_.clear();
			a = 1;
			continue;
		}
		memset(buff, 0, 1024);
		strcpy(buff, name_.c_str());

		fflush(stdin);

		len1 = strlen(buff);

		if (judge_ex(buff, len1, crr) == 1)
		{
			message.clear();
			memset(str, 0, 1024);
			sprintf(str, "\n[%s] Please enter [%s]:", name.c_str(), crr);
			message = str;
			response["statemsg"] = message;
			
			send(_clientfd, response.toStyledString().c_str(),
				strlen(response.toStyledString().c_str()) + 1, 0);
			fflush(stdout);
			continue;
		}

		if (strncmp(buff, "exit", 4) == 0)	exit(0);
		printf("buff = %s\n",buff);
		pid_t pid = fork();
		if (pid == 0)
		{
			buff[strlen(buff)-1] = 0;
			int len = strlen(buff);
			char **p = Separate(buff,len);
			close(1);
			close(2);
			dup2(filefddd[1], 1);
			dup2(filefddd[1], 2);
			execvp(p[0], p);
		}
		
		memset(str, 0, 1024);
		wait(NULL);
		read(filefddd[0], str, sizeof(str));
		cout<<str;
		message.clear();
		message = str;
	
		response["statemsg"] = message;
		send(_clientfd, response.toStyledString().c_str(),
			strlen(response.toStyledString().c_str()) + 1, 0);
		fflush(stdout);

		message.clear();
		memset(str, 0, 1024);
		sprintf(str, "\n[%s] Please enter [%s]:", name.c_str(), crr);
		message = str;
		response["statemsg"] = message;
		send(_clientfd, response.toStyledString().c_str(),
			strlen(response.toStyledString().c_str()) + 1, 0);
//		cout <<"send"<<endl;
		fflush(stdout);
			
	}
}
